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
#include <iostream>
#include <fstream>

using json = nlohmann::json;

namespace lapCore
{
    inline std::unordered_map<entt::id_type, std::string> hashRegistry;

    inline constexpr entt::hashed_string HASH(const char* name)
    {
        entt::hashed_string hash{name};

        hashRegistry.try_emplace(hash.value(), name);

        return hash;
    }

    inline constexpr entt::id_type HASH_ID(const char* name)
    {
        return HASH(name).value();
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
                std::vector<float> frameTimes{0.f};
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
        }

        namespace Functions
        {
            using namespace Render::UI;

            std::string ReadFileToString(const std::string &filePath, bool withPrefix = true);
            void WriteStringToFile(const std::string &filePath, const std::string &data);

            nlohmann::json ReadFileToJsonObject(const std::string &filePath, bool withPrefix = true);

            bool IsPointInViewportSpace(::Vector2 point, ::Vector2 logicalResolution, ::Camera2D* camera);
            bool IsRectangleInViewportSpace(Rectangle rect, ::Vector2 logicalResolution, ::Camera2D* camera);

            Rectangle GetCameraViewport(::Vector2 logicalResolution, ::Camera2D* camera);

            Vector2 GetMouseInViewportSpace(::Vector2 logicalResolution);

            Rectangle UITransformToRect(UITransform transform, ::Vector2 logicalResolution);
            Vector2 FrameVectorToVec2(FrameVector vector, ::Vector2 logicalResolution);

            void dbgln(const std::string &message, Other::LogType type = Other::LogType::INFO);
            void dbgln(const std::string &message, const char* colorCode, Other::LogType type = Other::LogType::INFO);

            template <typename T>
            void BinarySerialize(const T& value, const std::string& filePath)
            {
                std::ofstream stream(GetApplicationDirectory() + filePath, std::ios::binary);

                dbgln("Saving to: " + filePath, Other::LogType::INFO);
                dbgln("Stream open: " + std::to_string(stream.is_open()), Other::LogType::INFO);
                dbgln("Stream good: " + std::to_string(stream.good()), Other::LogType::INFO);

                if (!stream.is_open())
                {
                    throw std::runtime_error(
                        "Could not open save file: " + filePath
                    );
                }

                cereal::BinaryOutputArchive archive(stream);
                archive(value);
            }

            template <typename T>
            T BinaryDeserialize(const std::string& filePath)
            {
                T value;

                std::ifstream stream(GetApplicationDirectory() + filePath, std::ios::binary);

                if (!stream.is_open())
                {
                    throw std::runtime_error("Failed to open file for binary deserialization");
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