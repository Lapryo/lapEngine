#pragma once

#include <nlohmann/json.hpp>
#include <entt/entt.hpp>

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace lapCore
{
    using namespace std;
    using namespace entt;
    using json = nlohmann::json;
    using Object = entity;

    struct ResourceManager;

    struct IProjectElementData;

    template <typename T>
    struct ProjectElementData;

    struct SerializeContext
    {
        ResourceManager* resources = nullptr;
        string objectName = "";
    };

    template <typename T>
    constexpr id_type GetTypeID()
    {
        return type_hash<T>::value();
    }

    namespace mmJSON
    {
        template <typename T>
        struct Serializer
        {
            static json to_json(
                const T&,
                SerializeContext* = nullptr
            );

            static void from_json(T&, const json&);
        };

        inline void Merge(json& base, const json& other)
        {
            for (auto it = other.begin(); it != other.end(); ++it)
            {
                const auto& key = it.key();

                if (base.contains(key) &&
                    base[key].is_object() &&
                    it.value().is_object())
                {
                    Merge(base[key], it.value());
                }
                else
                {
                    base[key] = it.value();
                }
            }
        }
    }

    template <typename Format, typename T>
    struct MiddlemanSerializer;

    // Adapter for JSON serializers
    template <typename T>
    struct MiddlemanSerializer<json, T>
    {
        static json Serialize(const T& data, SerializeContext* ctx)
        {
            return mmJSON::Serializer<T>::to_json(data, ctx);
        }

        static void Deserialize(T& data, const json& j)
        {
            mmJSON::Serializer<T>::from_json(data, j);
        }
    };

    template <typename Format = json>
    struct MiddlemanEntry
    {
        string name;

        function<Format(const void*, SerializeContext*)> serialize;
        function<void(void*, const Format&)> deserialize;

        function<void(entt::registry&, Object, void*)> emplace;
        function<void(entt::registry&, Object)> erase;
        function<void(void*, const void*)> copy;

        function<
            unique_ptr<IProjectElementData>(
                const Format&,
                const string&
            )
        > create_project_data;

        function<
            Format(const IProjectElementData*, SerializeContext*)
        > serialize_project_data;
    };

    class Middleman
    {
    public:

        Middleman() = delete;

        template <typename Format = json>
        inline static unordered_map<
            id_type,
            MiddlemanEntry<Format>
        > registry;

        inline static unordered_map<
            string,
            id_type
        > reverseLookup;

        template <typename T, typename Format = json>
        static void Register(const string& name)
        {
            using Serializer = MiddlemanSerializer<Format, T>;

            const id_type id = GetTypeID<T>();

            reverseLookup[name] = id;

            registry<Format>[id] = MiddlemanEntry<Format>
            {
                name,

                [](const void* ptr, SerializeContext* ctx) -> Format
                {
                    return Serializer::Serialize(
                        *static_cast<const T*>(ptr),
                        ctx
                    );
                },

                [](void* ptr, const Format& data)
                {
                    Serializer::Deserialize(
                        *static_cast<T*>(ptr),
                        data
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

                [](const Format& data, const string& typeName)
                    -> unique_ptr<IProjectElementData>
                {
                    auto ptr = make_unique<ProjectElementData<T>>();

                    ptr->type = typeName;

                    Serializer::Deserialize(
                        ptr->data,
                        data
                    );

                    return ptr;
                },

                [](const IProjectElementData* element,
                SerializeContext* ctx) -> Format
                {
                    auto typed =
                        static_cast<const ProjectElementData<T>*>(element);

                    return Serializer::Serialize(
                        typed->data,
                        ctx
                    );
                }
            };
        }

        template <typename Format = json>
        static MiddlemanEntry<Format>* TryGet(id_type id)
        {
            auto it = registry<Format>.find(id);

            if (it == registry<Format>.end())
                return nullptr;

            return &it->second;
        }

        template <typename T, typename Format = json>
        static MiddlemanEntry<Format>* TryGet()
        {
            return TryGet<Format>(GetTypeID<T>());
        }

        template <typename Format = json>
        static MiddlemanEntry<Format>* TryGet(const string& name)
        {
            auto it = reverseLookup.find(name);

            if (it == reverseLookup.end())
                return nullptr;

            return TryGet<Format>(it->second);
        }
    };
}