#include "core.hpp"
#include "systems/gui_sys.hpp"

#include <algorithm>
#include <cstddef>

using namespace lapCore;
using namespace ELEMENTS;
using namespace ELEMENTS::Render::UI;

// A drop-in replacement for gui_sys.cpp. No GUISystem header changes required.
// Scene objects are assumed to live below scene->root, as in lapEngine.

namespace
{
    Rectangle CanvasRect(const Scene *scene)
    {
        const Vector2 size = scene->world->window.logical_resolution;
        return {0.0f, 0.0f, size.x, size.y};
    }

    bool SameRect(const Rectangle &a, const Rectangle &b)
    {
        return a.x == b.x && a.y == b.y &&
               a.width == b.width && a.height == b.height;
    }

    // These don't generate on_update signals: derived UI state isn't a
    // modification of authored UITransform properties.
    void InvalidateTransform(registry &registry, Object entity)
    {
        if (auto *p = registry.try_get<UIFrame>(entity))
            p->transform.dirty = true;
        if (auto *p = registry.try_get<UIButton>(entity))
            p->bounds.dirty = true;
        if (auto *p = registry.try_get<UIImage>(entity))
            p->transform.dirty = true;
        if (auto *p = registry.try_get<UITextLabel>(entity))
            p->frame.transform.dirty = true;
    }

    void InvalidateSubtree(Scene *scene, registry &registry, Object entity)
    {
        InvalidateTransform(registry, entity);

        const auto *data = scene->GetObject(entity);
        if (!data)
            return;

        for (const auto &[index, child] : data->children)
            InvalidateSubtree(scene, registry, child);
    }

    void InvalidateDescendants(Scene *scene, registry &registry, Object entity)
    {
        const auto *data = scene->GetObject(entity);
        if (!data)
            return;

        for (const auto &[index, child] : data->children)
            InvalidateSubtree(scene, registry, child);
    }

    // Used when a UIList is removed, so skipping list resets in list-free
    // scenes never leaves obsolete inUIList flags behind.
    void ClearAllListFlags(registry &registry)
    {
        for (auto [entity, frame] : registry.view<UIFrame>().each())
            frame.renderable.inUIList = false;
        for (auto [entity, button] : registry.view<UIButton>().each())
            button.inUIList = false;
        for (auto [entity, text] : registry.view<UITextLabel>().each())
            text.frame.renderable.inUIList = false;
        for (auto [entity, image] : registry.view<UIImage>().each())
            image.sprite.renderable.inUIList = false;
    }

    void InvalidateRegistryDescendants(registry &registry, Object entity)
    {
        const auto *node = registry.try_get<ObjectData>(entity);
        if (!node)
            return;
        for (const auto &[index, child] : node->children)
        {
            InvalidateTransform(registry, child);
            InvalidateRegistryDescendants(registry, child);
        }
    }

    void OnListDestroyed(registry &registry, Object entity)
    {
        ClearAllListFlags(registry);
        // Former list children must return to normal parent-relative layout.
        InvalidateRegistryDescendants(registry, entity);
    }

    // If the UIFrame belonging to a UIList is removed, that list no longer
    // participates in arrangement. Clear stale membership as well.
    void OnFrameDestroyed(registry &registry, Object entity)
    {
        if (registry.all_of<UIList>(entity))
        {
            ClearAllListFlags(registry);
            InvalidateRegistryDescendants(registry, entity);
        }
    }

    // Does not override a UIList's clipping decision when the parent is
    // visible. A hidden ancestor still hides everything below it.
    void ApplyInheritedVisibility(
        registry& registry,
        Object entity,
        bool parentVisible
    )
    {
        if (auto* p = registry.try_get<UIFrame>(entity))
        {
            p->renderable.inheritedVisible =
                !p->renderable.inheritsUI || parentVisible;
        }

        if (auto* p = registry.try_get<UIButton>(entity))
        {
            p->inheritedActive =
                !p->inheritsUI || parentVisible;
        }

        if (auto* p = registry.try_get<UITextLabel>(entity))
        {
            auto& r = p->frame.renderable;
            r.inheritedVisible = !r.inheritsUI || parentVisible;
        }

        if (auto* p = registry.try_get<UIImage>(entity))
        {
            auto& r = p->sprite.renderable;
            r.inheritedVisible = !r.inheritsUI || parentVisible;
        }
    }

