#pragma once

#include "system.hpp"
using namespace lapCore;

const std::string UIGradient_fragShaderSrc = R"(
#version 330

out vec4 finalColor;

uniform vec2 rectPosition;
uniform vec2 rectSize;

uniform vec4 colors[16];
uniform int colorCount;
uniform float angle;

void main()
{
    if (colorCount <= 0)
    {
        finalColor = vec4(0.0);
        return;
    }

    if (colorCount == 1)
    {
        finalColor = colors[0];
        return;
    }

    vec2 uv = (gl_FragCoord.xy - rectPosition) / rectSize;

    vec2 direction = vec2(
        cos(angle),
        sin(angle)
    );

    vec2 centered = uv - vec2(0.5);

    float projection = dot(centered, direction);

    float t = projection + 0.5;
    t = clamp(t, 0.0, 1.0);

    float scaled = t * float(colorCount - 1);

    int index = int(floor(scaled));
    float localT = fract(scaled);

    if (index >= colorCount - 1)
    {
        index = colorCount - 2;
        localT = 1.0;
    }

    finalColor = mix(
        colors[index],
        colors[index + 1],
        localT
    );
})";

class RenderSystem : public System
{
public:
    // TODO: Add support for models and custom 2d shapes
    enum class RenderType
    {
        Sprite,
        Text,
        Rect,
        Image,
        Ellipse,
        Line,
        Polygon,
        Model
    };

    struct RenderEntry
    {
        Object entity;
        int zlayer, ySort;
        bool isScreenSpace;
        RenderType type;
    };

    std::vector<RenderEntry> worldSpace, screenSpace;

    ~RenderSystem()
    {
        UnloadShader(UIGradient_fragShader);
    }

    RenderSystem(
        Scene *scene, 
        unsigned int order
    ) : System(order, scene) {
        UIGradient_fragShader = LoadShaderFromMemory(nullptr, UIGradient_fragShaderSrc.c_str());

        gradientColorLocation =
            GetShaderLocation(
                UIGradient_fragShader,
                "colors"
            );

        gradientColorCountLocation =
            GetShaderLocation(
                UIGradient_fragShader,
                "colorCount"
            );

        gradientAngleLocation =
            GetShaderLocation(
                UIGradient_fragShader,
                "angle"
            );

        gradientRectPositionLocation =
            GetShaderLocation(UIGradient_fragShader, "rectPosition");

        gradientRectSizeLocation =
            GetShaderLocation(UIGradient_fragShader, "rectSize");
    }

    void Update(
        float deltaTime, 
        entt::registry &reg
    ) override;

    void Connect(entt::registry &registry);
    void OnRenderableUpdated(
        entt::registry &registry, 
        Object entity
    );

    void RebuildRenderList(entt::registry &registry);

    std::vector<RenderEntry> renderList;
    bool needsResort = true;

    std::string GetName() const override 
    { return "RenderSystem"; }

private:
    Shader UIGradient_fragShader;

    int gradientColorLocation{-1};
    int gradientColorCountLocation{-1};
    int gradientAngleLocation{-1};

    int gradientRectPositionLocation{-1};
    int gradientRectSizeLocation{-1};
};