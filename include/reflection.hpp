#pragma once

#include <entt/entt.hpp>
#include <nlohmann/json.hpp>

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <iostream>

#include "resourcemanager.hpp"

namespace lapCore
{
    using json = nlohmann::json;
    using Object = entt::entity;

    struct IProjectElementData;
    template <typename T>
    struct ProjectElementData;

    struct SerializeContext
    {
        ResourceManager* resources = nullptr;
        std::string objectName = "";
    };

    template<typename T>
    constexpr entt::id_type GetTypeID()
    {
        return entt::type_hash<T>::value();
    }

    namespace JSON
    {
        template<typename T>
        struct Serializer
        {
            static json to_json(const T&, SerializeContext* = nullptr);
            static void from_json(T&, const json&);
        };

        inline void Merge(json& base, const json& other)
        {
            for (auto it = other.begin(); it != other.end(); ++it)
            {
                const auto& key = it.key();
                if (base.contains(key) && base[key].is_object() && it.value().is_object())
                    Merge(base[key], it.value());
                else
                    base[key] = it.value();
            }
        }
    }

    struct ReflectionEntry
    {
        std::string name;

        std::function<json(const void*, SerializeContext*)> to_json;
        std::function<void(void*, const json&)> from_json;

        std::function<void(entt::registry&, Object, void*)> emplace;
        std::function<void(entt::registry&, Object)> erase;
        std::function<void(void*, const void*)> copy;

        std::function<
            std::unique_ptr<IProjectElementData>(
                const json&,
                const std::string&
            )
        > create_project_data;
        std::function<
            json(const IProjectElementData*, SerializeContext*)
        > serialize_project_data;
    };

    class Reflection
    {
    public:

        Reflection() = delete;

        inline static std::unordered_map<
            entt::id_type,
            ReflectionEntry
        > registry;

        inline static std::unordered_map<
            std::string,
            entt::id_type
        > reverseLookup;

        template<typename T>
        static void Register(const std::string& name)
        {
            const entt::id_type id = GetTypeID<T>();

            reverseLookup[name] = id;

            registry[id] = ReflectionEntry
            {
                name,
                [](const void* ptr, SerializeContext* ctx) -> json
                {
                    return JSON::Serializer<T>::to_json(
                        *static_cast<const T*>(ptr),
                        ctx
                    );
                },
                [](void* ptr, const json& j)
                {
                    JSON::Serializer<T>::from_json(
                        *static_cast<T*>(ptr),
                        j
                    );
                },
                [](entt::registry& reg, Object obj, void* data)
                {
                    reg.emplace<T>(
                        obj,
                        *static_cast<T*>(data)
                    );
                },
                [](entt::registry& reg, Object obj)
                {
                    reg.remove<T>(obj);
                },
                [](void* dst, const void* src)
                {
                    *static_cast<T*>(dst) =
                        *static_cast<const T*>(src);
                },
                [](const json& j, const std::string& typeName)
                    -> std::unique_ptr<IProjectElementData>
                {
                    auto ptr =
                        std::make_unique<ProjectElementData<T>>();

                    ptr->type = typeName;

                    JSON::Serializer<T>::from_json(
                        ptr->data,
                        j
                    );

                    return ptr;
                },

                [](const IProjectElementData* element, SerializeContext* ctx) -> json
                {
                    auto typed =
                        static_cast<const ProjectElementData<T>*>(element);

                    return JSON::Serializer<T>::to_json(
                        typed->data,
                        ctx
                    );
                }
            };
        }
        
        static ReflectionEntry* TryGet(entt::id_type id)
        {
            auto it = registry.find(id);

            if (it == registry.end())
                return nullptr;

            return &it->second;
        }

        template<typename T>
        static ReflectionEntry* TryGet()
        {
            return TryGet(GetTypeID<T>());
        }

        static ReflectionEntry* TryGet(const std::string& name)
        {
            auto it = reverseLookup.find(name);

            if (it == reverseLookup.end())
                return nullptr;

            return TryGet(it->second);
        }
    };
}