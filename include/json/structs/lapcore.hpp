#pragma once

#include "project/project.hpp"
#include "elements.hpp"

using namespace lapCore;
using namespace EUTIL;
using namespace ELEMENTS;

using namespace EUTIL::Render;
using namespace ELEMENTS::Render;

using namespace EUTIL::Render::UI;
using namespace ELEMENTS::Render::UI;

using namespace EUTIL::Other;
using namespace ELEMENTS::Other;

using namespace Functions;

using namespace ELEMENTS::Physics;

using namespace Math;

namespace lapCore::JSON
{
    // EUTIL

    template <>
    struct Serializer<Renderable>
    {
        static json to_json(const Renderable &r, SerializeContext *ctx = nullptr);
        static void from_json(Renderable &r, const json &j);
    };

    template <>
    struct Serializer<Axis2D>
    {
        static json to_json(const Axis2D &a, SerializeContext *ctx = nullptr);
        static void from_json(Axis2D &a, const json &j);
    };

    template <>
    struct Serializer<ScrollSettings>
    {
        static json to_json(const ScrollSettings &s, SerializeContext *ctx = nullptr);
        static void from_json(ScrollSettings &s, const json &j);
    };

    template <>
    struct Serializer<Animated>
    {
        static json to_json(const Animated &a, SerializeContext *ctx = nullptr);
        static void from_json(Animated &a, const json &j);
    };

    template <>
    struct Serializer<HorizontalAlignment>
    {
        static json to_json(const HorizontalAlignment &h, SerializeContext *ctx = nullptr);
        static void from_json(HorizontalAlignment &h, const json &j);
    };

    template <>
    struct Serializer<VerticalAlignment>
    {
        static json to_json(const VerticalAlignment &v, SerializeContext *ctx = nullptr);
        static void from_json(VerticalAlignment &v, const json &j);
    };

    template <>
    struct Serializer<Direction2D>
    {
        static json to_json(const Direction2D &d, SerializeContext *ctx = nullptr);
        static void from_json(Direction2D &d, const json &j);
    };

    template <>
    struct Serializer<Alignment>
    {
        static json to_json(const Alignment &a, SerializeContext *ctx = nullptr);
        static void from_json(Alignment &a, const json &j);
    };

    template <>
    struct Serializer<FrameVector>
    {
        static json to_json(const FrameVector &v, SerializeContext *ctx = nullptr);
        static void from_json(FrameVector &v, const json &j);
    };

    template <>
    struct Serializer<Padding>
    {
        static json to_json(const Padding &p, SerializeContext *ctx = nullptr);
        static void from_json(Padding &p, const json &j);
    };

    template <>
    struct Serializer<UITransform>
    {
        static json to_json(const UITransform &t, SerializeContext *ctx = nullptr);
        static void from_json(UITransform &t, const json &j);
    };

    // ELEMENTS

    template <>
    struct Serializer<Transform2D>
    {
        static json to_json(const Transform2D &t, SerializeContext *ctx = nullptr);
        static void from_json(Transform2D &t, const json &j);
    };

    template <>
    struct Serializer<Physics2D>
    {
        static json to_json(const Physics2D &p, SerializeContext *ctx = nullptr);
        static void from_json(Physics2D &p, const json &j);
    };

    template <>
    struct Serializer<SoundPoint>
    {
        static json to_json(const SoundPoint &s, SerializeContext *ctx = nullptr);
        static void from_json(SoundPoint &s, const json &j);
    };

    template <>
    struct Serializer<UIFrame>
    {
        static json to_json(const UIFrame &f, SerializeContext *ctx = nullptr);
        static void from_json(UIFrame &f, const json &j);
    };

    template <>
    struct Serializer<UIListSpreading>
    {
        static json to_json(const UIListSpreading &s, SerializeContext *ctx = nullptr);
        static void from_json(UIListSpreading &s, const json &j);
    };

    template <>
    struct Serializer<UIList>
    {
        static json to_json(const UIList &l, SerializeContext *ctx = nullptr);
        static void from_json(UIList &l, const json &j);
    };

    template <>
    struct Serializer<UIGradient>
    {
        static json to_json(const UIGradient &g, SerializeContext *ctx = nullptr);
        static void from_json(UIGradient &g, const json &j);
    };

    template <>
    struct Serializer<Sprite>
    {
        static json to_json(const Sprite &s, SerializeContext *ctx = nullptr);
        static void from_json(Sprite &s, const json &j);
    };

    template <>
    struct Serializer<UIImage>
    {
        static json to_json(const UIImage &i, SerializeContext *ctx = nullptr);
        static void from_json(UIImage &i, const json &j);
    };

    template <>
    struct Serializer<UITextLabel>
    {
        static json to_json(const UITextLabel &l, SerializeContext *ctx = nullptr);
        static void from_json(UITextLabel &l, const json &j);
    };

    template <>
    struct Serializer<UIButtonEventCallbacks>
    {
        static json to_json(const UIButtonEventCallbacks &c, SerializeContext *ctx = nullptr);
        static void from_json(UIButtonEventCallbacks &c, const json &j);
    };

    template <>
    struct Serializer<UIButton>
    {
        static json to_json(const UIButton &b, SerializeContext *ctx = nullptr);
        static void from_json(UIButton &b, const json &j);
    };

    template <>
    struct Serializer<Tags>
    {
        static json to_json(const Tags &t, SerializeContext *ctx = nullptr);
        static void from_json(Tags &t, const json &j);
    };

    template <>
    struct Serializer<Parameter>
    {
        static json to_json(const Parameter &param, SerializeContext *ctx = nullptr);
        static void from_json(Parameter &param, const json &j);
    };

    template <>
    struct Serializer<Parameters>
    {
        static json to_json(const Parameters &p, SerializeContext *ctx = nullptr);
        static void from_json(Parameters &p, const json &j);
    };

    template <>
    struct Serializer<Script>
    {
        static json to_json(const Script &s, SerializeContext *ctx = nullptr);
        static void from_json(Script &s, const json &j);
    };
}