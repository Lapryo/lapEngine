#include "core.hpp"
#include "systems/gui_sys.hpp"

using namespace lapCore;
using namespace ELEMENTS;
using namespace ELEMENTS::Render::UI;

// ============================================================
// UI TRANSFORM HELPERS
// ============================================================

UITransform *GetUITransform(
    entt::registry &registry,
    entt::entity entity)
{
    if (auto *frame = registry.try_get<UIFrame>(entity))
        return &frame->transform;

    if (auto *button = registry.try_get<UIButton>(entity))
        return &button->bounds;

    if (auto *text = registry.try_get<UITextLabel>(entity))
        return &text->frame.transform;

    if (auto *image = registry.try_get<UIImage>(entity))
        return &image->transform;

    return nullptr;
}

// ============================================================
// UI VISUAL HELPERS
// ============================================================

bool IsUIEntity(
    entt::registry &registry,
    entt::entity entity)
{
    return registry.all_of<UIFrame>(entity) ||
           registry.all_of<UIButton>(entity) ||
           registry.all_of<UITextLabel>(entity) ||
           registry.all_of<UIImage>(entity);
}

bool GetUIVisible(
    entt::registry &registry,
    entt::entity entity)
{
    if (auto *frame =
            registry.try_get<UIFrame>(entity))
    {
        return frame->renderable.visible;
    }

    if (auto *button =
            registry.try_get<UIButton>(entity))
    {
        return button->active;
    }

    if (auto *text =
            registry.try_get<UITextLabel>(entity))
    {
        return text->frame.renderable.visible;
    }

    if (auto *image =
            registry.try_get<UIImage>(entity))
    {
        return image->sprite.renderable.visible;
    }

    return true;
}

void SetUIVisible(
    entt::registry &registry,
    entt::entity entity,
    bool visible)
{
    if (auto *frame =
            registry.try_get<UIFrame>(entity))
    {
        if (frame->renderable.inheritsUI)
            frame->renderable.visible = visible;
    }

    if (auto *button =
            registry.try_get<UIButton>(entity))
    {
        if (button->inheritsUI)
            button->active = visible;
    }

    if (auto *text =
            registry.try_get<UITextLabel>(entity))
    {
        if (text->frame.renderable.inheritsUI)
            text->frame.renderable.visible = visible;
    }

    if (auto *image =
            registry.try_get<UIImage>(entity))
    {
        if (image->sprite.renderable.inheritsUI)
            image->sprite.renderable.visible = visible;
    }
}

// ============================================================
// MARK UI TRANSFORM DIRTY
// ============================================================

void GUISystem::MarkUIDirty(
    Scene *scene,
    entt::registry &registry,
    entt::entity entity)
{
    UITransform *transform =
        GetUITransform(
            registry,
            entity);

    if (!transform)
        return;

    transform->dirty = true;

    auto entry =
        scene->FindEntry(entity);

    if (!entry)
        return;

    for (const auto &child :
         entry->children)
    {
        MarkUIDirty(
            scene,
            registry,
            child.second.object);
    }
}

// ============================================================
// ENTT UI UPDATE SIGNALS
// ============================================================

void GUISystem::Connect(
    entt::registry &registry)
{
    registry.on_update<UIFrame>()
        .connect<&GUISystem::OnUIUpdated>(*this);

    registry.on_update<UIButton>()
        .connect<&GUISystem::OnUIUpdated>(*this);

    registry.on_update<UITextLabel>()
        .connect<&GUISystem::OnUIUpdated>(*this);

    registry.on_update<UIImage>()
        .connect<&GUISystem::OnUIUpdated>(*this);
}

void GUISystem::OnUIUpdated(
    entt::registry &registry,
    Object entity)
{
    MarkUIDirty(
        scene,
        registry,
        entity);
}

// ============================================================
// RESOLVE UI TRANSFORM
// ============================================================