    // Performs the same list-assigned visibility changes as the old helper.
    void AssignListVisibility(registry& registry, Object entity, bool visible)
    {
        if (auto* p = registry.try_get<UIFrame>(entity))
            p->renderable.listVisible = visible;

        if (auto* p = registry.try_get<UIButton>(entity))
            p->listActive = visible;

        if (auto* p = registry.try_get<UITextLabel>(entity))
            p->frame.renderable.listVisible = visible;

        if (auto* p = registry.try_get<UIImage>(entity))
            p->sprite.renderable.listVisible = visible;
    }

    void ResolveDirtyTransforms(Scene *scene, registry &registry);
}

UITransform *GetUITransform(registry &registry, entity entity)
{
    if (auto *p = registry.try_get<UIFrame>(entity))
        return &p->transform;
    if (auto *p = registry.try_get<UIButton>(entity))
        return &p->bounds;
    if (auto *p = registry.try_get<UIImage>(entity))
        return &p->transform;
    if (auto *p = registry.try_get<UITextLabel>(entity))
        return &p->frame.transform;
    return nullptr;
}

bool IsUIEntity(registry &registry, entity entity)
{
    return registry.any_of<UIFrame, UIButton, UITextLabel, UIImage>(entity);
}

bool GetUIVisible(registry& registry, Object entity)
{
    if (auto* p = registry.try_get<UIFrame>(entity))
        return isRenderableVisible(p->renderable);

    if (auto* p = registry.try_get<UIButton>(entity))
        return isUIButtonActive(*p);

    if (auto* p = registry.try_get<UITextLabel>(entity))
        return isRenderableVisible(p->frame.renderable);

    if (auto* p = registry.try_get<UIImage>(entity))
        return isRenderableVisible(p->sprite.renderable);

    return true;
}

void SetUIVisible(registry& registry, Object entity, bool visible)
{
    if (auto* frame = registry.try_get<UIFrame>(entity))
        frame->renderable.visible = visible;

    if (auto* button = registry.try_get<UIButton>(entity))
        button->active = visible;

    if (auto* text = registry.try_get<UITextLabel>(entity))
        text->frame.renderable.visible = visible;

    if (auto* image = registry.try_get<UIImage>(entity))
        image->sprite.renderable.visible = visible;
}

void GUISystem::MarkUIDirty(Scene *scene, registry &registry, entity entity)
{
    InvalidateSubtree(scene, registry, entity);
}

void GUISystem::Connect(registry &registry)
{
    registry.on_update<UIFrame>().connect<&GUISystem::OnUIUpdated>(*this);
    registry.on_update<UIButton>().connect<&GUISystem::OnUIUpdated>(*this);
    registry.on_update<UITextLabel>().connect<&GUISystem::OnUIUpdated>(*this);
    registry.on_update<UIImage>().connect<&GUISystem::OnUIUpdated>(*this);

    registry.on_construct<UIFrame>().connect<&GUISystem::OnUIUpdated>(*this);
    registry.on_construct<UIButton>().connect<&GUISystem::OnUIUpdated>(*this);
    registry.on_construct<UITextLabel>().connect<&GUISystem::OnUIUpdated>(*this);
    registry.on_construct<UIImage>().connect<&GUISystem::OnUIUpdated>(*this);

    // Component removals are rare. Cleanup is cheaper than clearing every
    // UIFrame in scenes that contain no UILists at all.
    registry.on_destroy<UIList>().connect<&OnListDestroyed>();
    registry.on_destroy<UIFrame>().connect<&OnFrameDestroyed>();
}

void GUISystem::OnUIUpdated(registry &registry, Object entity)
{
    MarkUIDirty(scene, registry, entity);
}

Rectangle ResolveUI(Scene *scene, registry &registry,
                    entity entity, UITransform &transform)
{
    if (entity == scene->root)
    {
        const Rectangle rect = CanvasRect(scene);
        transform.parentAbsRect = rect;
        transform.absoluteRect = rect;
        transform.dirty = false;
        return rect;
    }

    // Most of the work in a static scene finishes here.
    if (!transform.dirty)
        return transform.absoluteRect;

    const auto *node = scene->GetObject(entity);
    if (!node)
        return {};

    Rectangle parentRect{};
    if (node->parent == scene->root || node->parent == null)
        parentRect = CanvasRect(scene);
    else if (auto *parentTransform = GetUITransform(registry, node->parent))
        parentRect = ResolveUI(scene, registry, node->parent, *parentTransform);
    else
        return {}; // Matches the existing non-UI-parent behavior.

    const float w = parentRect.width * transform.size.scale.x + transform.size.offset.x;
    const float h = parentRect.height * transform.size.scale.y + transform.size.offset.y;
    const float x = parentRect.x + parentRect.width * transform.position.scale.x +
                    transform.position.offset.x - w * transform.anchor.scale.x;
    const float y = parentRect.y + parentRect.height * transform.position.scale.y +
                    transform.position.offset.y - h * transform.anchor.scale.y;

    transform.parentAbsRect = parentRect;
    transform.absoluteRect = {x, y, w, h};
    transform.dirty = false;
    return transform.absoluteRect;
}

