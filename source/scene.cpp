#include "scene.hpp"
#include "world.hpp"

using namespace lapCore;

#include "systems/physics_sys.hpp"

ObjectInfo lapCore::ObjectContainer::AddObject(entt::hashed_string name, entt::hashed_string parent, int childIndex, bool fromPrefab)
{
    ObjectEntry entry;
    entry.info.object = this->objects.create();
    entry.parent.id = parent.value();
    entry.childIndex = childIndex;
    entry.fromPrefab = fromPrefab;

    std::string finalName = name.data();

    if (objectMap.contains(name.value()))
    {
        const std::string baseName = finalName;
        int i = 1;

        do
        {
            finalName =
                baseName + "_" + std::to_string(i++);
        }
        while (
            objectMap.contains(HASH_ID(finalName.c_str()))
        );

        name = HASH(finalName.c_str());
    }

    entry.info.id = name.value();
    
    if (parent.data() != nullptr)
    {
        auto it = objectMap.find(parent.value());
        if (it != objectMap.end())
        {
            if (childIndex == -1)
                childIndex = it->second.children.size() + 1;

            it->second.children[childIndex] = entry.info;
        }
    }

    objectMap[name.value()] = entry;
    lookup[entry.info.id] = finalName;

    return entry.info;
}

void lapCore::ObjectContainer::RemoveObject(entt::id_type id)
{
    auto object = *FindObject(id);

    objects.destroy(object);
    objectMap.erase(id);
}

void lapCore::ObjectContainer::RemoveObject(Object object)
{
    auto entry = FindEntry(object);
    if (!entry) return;

    RemoveObject(*entry);
}

void lapCore::ObjectContainer::RemoveObject(ObjectEntry entry)
{
    objects.destroy(entry.info.object);
    objectMap.erase(entry.info.id);
}

Object *lapCore::ObjectContainer::FindObject(entt::id_type id)
{
    auto it = objectMap.find(id);
    if (it != objectMap.end())
        return &it->second.info.object;
    return nullptr;
}

ObjectEntry *lapCore::ObjectContainer::FindEntry(entt::id_type id)
{
    for (auto& objEntry : objectMap)
    {
        if (objEntry.second.info.id == id)
            return &objEntry.second;
    }
    return nullptr;
}

ObjectEntry *lapCore::ObjectContainer::FindEntry(Object object)
{
    for (auto& objEntry : objectMap)
    {
        if (objEntry.second.info.object == object)
            return &objEntry.second;
    }
    return nullptr;
}

ObjectInfo lapCore::ObjectContainer::CloneObject(Object object, std::string newName)
{
    auto* srcObject = FindEntry(object);
    if (!srcObject)
        return {};

    auto srcObjNameIt = lookup.find(srcObject->info.id);
    if (srcObjNameIt == lookup.end())
        return {};

    if (newName.empty())
        newName = srcObjNameIt->second;

    // Get the original parent name if one exists
    std::string parentName;

    auto srcObjParentNameIt = lookup.find(srcObject->parent.id);
    if (srcObjParentNameIt != lookup.end())
        parentName = srcObjParentNameIt->second;

    std::function<ObjectInfo(Object, const std::string&, const std::string&)> cloneRecursive;

    cloneRecursive =
        [&](Object srcEntity,
            const std::string& requestedName,
            const std::string& dstParentName) -> ObjectInfo
    {
        auto* srcEntry = FindEntry(srcEntity);
        if (!srcEntry)
            return {};

        auto srcNameIt = lookup.find(srcEntry->info.id);
        if (srcNameIt == lookup.end())
            return {};

        std::string dstName = requestedName;

        if (dstName.empty())
            dstName = srcNameIt->second;

        ObjectInfo dstObject = AddObject(
            HASH(dstName.c_str()),
            HASH(dstParentName.c_str()),
            -1,
            srcEntry->fromPrefab
        );

        // Get the actual name AddObject chose.
        // Important if "enemy" became "enemy_1".
        auto dstNameIt = lookup.find(dstObject.id);
        if (dstNameIt == lookup.end())
            return {};

        const std::string actualDstName = dstNameIt->second;

        // Copy all components
        for (auto&& curr : objects.storage())
        {
            auto& storage = curr.second;

            if (storage.contains(srcEntry->info.object))
            {
                storage.push(
                    dstObject.object,
                    storage.value(srcEntry->info.object)
                );
            }
        }

        // Clone children under the NEW object
        for (const auto& [childIndex, child] : srcEntry->children)
        {
            cloneRecursive(
                child.object,
                "",
                actualDstName
            );
        }

        return dstObject;
    };

    return cloneRecursive(
        object,
        newName,
        parentName
    );
}