Rectangle ResolveUI(
    Scene *scene,
    entt::registry &registry,
    entt::entity entity)
{
    UITransform *transform =
        GetUITransform(
            registry,
            entity);

    if (!transform)
        return {};

    auto entry =
        scene->FindEntry(entity);

    if (!entry)
        return {};

    const bool isRoot =
        entry->parent.object == entt::null;

    // --------------------------------------------------------
    // Cached result.
    // --------------------------------------------------------

    if (!isRoot && !transform->dirty)
        return transform->absoluteRect;

    // --------------------------------------------------------
    // Resolve parent rectangle.
    // --------------------------------------------------------

    Rectangle parentRect{};

    if (isRoot)
    {
        parentRect = {
            0.0f,
            0.0f,
            scene->world->window.logical_resolution.x,
            scene->world->window.logical_resolution.y};
    }
    else
    {
        parentRect =
            ResolveUI(
                scene,
                registry,
                entry->parent.object);
    }

    transform->parentAbsRect =
        parentRect;

    // --------------------------------------------------------
    // Calculate size.
    // --------------------------------------------------------

    const float width =
        parentRect.width *
            transform->size.scale.x +
        transform->size.offset.x;

    const float height =
        parentRect.height *
            transform->size.scale.y +
        transform->size.offset.y;

    // --------------------------------------------------------
    // Calculate position.
    //
    // absoluteRect is already the final top-left rectangle.
    // The renderer must NOT apply the anchor again.
    // --------------------------------------------------------

    const float x =
        parentRect.x +
        parentRect.width *
            transform->position.scale.x +
        transform->position.offset.x -
        width *
            transform->anchor.scale.x;

    const float y =
        parentRect.y +
        parentRect.height *
            transform->position.scale.y +
        transform->position.offset.y -
        height *
            transform->anchor.scale.y;

    // --------------------------------------------------------
    // Cache result.
    // --------------------------------------------------------

    transform->absoluteRect = {
        x,
        y,
        width,
        height};

    transform->dirty = false;

    return transform->absoluteRect;
}

// ============================================================
// APPLY UI VISIBILITY
// ============================================================
//
// inheritsUI:
//
//     child.visible = parent.visible
//
// UIList does NOT create another visibility inheritance rule.
//
// UIList directly controls the visibility of its direct
// elements based on whether their element slot intersects
// the list viewport.
//
// ApplyUIVisibility() then propagates that final visibility
// through the normal scene hierarchy.
//
// Example:
//
//     row = false
//       |
//       +-- slot = false
//             |
//             +-- image = false
//             +-- text  = false
//
// ============================================================

void ApplyUIVisibility(
    Scene *scene,
    entt::registry &registry,
    entt::entity entity,
    bool parentVisible,
    bool hasParent)
{
    auto entry =
        scene->FindEntry(entity);

    if (!entry)
        return;

    if (hasParent)
    {
        SetUIVisible(
            registry,
            entity,
            parentVisible);
    }

    const bool currentVisible =
        GetUIVisible(
            registry,
            entity);

    for (const auto &child :
         entry->children)
    {
        ApplyUIVisibility(
            scene,
            registry,
            child.second.object,
            currentVisible,
            true);
    }
}

// ============================================================
// APPLY VISIBILITY TO ROOT UI OBJECTS
// ============================================================

void ApplyUIVisibilityHierarchy(
    Scene *scene,
    entt::registry &registry)
{
    /*
     * An entity can theoretically contain more than one UI
     * component. Use a set so that a root entity is only
     * processed once.
     */
    std::unordered_set<entt::entity> processed;

    auto ProcessRoots =
        [&](auto view)
    {
        for (auto entity : view)
        {
            auto entry =
                scene->FindEntry(entity);

            if (!entry)
                continue;

            if (entry->parent.object != entt::null)
                continue;

            if (!IsUIEntity(
                    registry,
                    entity))
            {
                continue;
            }

            if (!processed.insert(entity).second)
                continue;

            ApplyUIVisibility(
                scene,
                registry,
                entity,
                true,
                false);
        }
    };

    ProcessRoots(
        registry.view<UIFrame>());

    ProcessRoots(
        registry.view<UIButton>());

    ProcessRoots(
        registry.view<UITextLabel>());

    ProcessRoots(
        registry.view<UIImage>());
}

// ============================================================
// UI LIST HELPERS
// ============================================================

Vector2 GetUIListScrollSize(
    const UIList &list,
    const Rectangle &listRect)
{
    return FrameVectorToVec2(
        list.scrollSize,
        {listRect.width,
         listRect.height});
}

Vector2 GetUIListElementSize(
    const UIList &list,
    const Rectangle &listRect)
{
    const Vector2 scrollSize =
        GetUIListScrollSize(
            list,
            listRect);

    return FrameVectorToVec2(
        list.displaySize,
        scrollSize);
}