namespace
{
    // No GetObject, no parent traversal, no recursive calls for the common
    // 10,000-frame case. Every object is a direct child of the canvas and
    // every entity has exactly one UIFrame; no other UI components exist.
    bool ApplyFlatFrameVisibility(Scene *scene, registry &registry)
    {
        const auto *root = scene->GetObject(scene->root);
        if (!root)
            return false;

        const size_t count = root->children.size() + 1;
        if (registry.storage<ObjectData>().size() != count ||
            registry.storage<UIFrame>().size() != count ||
            !registry.storage<UIButton>().empty() ||
            !registry.storage<UITextLabel>().empty() ||
            !registry.storage<UIImage>().empty() ||
            !registry.storage<UIList>().empty())
            return false;

        const auto *rootFrame = registry.try_get<UIFrame>(scene->root);
        if (!rootFrame)
            return false;

        const bool parentVisible =
            isRenderableVisible(rootFrame->renderable);

        for (auto [entity, frame] : registry.view<UIFrame>().each())
        {
            if (entity == scene->root)
                continue;

            frame.renderable.inheritedVisible =
                !frame.renderable.inheritsUI || parentVisible;
        }
        return true;
    }

    void ResolveDirtyTransforms(Scene *scene, registry &registry)
    {
        // Iterate tightly packed component pools. Only dirty UI elements
        // incur hierarchy lookups, parent recursion, or calculations.
        for (auto [entity, frame] : registry.view<UIFrame>().each())
            if (frame.transform.dirty)
                ResolveUI(scene, registry, entity, frame.transform);

        for (auto [entity, button] : registry.view<UIButton>().each())
            if (button.bounds.dirty)
                ResolveUI(scene, registry, entity, button.bounds);

        for (auto [entity, text] : registry.view<UITextLabel>().each())
            if (text.frame.transform.dirty)
                ResolveUI(scene, registry, entity, text.frame.transform);

        for (auto [entity, image] : registry.view<UIImage>().each())
            if (image.transform.dirty)
                ResolveUI(scene, registry, entity, image.transform);
    }
}

void ApplyUIVisibility(
    Scene* scene,
    registry& registry,
    Object entity,
    bool parentVisible,
    bool hasParent
)
{
    const auto* node = scene->GetObject(entity);
    if (!node)
        return;

    ApplyInheritedVisibility(
        registry,
        entity,
        hasParent ? parentVisible : true
    );

    const bool visible = IsUIEntity(registry, entity)
        ? GetUIVisible(registry, entity)
        : parentVisible;

    for (const auto& [index, child] : node->children)
        ApplyUIVisibility(scene, registry, child, visible, true);
}

void ApplyUIVisibilityHierarchy(Scene *scene, registry &registry)
{
    if (!ApplyFlatFrameVisibility(scene, registry))
        ApplyUIVisibility(scene, registry, scene->root, true, false);
}

Vector2 GetUIListScrollSize(const UIList &list, const Rectangle &listRect)
{
    return FrameVectorToVec2(list.scrollSize, {listRect.width, listRect.height});
}

Vector2 GetUIListElementSize(const UIList &list, const Rectangle &listRect)
{
    const Vector2 scroll = GetUIListScrollSize(list, listRect);
    return FrameVectorToVec2(list.displaySize, scroll);
}

Rectangle ResolveUITransform(const UITransform &transform, const Rectangle &parentRect)
{
    const float w = parentRect.width * transform.size.scale.x + transform.size.offset.x;
    const float h = parentRect.height * transform.size.scale.y + transform.size.offset.y;
    return {
        parentRect.x + parentRect.width * transform.position.scale.x +
            transform.position.offset.x - w * transform.anchor.scale.x,
        parentRect.y + parentRect.height * transform.position.scale.y +
            transform.position.offset.y - h * transform.anchor.scale.y,
        w, h
    };
}