ObjectInfo lapCore::ObjectContainer::CloneObjectFromContainer(
    ObjectContainer& container,
    Object object,
    entt::hashed_string newName,
    entt::hashed_string newParent
)
{
    auto* srcObject = container.FindEntry(object);
    if (!srcObject)
        return {};

    auto srcObjNameIt = container.lookup.find(srcObject->info.id);
    if (srcObjNameIt == container.lookup.end())
        return {};

    if (newName.value() == HASH_ID(""))
        newName = HASH(srcObjNameIt->second.c_str());

    std::function<ObjectInfo(
        Object,
        entt::hashed_string,
        entt::hashed_string
    )> cloneRecursive;

    cloneRecursive =
        [&](Object srcEntity,
            entt::hashed_string dstName,
            entt::hashed_string dstParentName) -> ObjectInfo
    {
        auto* srcEntry = container.FindEntry(srcEntity);
        if (!srcEntry)
            return {};

        auto srcNameIt = container.lookup.find(srcEntry->info.id);
        if (srcNameIt == container.lookup.end())
            return {};

        if (dstName.value() == HASH_ID(""))
            dstName = HASH(srcNameIt->second.c_str());

        ObjectInfo dstObject = AddObject(
            dstName,
            dstParentName,
            -1,
            srcEntry->fromPrefab
        );

        auto dstNameIt = lookup.find(dstObject.id);
        if (dstNameIt == lookup.end())
            return {};

        entt::hashed_string actualDstName =
            HASH(dstNameIt->second.c_str());

        for (auto&& curr : container.objects.storage())
        {
            entt::id_type componentType = curr.first;
            auto& storage = curr.second;

            if (storage.contains(srcEntry->info.object))
            {
                AddElement(
                    dstObject.object,
                    componentType,
                    storage.value(srcEntry->info.object)
                );
            }
        }

        for (const auto& [childIndex, childInfo] : srcEntry->children)
        {
            cloneRecursive(
                childInfo.object,
                HASH(""),
                actualDstName
            );
        }

        return dstObject;
    };

    return cloneRecursive(
        object,
        newName,
        newParent
    );
}

void lapCore::ObjectContainer::AddObjectsFromProjectData(std::vector<ProjectObjectData> objects)
{
    for (const auto& object_data : objects)
    {
        auto prefabObject = AddObject(HASH(object_data.name.c_str()), HASH(object_data.parent.c_str()), object_data.child_index, true);
        for (const auto& element : object_data.elements)
        {
            if (!element.get())
                continue;

            AddElement(HASH(object_data.name.c_str()), element->GetTypeID(), element->GetDataPtr());
        }
    }
}