// ============================================================
// RESOLVE TRANSFORM INSIDE RECTANGLE
// ============================================================

Rectangle ResolveUITransform(
    const UITransform &transform,
    const Rectangle &parentRect)
{
    const float width =
        parentRect.width *
            transform.size.scale.x +
        transform.size.offset.x;

    const float height =
        parentRect.height *
            transform.size.scale.y +
        transform.size.offset.y;

    const float x =
        parentRect.x +
        parentRect.width *
            transform.position.scale.x +
        transform.position.offset.x -
        width *
            transform.anchor.scale.x;

    const float y =
        parentRect.y +
        parentRect.height *
            transform.position.scale.y +
        transform.position.offset.y -
        height *
            transform.anchor.scale.y;

    return {
        x,
        y,
        width,
        height};
}

// ============================================================
// RESET UI LIST STATE
// ============================================================

void GUISystem::ResetInUIList()
{
    auto &registry =
        scene->objects;

    for (auto entity :
         registry.view<UITextLabel>())
    {
        auto &text =
            registry.get<UITextLabel>(entity);

        text.frame.renderable.inUIList =
            false;
    }

    for (auto entity :
         registry.view<UIFrame>())
    {
        auto &frame =
            registry.get<UIFrame>(entity);

        frame.renderable.inUIList =
            false;
    }

    for (auto entity :
         registry.view<UIButton>())
    {
        auto &button =
            registry.get<UIButton>(entity);

        button.inUIList =
            false;
    }

    for (auto entity :
         registry.view<UIImage>())
    {
        auto &image =
            registry.get<UIImage>(entity);

        image.sprite.renderable.inUIList =
            false;
    }
}

// ============================================================
// ARRANGE ONE UI LIST
// ============================================================
//
// UIList responsibilities:
//
// 1. Determine element slots.
// 2. Move each element according to scrollOffset.
// 3. Resolve the element's UI transform relative to its slot.
// 4. Determine whether the element is inside the viewport.
// 5. Set the element's visibility accordingly.
// 6. Recursively arrange nested UILists.
//
// The normal hierarchy visibility pass happens later.
//
// ============================================================

void ArrangeUIList(
    Scene *scene,
    entt::registry &registry,
    entt::entity listEntity)
{
    auto *list =
        registry.try_get<UIList>(
            listEntity);

    auto *frame =
        registry.try_get<UIFrame>(
            listEntity);

    if (!list || !frame)
        return;

    auto entry =
        scene->FindEntry(listEntity);

    if (!entry)
        return;

    const Rectangle listRect =
        frame->transform.absoluteRect;

    const Vector2 scrollSize =
        GetUIListScrollSize(
            *list,
            listRect);

    const Vector2 elementSize =
        FrameVectorToVec2(
            list->displaySize,
            scrollSize);

    const bool vertical =
        list->settings.direction ==
        Axis2D::VERTICAL;

    // --------------------------------------------------------
    // Arrange each direct child.
    // --------------------------------------------------------

    for (size_t index = 0;
         index < entry->children.size();
         ++index)
    {
        const entt::entity childEntity =
            entry->children[index].object;

        const float contentOffset =
            static_cast<float>(index) *
            (vertical
                 ? elementSize.y
                 : elementSize.x);

        Rectangle elementRect = {
            listRect.x,
            listRect.y,
            elementSize.x,
            elementSize.y};

        if (vertical)
        {
            elementRect.y +=
                contentOffset -
                list->scrollOffset;
        }
        else
        {
            elementRect.x +=
                contentOffset -
                list->scrollOffset;
        }

        // ----------------------------------------------------
        // Determine whether the ELEMENT is inside the
        // list viewport.
        //
        // Use elementRect, not the child's final rectangle.
        // ----------------------------------------------------

        const bool elementVisible =
            CheckCollisionRecs(
                elementRect,
                listRect);

        // ----------------------------------------------------
        // Apply list membership.
        // ----------------------------------------------------

        if (auto *text =
                registry.try_get<UITextLabel>(
                    childEntity))
        {
            text->frame.renderable.inUIList =
                true;

            text->frame.transform.parentAbsRect =
                elementRect;

            text->frame.transform.absoluteRect =
                ResolveUITransform(
                    text->frame.transform,
                    elementRect);

            text->frame.transform.dirty =
                false;
        }

        if (auto *childFrame =
                registry.try_get<UIFrame>(
                    childEntity))
        {
            childFrame->renderable.inUIList =
                true;

            childFrame->transform.parentAbsRect =
                elementRect;

            childFrame->transform.absoluteRect =
                ResolveUITransform(
                    childFrame->transform,
                    elementRect);

            childFrame->transform.dirty =
                false;
        }

        if (auto *button =
                registry.try_get<UIButton>(
                    childEntity))
        {
            button->inUIList =
                true;

            button->bounds.parentAbsRect =
                elementRect;

            button->bounds.absoluteRect =
                ResolveUITransform(
                    button->bounds,
                    elementRect);

            button->bounds.dirty =
                false;
        }

        if (auto *image =
                registry.try_get<UIImage>(
                    childEntity))
        {
            image->sprite.renderable.inUIList =
                true;

            image->transform.parentAbsRect =
                elementRect;

            image->transform.absoluteRect =
                ResolveUITransform(
                    image->transform,
                    elementRect);

            image->transform.dirty =
                false;
        }

        // ----------------------------------------------------
        // IMPORTANT:
        //
        // UIList controls the visibility of its direct
        // element.
        //
        // ApplyUIVisibilityHierarchy() will propagate this
        // state to the element's descendants afterward.
        // ----------------------------------------------------

        SetUIVisible(
            registry,
            childEntity,
            elementVisible);
    }

    // --------------------------------------------------------
    // Recursively arrange nested UILists.
    // --------------------------------------------------------

    for (const auto &child :
         entry->children)
    {
        const entt::entity childEntity =
            child.second.object;

        if (!registry.all_of<UIList, UIFrame>(
                childEntity))
        {
            continue;
        }

        ArrangeUIList(
            scene,
            registry,
            childEntity);
    }
}