void GUISystem::ResetInUIList()
{
    // Important: RenderSystem also calls this in the supplied implementation.
    // For the flat stress test, both calls now return in O(1).
    if (scene->objects.storage<UIList>().empty())
        return;
    ClearAllListFlags(scene->objects);
}

float GetUIListSpacing(const UIList &list, size_t elementCount,
                       float elementLength, float contentLength)
{
    if (!elementCount)
        return 0.0f;

    const float free = max(0.0f,
        contentLength - static_cast<float>(elementCount) * elementLength);

    switch (list.spreading)
    {
        case UIListSpreading::EVENLY:
            return free / static_cast<float>(elementCount + 1);
        case UIListSpreading::DISTANCING:
            return elementCount > 1 ? free / static_cast<float>(elementCount - 1) : 0.0f;
        default:
            return 0.0f;
    }
}

float GetUIListStartOffset(const UIList &list, size_t elementCount,
                           float elementLength, float contentLength, float spacing)
{
    if (!elementCount)
        return 0.0f;

    const float free = max(0.0f,
        contentLength - static_cast<float>(elementCount) * elementLength);

    switch (list.spreading)
    {
        case UIListSpreading::BUNCH_MIDDLE: return free * 0.5f;
        case UIListSpreading::BUNCH_END:    return free;
        case UIListSpreading::EVENLY:       return spacing;
        default:                            return 0.0f;
    }
}

namespace
{
    // UIList gives each child a slot. Returns whether the absolute rectangle
    // changed; descendants only need invalidation in that case.
    bool AssignListRect(UITransform &transform, const Rectangle &slot)
    {
        const Rectangle rect = ResolveUITransform(transform, slot);
        const bool changed = !SameRect(transform.absoluteRect, rect) ||
                             !SameRect(transform.parentAbsRect, slot);
        transform.parentAbsRect = slot;
        transform.absoluteRect = rect;
        transform.dirty = false;
        return changed;
    }
}

void ArrangeUIList(Scene *scene, registry &registry, entity listEntity)
{
    auto *list = registry.try_get<UIList>(listEntity);
    auto *frame = registry.try_get<UIFrame>(listEntity);
    const auto *node = scene->GetObject(listEntity);
    if (!list || !frame || !node)
        return;

    const Rectangle viewport = frame->transform.absoluteRect;
    const Vector2 scrollSize = GetUIListScrollSize(*list, viewport);
    const Vector2 cellSize = FrameVectorToVec2(list->displaySize, scrollSize);
    const bool vertical = list->settings.direction == Axis2D::VERTICAL;
    const float cellLength = vertical ? cellSize.y : cellSize.x;
    const float contentLength = vertical ? scrollSize.y : scrollSize.x;
    const size_t count = node->children.size();
    const float spacing = GetUIListSpacing(*list, count, cellLength, contentLength);
    const float start = GetUIListStartOffset(*list, count, cellLength, contentLength, spacing);

    size_t ordinal = 0;
    for (const auto &[index, child] : node->children)
    {
        Rectangle slot{viewport.x, viewport.y, cellSize.x, cellSize.y};
        const float offset = start + static_cast<float>(ordinal++) *
            (cellLength + spacing) - list->scrollOffset;
        if (vertical)
            slot.y += offset;
        else
            slot.x += offset;

        const bool inViewport = CheckCollisionRecs(slot, viewport);
        bool moved = false;

        if (auto *p = registry.try_get<UIFrame>(child))
        {
            p->renderable.inUIList = true;
            moved |= AssignListRect(p->transform, slot);
        }
        if (auto *p = registry.try_get<UIButton>(child))
        {
            p->inUIList = true;
            moved |= AssignListRect(p->bounds, slot);
        }
        if (auto *p = registry.try_get<UITextLabel>(child))
        {
            p->frame.renderable.inUIList = true;
            moved |= AssignListRect(p->frame.transform, slot);
        }
        if (auto *p = registry.try_get<UIImage>(child))
        {
            p->sprite.renderable.inUIList = true;
            moved |= AssignListRect(p->transform, slot);
        }

        AssignListVisibility(registry, child, inViewport);

        // The new child rectangle changes the coordinate system for its
        // descendants. Recompute only if the slot/rectangle has moved.
        if (moved)
            InvalidateDescendants(scene, registry, child);
    }

    // Nested lists use their newly assigned rectangles, not the values
    // computed before their parent list was arranged.
    for (const auto &[index, child] : node->children)
    {
        if (registry.all_of<UIList, UIFrame>(child))
            ArrangeUIList(scene, registry, child);
    }
}

