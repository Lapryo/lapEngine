#include "elements.hpp"
#include "reflection.hpp"
#include "json/structs.hpp"
#include "systems.hpp"
#include "project/deserializer.hpp"

#include "json/unpacker.hpp"

#include <nlohmann/json.hpp>
#include <iostream>

using namespace lapCore;
using namespace EUTIL;
using namespace ELEMENTS;

using namespace ELEMENTS::Render;
using namespace EUTIL::Render;
using namespace ELEMENTS::Render::UI;
using namespace EUTIL::Render::UI;
using namespace ELEMENTS::Other;
using namespace EUTIL::Other;

using namespace ELEMENTS::Physics;

using namespace Functions;
using namespace std;

size_t GetMainScene(const nlohmann::json_abi_v3_12_0::json &projectJson)
{
    if (jsonContainsArray(projectJson, "scenes"))
    {
        for (size_t i = 0; i < projectJson["scenes"].size(); ++i)
        {
            const auto &sceneEntry = projectJson["scenes"].at(i);
            string sceneName;

            if (sceneEntry.is_string())
            {
                string sceneFilePath = sceneEntry.get<string>();
                size_t lastSlash = sceneFilePath.find_last_of("/\\");
                size_t lastDot = sceneFilePath.find_last_of('.');
                if (lastDot != string::npos)
                    sceneName = sceneFilePath.substr(lastSlash + 1, lastDot - lastSlash - 1);
                else
                    sceneName = sceneFilePath.substr(lastSlash + 1);
            }
            else if (sceneEntry.is_object())
                sceneName = sceneEntry.value("name", "Unnamed Scene");
            else
            {
                dbgln("Scene entry was neither a string (file path) nor an object, skipping...", LogType::WARNING);
                continue;
            }

            if (sceneName == "main")
            {
                dbgln("Main scene found: " + sceneName + " at index " + to_string(i), LogType::INFO);
                return i;
            }
        }
    }
    else
        dbgln("[WARNING] Project did not contain any scenes to search for the main scene", LogType::WARNING);
    return 0; // Return -1 if no main scene is found
}

namespace lapCore
{
    Project LoadProject(const filesystem::path& path)
    {
        // First we must register the default structs
        if (!defaultStructsRegistered)
            defaultStructsRegistered = RegisterDefaultStructs();

        // Then we unpack the entire project directory
        json projectJson = UnpackMultidirectoryProject(path);
        if (projectJson.is_null())
        {
            throw runtime_error("[ERROR] Project JSON returned null when loading project");
        }

        // Then make it
        return MakeProject(projectJson);
    }

    bool RegisterDefaultStructs()
    {
        // ELEMENTS
        Reflection::Register<Transform2D>("transform-2d");
        Reflection::Register<Physics2D>("physics-2d");
        Reflection::Register<SoundPoint>("sound-point");
        Reflection::Register<UIFrame>("ui-frame");
        Reflection::Register<UIList>("ui-list");
        Reflection::Register<UIGradient>("ui-gradient");
        //Reflection::Register<UIGrid>("ui-grid");
        Reflection::Register<Sprite>("sprite");
        Reflection::Register<UIImage>("ui-image");
        Reflection::Register<UITextLabel>("ui-text-label");
        Reflection::Register<UIButton>("ui-button");
        Reflection::Register<Tags>("tags");
        Reflection::Register<Parameter>("parameter");
        Reflection::Register<Parameters>("parameters");
        Reflection::Register<Script>("script");

        // EUTIL
        Reflection::Register<HorizontalAlignment>("horizontal-alignment");
        Reflection::Register<VerticalAlignment>("vertical-alignment");
        Reflection::Register<Alignment>("alignment");
        Reflection::Register<Renderable>("renderable");
        Reflection::Register<Axis2D>("axis-2d");
        Reflection::Register<ScrollSettings>("scroll-settings");
        Reflection::Register<Animated>("animated");
        Reflection::Register<Direction2D>("direction-2d");
        Reflection::Register<FrameVector>("frame-vector");
        Reflection::Register<Padding>("padding");
        Reflection::Register<UITransform>("ui-transform");

        // RAYLIB
        Reflection::Register<Camera2D>("rl-camera-2d");
        Reflection::Register<Vector2>("rl-vector2");
        Reflection::Register<Color>("rl-color");
        Reflection::Register<Rectangle>("rl-rectangle");

        // BOX2D
        Reflection::Register<b2BodyDef>("b2-body-def");
        Reflection::Register<b2ShapeDef>("b2-shape-def");
        Reflection::Register<b2Polygon>("b2-polygon");
        Reflection::Register<b2Filter>("b2-filter");
        Reflection::Register<b2SurfaceMaterial>("b2-surface-material");
        Reflection::Register<b2MotionLocks>("b2-motion-locks");
        Reflection::Register<b2BodyType>("b2-body-type");
        Reflection::Register<b2Vec2>("b2-vec2");
        Reflection::Register<b2Rot>("b2-rot");

        return true;
    }

    ProjectObjectData::ProjectObjectData(const ProjectObjectData &other)
        : name(other.name), parent(other.parent), index(other.index)
    {
        elements.reserve(other.elements.size());
        for (const auto &element : other.elements)
        {
            elements.push_back(element ? element->Clone() : nullptr);
        }
    }

    ProjectObjectData& ProjectObjectData::operator=(const ProjectObjectData& other)
    {
        if (this == &other)
            return *this;

        // Clone before modifying this object (clone failures are harmless).
        vector<ElementDataPtr> clonedElements;
        clonedElements.reserve(other.elements.size());
        for (const auto& element : other.elements)
            clonedElements.push_back(element ? element->Clone() : nullptr);

        name = other.name;
        parent = other.parent;
        index = other.index;
        elements = std::move(clonedElements);
        return *this;
    }
}