// ============================================================
// ARRANGE ALL UI LISTS
// ============================================================

void ArrangeUIListElements(
    Scene *scene,
    entt::registry &registry)
{
    auto listView =
        registry.view<UIList, UIFrame>();

    for (auto [entity, list, frame] :
         listView.each())
    {
        auto entry =
            scene->FindEntry(entity);

        if (!entry)
            continue;

        const entt::entity parent =
            entry->parent.object;

        // ----------------------------------------------------
        // If this list is inside another UIList, its parent
        // list will arrange it recursively.
        // ----------------------------------------------------

        if (parent != entt::null &&
            registry.all_of<UIList>(parent))
        {
            continue;
        }

        // ----------------------------------------------------
        // Otherwise this is the top-level UIList in its
        // UIList hierarchy.
        //
        // It can still have a normal UI parent.
        //
        // Example:
        //
        // inventory-menu
        //     |
        //     +-- inventory-menu-row-list
        //
        // inventory-menu-row-list is NOT a scene root,
        // but it IS a UIList root.
        // ----------------------------------------------------

        ArrangeUIList(
            scene,
            registry,
            entity);
    }
}

/*void ArrangeUIGrid(Scene* scene, entt::registry &registry, Object gridObject)
{
    
}

void ArrangeUIGridElements(Scene* scene, entt::registry &registry)
{
    auto gridView = registry.view<UIGrid, UIFrame>();

    for (auto [entity, grid, frame] : gridView.each())
    {
        auto entry = scene->FindEntry(entity);
        if (!entry) continue;

        if (entry->parent.object != entt::null && registry.all_of<UIGrid>(entry->parent.object)) continue;

        ArrangeUIGrid(scene, registry, entity);
    }
}*/

// ============================================================
// BUTTON INPUT
// ============================================================