void lapCore::Scene::LoadMapObjects(Map &map)
{
    for (const auto& layer : map.layers)
    {
        for (const auto& tileData : layer.second.second)
        {
            std::string tileName = "tile_" + 
                std::to_string(layer.first) + "_" + 
                std::to_string(tileData.id) + "_" + 
                std::to_string(tileData.rect.x) + "_" + 
                std::to_string(tileData.rect.y);

            auto tile = AddObject(HASH(tileName.c_str()), HASH(""), -1);
            map.objects[layer.first] = tile.id;

            // Add tile properties as elements here

            auto tileset = world->resources.tilesets.TryGet(map.tilesets[tileData.tileset].id);
            if (!tileset) continue;

            Transform2D tileTransform;
            tileTransform.position = {tileData.rect.x, tileData.rect.y};

            if (tileData.id != 0)
            {
                Sprite tileSprite;
                tileSprite.textureID = tileset->textureID;
                tileSprite.animated.active = false;

                unsigned int localID =
                    tileData.id - map.tilesets[tileData.tileset].firstGID;

                int column = localID % tileset->columns;
                int row    = localID / tileset->columns;

                tileSprite.sourceRect = {
                    (float)(column * map.tileSize.x),
                    (float)(row * map.tileSize.y),
                    (float)map.tileSize.x,
                    (float)map.tileSize.y
                };

                tileSprite.destRect = tileSprite.sourceRect;
                tileSprite.destRect.x = 0.f;
                tileSprite.destRect.y = 0.f;
                tileSprite.destRect.width = (float)tileData.rect.width;
                tileSprite.destRect.height = (float)tileData.rect.height;

                tileSprite.renderable.zlayer = layer.first;

                float rotation = 0.f;

                const bool h = tileData.flipX;
                const bool v = tileData.flipY;
                const bool d = tileData.flipD;

                bool flipX = false;
                bool flipY = false;

                if (!d)
                {
                    // Normal Tiled flips
                    flipX = h;
                    flipY = v;
                }
                else
                {
                    // Tiled diagonal transformations

                    if (!h && !v)
                    {
                        // Diagonal only
                        rotation = 90.f;
                        flipY = true;
                    }
                    else if (h && !v)
                    {
                        // Diagonal + horizontal
                        rotation = 90.f;
                    }
                    else if (!h && v)
                    {
                        // Diagonal + vertical
                        rotation = -90.f;
                    }
                    else // h && v
                    {
                        // Diagonal + horizontal + vertical
                        rotation = 90.f;
                        flipX = true;
                    }
                }

                if (flipX)
                    tileSprite.sourceRect.width *= -1.f;

                if (flipY)
                    tileSprite.sourceRect.height *= -1.f;

                tileTransform.rotation = rotation;

                tileSprite.renderable.tint = {255, 255, 255, (unsigned char)(255 * tileData.opacity)};

                tileSprite.renderable.ySort = tileTransform.position.y + map.tileSize.y;

                AddElement<Sprite>(tile.object, tileSprite);

                if (tileData.isObject)
                {
                    tileTransform.position.x += (tileData.rect.width / 2.f) - (map.tileSize.x / 2.f);
                    tileTransform.position.y += (tileData.rect.height / 2.f) - (map.tileSize.y / 2.f);
                }
            }
            else if (tileData.isObject)
            {
                tileTransform.position.y += map.tileSize.y;
            }

            AddElement<Transform2D>(tile.object, tileTransform);

            auto it = tileData.properties.find("type");
            if (it != tileData.properties.end())
            {
                auto tileType = it->second;
                if (tileType == "solid")
                {
                    Physics2D tilephysics;
                    tilephysics.bodyDef = b2DefaultBodyDef();
                    tilephysics.bodyDef.position = Convert::Vec2::box2d(tileTransform.position);
                    tilephysics.bodyDef.type = b2BodyType::b2_staticBody;
                    tilephysics.shapeDef = b2DefaultShapeDef();
                    tilephysics.polygon = b2MakeBox(((float)tileData.rect.width) / 2.f, ((float)tileData.rect.height) / 2.f);
                    auto physics = AddElement<Physics2D>(tile.object, tilephysics);

                    auto pSys = GetSystem<PhysicsSystem>();
                    pSys->bodyMap[tile.object] = pSys->Create2DBody(tilephysics.bodyDef, tilephysics.shapeDef, tilephysics.polygon);
                }
                else if (tileType == "teleport")
                {
                    Physics2D tilephysics;
                    tilephysics.bodyDef = b2DefaultBodyDef();
                    tilephysics.bodyDef.position = Convert::Vec2::box2d(tileTransform.position);
                    tilephysics.bodyDef.type = b2BodyType::b2_staticBody;
                    tilephysics.shapeDef = b2DefaultShapeDef();
                    tilephysics.polygon = b2MakeBox(((float)tileData.rect.width) / 2.f, ((float)tileData.rect.height) / 2.f);
                    tilephysics.shapeDef.isSensor = true;

                    auto physics = AddElement<Physics2D>(tile.object, tilephysics);

                    auto pSys = GetSystem<PhysicsSystem>();
                    pSys->bodyMap[tile.object] = pSys->Create2DBody(tilephysics.bodyDef, tilephysics.shapeDef, tilephysics.polygon);
                }
            }

            AddElement<Map::TileProperties>(tile.object, tileData.properties);
        }
    }
}

