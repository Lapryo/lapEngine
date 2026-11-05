#pragma once

#include <raylib.h>
#include <nlohmann/json.hpp>
#include <box2d/box2d.h>
#include <cereal/archives/binary.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/cereal.hpp>

#include <entt/entt.hpp>

#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>
#include <fstream>

using json = nlohmann::json;
using namespace entt;
using namespace std;

namespace lapCore
{
    inline unordered_map<id_type, string> hashRegistry;

    inline hashed_string HASH(const char* name)
    {
        hashed_string hash{name};

        hashRegistry.try_emplace(hash.value(), name);

        return hash;
    }

    inline id_type HASH_ID(const char* name)
    {
        auto hash = HASH(name);
        return hash.value();
    }

    inline const string GET_FORMER_HASHED(id_type id)
    {
        auto it = hashRegistry.find(id);
        if (it != hashRegistry.end())
            return it->second;
        return "";
    }

    static json SerializeEventID(entt::id_type id)
    {
        std::string name = GET_FORMER_HASHED(id);

        if (!name.empty())
            return name;

        return id;
    }

    static entt::id_type DeserializeEventID(const json& j)
    {
        if (j.is_number_unsigned())
            return j.get<entt::id_type>();

        if (j.is_string())
        {
            std::string name = j.get<std::string>();

            if (name.empty())
                return 0;

            return entt::hashed_string{name.c_str()}.value();
        }

        return 0;
    }

    #define DEBUG_LEVEL DebugLevel::HIGH

    namespace EUTIL
    {
        namespace Console
        {
            constexpr const char* _RESET   = "\033[0m";
            constexpr const char* _RED     = "\033[31m";
            constexpr const char* _GREEN   = "\033[32m";
            constexpr const char* _YELLOW  = "\033[33m";
            constexpr const char* _BLUE    = "\033[34m";
            constexpr const char* _MAGENTA = "\033[35m";
            constexpr const char* _CYAN    = "\033[36m";
            constexpr const char* _WHITE   = "\033[37m";
        }

        namespace Math
        {
            enum class Axis2D
            {
                HORIZONTAL,
                VERTICAL
            };

            enum class HorizontalAlignment
            {
                LEFT,
                MIDDLE,
                RIGHT
            };

            enum class VerticalAlignment
            {
                TOP,
                MIDDLE,
                BOTTOM
            };

            struct Alignment
            {
                HorizontalAlignment horizontal{HorizontalAlignment::LEFT};
                VerticalAlignment vertical{VerticalAlignment::TOP};
            };

            enum class Direction2D
            {
                UP,
                DOWN,
                LEFT,
                RIGHT
            };

            template <typename T>
            struct Range
            {
                T upper, lower;
            };
        }

        namespace Render
        {
            using namespace Math;

            namespace UI
            {
                struct ScrollSettings
                {
                    bool freeMode{false};
                    Axis2D direction{Axis2D::HORIZONTAL};
                    bool allowHorizontalScrolling{true};
                    bool allowVerticalScrolling{true};
                    float scrollSpeed{1.f};
                };

                struct FrameVector
                {
                    Vector2 scale{0.f, 0.f};
                    Vector2 offset{0.f, 0.f};
                };

                struct FrameRect // TODO: Needs JSON implementation
                {
                    FrameVector position;
                    FrameVector size;
                };

                struct Padding
                {
                    float top{0.f}, bottom{0.f}, left{0.f}, right{0.f};
                };

                enum class UIListSpreading
                {
                    BUNCH_START, // 1 - 2 - 3 --
                    BUNCH_MIDDLE, // - 1 - 2 - 3 -
                    BUNCH_END, // -- 1 - 2 - 3
                    EVENLY, // - 1 - 2 - 3 - .. NOT THE SAME AS BUNCH_MIDDLE, ELEMENTS ARE DISTRIBUTED EQUALLY WITH SPACE ON BOTH ENDS
                    DISTANCING // 1 -- 2 -- 3
                };

                struct UITransform
                {
                    // Should I store parent absolute position + size here?
                    Rectangle parentAbsRect;
                    Rectangle absoluteRect;

                    FrameVector position{};
                    FrameVector size{};
                    FrameVector anchor{};
                    float rotation{0.f};

                    bool dirty{true}; // If true, the parentAbsRect and absoluteRect will be recalculated on the next update
                };
            }

            struct Renderable
            {
                // Not to be changed, i mean you can, but it wont do anything, itll just be overwritten by the render system
                Rectangle drawRect{0, 0, 0, 0};

                int zlayer{0}, ySort{0};
                bool isScreenSpace{false};
                bool visible{true};
                Color tint{255, 255, 255, 255};

                bool inheritsUI{false};

                bool culling = true;

                // Not serializable, only for runtime
                bool inUIList{false};
            };

            struct Animated
            {
                bool active{true};
                unsigned int index{0}, columns{0};
                vector<float> frameTimes{0.f};
            };
        }