void HandleButtonInputs(
    Scene *scene,
    entt::registry &registry)
{
    auto buttonView =
        registry.view<UIButton>();

    const Vector2 mouse =
        GetMouseInViewportSpace(
            scene->world->window.logical_resolution);

    for (auto entity : buttonView)
    {
        auto &button =
            buttonView.get<UIButton>(
                entity);

        if (!button.active)
            continue;

        const Rectangle rect =
            button.bounds.absoluteRect;

        const bool hovered =
            CheckCollisionPointRec(
                mouse,
                rect);

        if (hovered)
        {
            if (IsMouseButtonPressed(
                    MOUSE_LEFT_BUTTON))
            {
                std::cout
                    << "firing event: "
                    << EventRegistry::reverseLookup[button.eventCallbacks.leftClick]
                    << '\n';

                EventRegistry::Fire(
                    scene,
                    button.eventCallbacks.leftClick,
                    entity);
            }

            if (IsMouseButtonPressed(
                    MOUSE_RIGHT_BUTTON))
            {
                EventRegistry::Fire(
                    scene,
                    button.eventCallbacks.rightClick,
                    entity);
            }

            if (IsMouseButtonPressed(
                    MOUSE_MIDDLE_BUTTON))
            {
                EventRegistry::Fire(
                    scene,
                    button.eventCallbacks.middleClick,
                    entity);
            }

            if (!button.mouseHovering)
            {
                EventRegistry::Fire(
                    scene,
                    button.eventCallbacks.mouseEnter,
                    entity);
            }

            button.mouseHovering =
                true;

            EventRegistry::Fire(
                scene,
                button.eventCallbacks.mouseHover,
                entity);
        }
        else
        {
            if (button.mouseHovering)
            {
                EventRegistry::Fire(
                    scene,
                    button.eventCallbacks.mouseExit,
                    entity);
            }

            button.mouseHovering =
                false;
        }
    }
}

// ============================================================
// UI LIST SCROLL
// ============================================================

void HandleUIListScroll(
    float deltaTime,
    Scene *scene,
    entt::registry &registry)
{
    const float wheel =
        GetMouseWheelMove();

    if (wheel == 0.0f)
        return;

    const Vector2 mouse =
        GetMouseInViewportSpace(
            scene->world->window.logical_resolution);

    auto uilistView =
        registry.view<UIList, UIFrame>();

    for (auto [entity, list, frame] :
         uilistView.each())
    {
        const Rectangle listRect =
            frame.transform.absoluteRect;

        if (!CheckCollisionPointRec(
                mouse,
                listRect))
        {
            continue;
        }

        const Vector2 scrollSize =
            GetUIListScrollSize(
                list,
                listRect);

        const Vector2 viewportSize = {
            listRect.width,
            listRect.height};

        float maxScroll = 0.0f;

        if (list.settings.direction ==
            Axis2D::VERTICAL)
        {
            maxScroll =
                scrollSize.y -
                viewportSize.y;
        }
        else
        {
            maxScroll =
                scrollSize.x -
                viewportSize.x;
        }

        maxScroll =
            std::max(
                0.0f,
                maxScroll);

        /*
         * Positive scrollOffset means moving through the
         * content in the positive direction.
         */
        list.scrollOffset -=
            wheel *
            list.settings.scrollSpeed *
            deltaTime *
            1000.0f;

        list.scrollOffset =
            std::clamp(
                list.scrollOffset,
                0.0f,
                maxScroll);
    }
}

// ============================================================
// GUI UPDATE
// ============================================================

void GUISystem::Update(
    float deltaTime,
    entt::registry &registry)
{
    // ========================================================
    // 1. Reset transient UIList state.
    // ========================================================

    ResetInUIList();

    // ========================================================
    // 2. Resolve normal UI hierarchy.
    // ========================================================

    for (auto entity :
         registry.view<UIFrame>())
    {
        ResolveUI(
            scene,
            registry,
            entity);
    }

    for (auto entity :
         registry.view<UIButton>())
    {
        ResolveUI(
            scene,
            registry,
            entity);
    }

    for (auto entity :
         registry.view<UITextLabel>())
    {
        ResolveUI(
            scene,
            registry,
            entity);
    }

    for (auto entity :
         registry.view<UIImage>())
    {
        ResolveUI(
            scene,
            registry,
            entity);
    }

    // ========================================================
    // 3. Handle scrolling.
    // ========================================================

    HandleUIListScroll(
        deltaTime,
        scene,
        registry);

    // ========================================================
    // 4. Arrange UILists.
    //
    // Sets the visibility of direct list
    // elements based on whether their slots intersect
    // the viewport.
    // ========================================================

    ArrangeUIListElements(scene, registry);



    // ========================================================
    // 5. Propagate visibility through the scene hierarchy.
    //
    // This handles:
    //
    //     row
    //       -> slot
    //           -> image
    //           -> text
    //
    // as well as arbitrarily nested UILists.
    // ========================================================

    ApplyUIVisibilityHierarchy(
        scene,
        registry);

    // ========================================================
    // 6. Handle input using final UI state.
    // ========================================================

    HandleButtonInputs(
        scene,
        registry);
}