void ArrangeUIListElements(Scene *scene, registry &registry)
{
    auto lists = registry.view<UIList, UIFrame>();
    for (auto [entity, list, frame] : lists.each())
    {
        const auto *node = scene->GetObject(entity);
        if (!node)
            continue;
        // Nested lists are processed recursively by their parent list.
        if (node->parent != null && registry.all_of<UIList, UIFrame>(node->parent))
            continue;
        ArrangeUIList(scene, registry, entity);
    }
}

void HandleButtonInputs(Scene *scene, registry &registry)
{
    auto buttons = registry.view<UIButton>();
    if (buttons.begin() == buttons.end())
        return;

    const Vector2 mouse = GetMouseInViewportSpace(scene->world->window.logical_resolution);
    const bool left = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    const bool right = IsMouseButtonPressed(MOUSE_RIGHT_BUTTON);
    const bool middle = IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON);

    for (auto [entity, button] : buttons.each())
    {
        if (!isUIButtonActive(button))
            continue;

        const bool hovered = CheckCollisionPointRec(mouse, button.bounds.absoluteRect);
        if (hovered)
        {
            if (left)
            {
                cout << "firing event: "
                          << EventRegistry::reverseLookup[button.eventCallbacks.leftClick] << '\n';
                EventRegistry::Fire(scene, button.eventCallbacks.leftClick, entity);
            }
            if (right)
                EventRegistry::Fire(scene, button.eventCallbacks.rightClick, entity);
            if (middle)
                EventRegistry::Fire(scene, button.eventCallbacks.middleClick, entity);
            if (!button.mouseHovering)
                EventRegistry::Fire(scene, button.eventCallbacks.mouseEnter, entity);

            button.mouseHovering = true;
            EventRegistry::Fire(scene, button.eventCallbacks.mouseHover, entity);
        }
        else
        {
            if (button.mouseHovering)
                EventRegistry::Fire(scene, button.eventCallbacks.mouseExit, entity);
            button.mouseHovering = false;
        }
    }
}

void HandleUIListScroll(float deltaTime, Scene *scene, registry &registry)
{
    const float wheel = GetMouseWheelMove();
    if (wheel == 0.0f)
        return;

    const Vector2 mouse = GetMouseInViewportSpace(scene->world->window.logical_resolution);
    for (auto [entity, list, frame] : registry.view<UIList, UIFrame>().each())
    {
        const Rectangle viewport = frame.transform.absoluteRect;
        if (!CheckCollisionPointRec(mouse, viewport))
            continue;

        const Vector2 content = GetUIListScrollSize(list, viewport);
        const bool vertical = list.settings.direction == Axis2D::VERTICAL;
        const float maxScroll = max(0.0f, vertical ?
            content.y - viewport.height : content.x - viewport.width);

        list.scrollOffset = clamp(
            list.scrollOffset - wheel * list.settings.scrollSpeed * deltaTime * 1000.0f,
            0.0f, maxScroll);
    }
}

void GUISystem::Update(float deltaTime, registry &registry)
{
    const Rectangle viewport = CanvasRect(scene);
    const Object root = scene->root;
    auto *rootFrame = registry.try_get<UIFrame>(root);
    if (!rootFrame)
        rootFrame = &scene->AddElement<UIFrame>(root);

    if (!SameRect(rootFrame->transform.absoluteRect, viewport))
        MarkUIDirty(scene, registry, root);

    rootFrame->transform.size.offset = {viewport.width, viewport.height};
    rootFrame->transform.parentAbsRect = viewport;
    rootFrame->transform.absoluteRect = viewport;
    rootFrame->transform.dirty = false;
    rootFrame->renderable.isScreenSpace = true;
    rootFrame->renderable.inheritsUI = true;

    // Empty UIList storage is a very common (and very cheap) fast path.
    const bool hasLists = !registry.storage<UIList>().empty();
    if (hasLists)
        ResetInUIList();

    ResolveDirtyTransforms(scene, registry);

    if (hasLists)
    {
        HandleUIListScroll(deltaTime, scene, registry);
        ArrangeUIListElements(scene, registry);
        // List placement can invalidate transforms of nested non-list UI.
        ResolveDirtyTransforms(scene, registry);
    }

    ApplyUIVisibilityHierarchy(scene, registry);
    HandleButtonInputs(scene, registry);
}
