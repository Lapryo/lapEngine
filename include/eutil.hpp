#ifndef EUTIL_HPP
#define EUTIL_HPP

#include <raylib.h>
#include <nlohmann/json.hpp>
#include <box2d/box2d.h>

#include <vector>
#include <string>

namespace lapCore
{
    struct Renderable
    {
        Rectangle space;

        unsigned int zlayer;
        bool isScreenSpace;
        bool visible = true;
        Color tint;

        bool usesUIListVisiblity;

        Renderable();
        Renderable(unsigned int zlayer, bool isScreenSpace, bool visible, Color tint, bool usesUIListVisiblity)
            : zlayer(zlayer), isScreenSpace(isScreenSpace), visible(visible), tint(tint), usesUIListVisiblity(usesUIListVisiblity) {}
        Renderable(nlohmann::json &json);
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

    enum class Direction2D
    {
        UP,
        DOWN,
        LEFT,
        RIGHT
    };

    struct Alignment
    {
        HorizontalAlignment horizontal;
        VerticalAlignment vertical;

        Alignment();
        Alignment(HorizontalAlignment horizontal, VerticalAlignment vertical)
            : horizontal(horizontal), vertical(vertical) {}
        Alignment(nlohmann::json &json);
    };

    enum class Axis2D
    {
        HORIZONTAL,
        VERTICAL
    };

    struct FrameVector
    {
        Vector2 scale;
        Vector2 offset;

        FrameVector();
        FrameVector(Vector2 scale, Vector2 offset)
            : scale(scale), offset(offset) {}
        FrameVector(nlohmann::json &json);
    };

    struct Padding
    {
        float top, bottom, left, right;

        Padding();
        Padding(float top, float bottom, float left, float right)
            : top(top), bottom(bottom), left(left), right(right) {}
        Padding(nlohmann::json &json);
    };

    struct UIOrigin
    {
        FrameVector position;
        FrameVector size;
        Vector2 anchor;
        float rotation;

        UIOrigin();
        UIOrigin(FrameVector position, FrameVector size, Vector2 anchor, float rotation)
            : position(position), size(size), anchor(anchor), rotation(rotation) {}
        UIOrigin(nlohmann::json &json);
    };

    Vector2 json_rlvec2(const nlohmann::json &json, const std::string &key);
    Vector3 json_rlvec3(const nlohmann::json &json, const std::string &key);
    Vector4 json_rlvec4(const nlohmann::json &json, const std::string &key);

    b2BodyDef json_b2bodydef(const nlohmann::json &json);
    b2ShapeDef json_b2shapedef(const nlohmann::json &json);
    b2Polygon json_b2polygon(const nlohmann::json &json);

    class FileDialogs
    {
    public:
        static std::string OpenFile(std::vector<std::string> filters);
        static std::string SaveFile(std::vector<std::string> filters);
    };

    std::string ReadFileToString(const std::string &filePath);
    void WriteStringToFile(const std::string &filePath, const std::string &data);

    nlohmann::json_abi_v3_12_0::json ReadFileToJsonObject(const std::string &filePath);

    Vector2 GetMouseInViewportSpace(int logicalWidth, int logicalHeight);

    Rectangle UIOriginToRect(UIOrigin origin, int logicalWidth, int logicalHeight);
    Vector2 FrameVectorToVec2(lapCore::FrameVector vector, int logicalWidth, int logicalHeight);

    enum class LogType
    {
        INFO,
        NOTICE,
        WARNING,
        ERROR
    };
    void dbgln(const std::string &message, LogType type = LogType::INFO);
}

#endif