        namespace Other
        {
            enum class DebugLevel
            {
                NONE, // As the name suggests, nothing will be printed
                LOW, // Minimal debugging information (only errors)
                MEDIUM, // Moderate debugging information (errors, warnings & notices)
                HIGH, // Extensive debugging information, covers nearly everything (errors, warnings, notices, info)
            };

            enum class LogType
            {
                INFO,
                NOTICE,
                WARNING,
                ERROR
            };

            enum class SuccessType
            {
                SUCCESS,
                PARTIAL,
                FAILURE
            };

            enum class ShiftMode
            {
                CLUMPING,
                SUBSEQUENT
            };
        }

        namespace Functions
        {
            using namespace Render::UI;

            string ReadFileToString(const filesystem::path &filePath, bool withPrefix = true);
            void WriteStringToFile(const string &filePath, const string &data);

            nlohmann::json ReadFileToJsonObject(const string &filePath, bool withPrefix = true);

            bool IsPointInViewportSpace(::Vector2 point, ::Vector2 logicalResolution, ::Camera2D* camera);
            bool IsRectangleInViewportSpace(Rectangle rect, ::Vector2 logicalResolution, ::Camera2D* camera);

            Rectangle GetCameraViewport(::Vector2 logicalResolution, ::Camera2D* camera);

            Vector2 GetMouseInViewportSpace(::Vector2 logicalResolution);

            Rectangle UITransformToRect(UITransform transform, ::Vector2 logicalResolution);
            Vector2 FrameVectorToVec2(FrameVector vector, ::Vector2 logicalResolution);

            void dbgln(const string &message, Other::LogType type = Other::LogType::INFO);
            void dbgln(const string &message, const char* colorCode, Other::LogType type = Other::LogType::INFO);

            template <typename T>
            void ShiftMap(
                map<unsigned int, T>& map,
                Other::ShiftMode shiftMode,
                unsigned int pos,
                const T& replacementValue,
                bool right = true
            )
            {
                vector<unsigned int> keys;

                if (right)
                {
                    // Find the keys that need shifting.
                    unsigned int expected = pos;

                    for (auto it = map.lower_bound(pos); it != map.end(); ++it)
                    {
                        if (shiftMode == Other::ShiftMode::CLUMPING &&
                            it->first != expected)
                            break;

                        if (it->first == UINT_MAX)
                            throw overflow_error("Cannot shift beyond UINT_MAX");

                        keys.push_back(it->first);

                        if (shiftMode == Other::ShiftMode::CLUMPING)
                            ++expected;
                    }

                    // Shift right, highest index first.
                    for (auto it = keys.rbegin(); it != keys.rend(); ++it)
                    {
                        auto node = map.extract(*it);
                        node.key() = *it + 1;
                        map.insert(std::move(node));
                    }

                    map.insert_or_assign(pos, replacementValue);
                }
                else
                {
                    // Remove the element at pos.
                    map.erase(pos);

                    if (pos == UINT_MAX)
                        return;

                    unsigned int expected = pos + 1;

                    for (auto it = map.upper_bound(pos); it != map.end(); ++it)
                    {
                        if (shiftMode == Other::ShiftMode::CLUMPING &&
                            it->first != expected)
                            break;

                        keys.push_back(it->first);

                        if (shiftMode == Other::ShiftMode::CLUMPING)
                            ++expected;
                    }

                    // Shift left, lowest index first.
                    for (auto key : keys)
                    {
                        auto node = map.extract(key);
                        node.key() = key - 1;
                        map.insert(std::move(node));
                    }
                }
            }

            template <typename T>
            void BinarySerialize(const T& value, const string& filePath)
            {
                ofstream stream(GetApplicationDirectory() + filePath, ios::binary);

                dbgln("Saving to: " + filePath, Other::LogType::INFO);
                dbgln("Stream open: " + to_string(stream.is_open()), Other::LogType::INFO);
                dbgln("Stream good: " + to_string(stream.good()), Other::LogType::INFO);

                if (!stream.is_open())
                {
                    throw runtime_error(
                        "Could not open save file: " + filePath
                    );
                }

                cereal::BinaryOutputArchive archive(stream);
                archive(value);
            }

            template <typename T>
            T BinaryDeserialize(const string& filePath)
            {
                T value;

                ifstream stream(GetApplicationDirectory() + filePath, ios::binary);

                if (!stream.is_open())
                {
                    throw runtime_error("Failed to open file for binary deserialization");
                }

                cereal::BinaryInputArchive archive(stream);
                archive(value);

                return value;
            }

            namespace Convert
            {
                namespace Vec2
                {
                    inline b2Vec2 box2d(Vector2 vec)
                    {
                        return {vec.x, vec.y};
                    }
                    inline Vector2 raylib(b2Vec2 vec)
                    {
                        return {vec.x, vec.y};
                    }
                }
            }
        }
    }
}