void lapCore::ObjectContainer::AddElement(entt::hashed_string objectName, entt::id_type elementType, void *elementData)
{
    auto it = objectMap.find(objectName.value());
    if (it == objectMap.end())
        return;

    AddElement(it->second.info.object, elementType, elementData);
}

void lapCore::ObjectContainer::AddElement(Object object, entt::id_type elementType, void *elementData)
{
    auto entry = Reflection::TryGet(elementType);
    entry->emplace(objects, object, elementData);
}

void lapCore::ObjectContainer::RemoveElement(entt::id_type id, entt::id_type elementType)
{
    auto it = objectMap.find(id);
    if (it == objectMap.end())
        return;

    RemoveElement(it->second.info.object, elementType);
}

void lapCore::ObjectContainer::RemoveElement(
    Object object,
    entt::id_type elementType
)
{
    auto entry = Reflection::TryGet(elementType);
    if (!entry)
        return;

    auto* storage = objects.storage(elementType);
    if (!storage)
        return;

    if (!storage->contains(object))
        return;

    entry->erase(objects, object);
}

void *lapCore::ObjectContainer::FindElement(entt::id_type id, entt::id_type elementType)
{
    auto it = objectMap.find(id);
    if (it == objectMap.end())
        return nullptr;

    return FindElement(it->second.info.object, elementType);
}

void *lapCore::ObjectContainer::FindElement(Object object, entt::id_type elementType)
{
    auto* storage = objects.storage(elementType);
    if (!storage)
        return nullptr;

    if (!storage->contains(object))
        return nullptr;

    return storage->value(object);
}

void lapCore::ObjectContainer::ClearObjects()
{
    objects.clear();
    objectMap.clear();
}

void lapCore::Scene::Update(float deltaTime, RenderTexture2D &target)
{
    /// Draw to render texture first
    BeginTextureMode(target);
    ClearBackground(WHITE);

    for (auto &[order, system] : systems)
    {
        if (!system || !system->active)
            continue;

        system->Update(deltaTime, objects);
    }

    EndTextureMode();

    // Draw to the screen with the render texture afterwards
    BeginDrawing();
    ClearBackground(BLACK);

    // Makes the window letterboxed/pillarboxed to maintain the aspect ratio of the logical resolution
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();
    float screenAspect = (float)screenW / screenH;
    float targetAspect = (float)world->window.logical_resolution.x / world->window.logical_resolution.y;

    int drawWidth, drawHeight;
    int offsetX, offsetY;

    if (screenAspect > targetAspect)
    {
        // window is wider than logical
        drawHeight = screenH;
        drawWidth = (int)(screenH * targetAspect);
        offsetX = (screenW - drawWidth) / 2;
        offsetY = 0;
    }
    else
    {
        // window is taller than logical
        drawWidth = screenW;
        drawHeight = (int)(screenW / targetAspect);
        offsetX = 0;
        offsetY = (screenH - drawHeight) / 2;
    }

    // Draw the render texture to the screen, scaling it
    DrawTexturePro(
        target.texture,
        {0.0f, 0.0f, (float)target.texture.width, -(float)target.texture.height}, // source rect (flip y)
        {(float)offsetX, (float)offsetY, (float)drawWidth, (float)drawHeight},    // dest rect
        {0.0f, 0.0f},                                                             // origin
        0.0f,                                                                     // rotation
        WHITE);

    EndDrawing();
}

void lapCore::Scene::Clear()
{
    ClearObjects();
    systems.clear();
    name = "";
}