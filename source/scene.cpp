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

    // Get new name
    std::string newName = "";
    if (objectMap.find(name.value()) != objectMap.end())
    {
        int i = 1;
        while (objectMap.find(HASH_ID(newName.c_str())) != objectMap.end())
        {
            newName = std::string(name.data()) + "_" + std::to_string(i);
            i++;
        }
    }

    if (newName != "")
        name = HASH(newName.c_str());
    entry.info.id = name.value();

    // Set the child index to the last if its == -1
    // TODO: But what if the parent doesn't exist yet?
    // Maybe create a new map thats just for object hierarchy?
    
    if (parent.data() != nullptr)
    {
        auto it = objectMap.find(parent.value());
        if (it != objectMap.end())
        {
            if (childIndex == -1)
                it->second.children[it->second.children.size() + 1] = entry.info;
            else
                it->second.children[childIndex] = entry.info;
        }
    }

    objectMap[name.value()] = entry;
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
    for (const auto &obj : objectMap)
    {
        if (obj.second.info.object == object)
        {
            objects.destroy(obj.second.info.object);
            objectMap.erase(obj.second.info.id);
        }
    }
}

Object *lapCore::ObjectContainer::FindObject(entt::id_type id)
{
    auto it = objectMap.find(id);
    if (it != objectMap.end())
        return &it->second.info.object;
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
        for (const auto& tileData : layer.second)
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
                if (localID == 24)
                {
                    std::cout << "bush, zlayer: " << layer.first << '\n';
                }

                float rotation = 0.f;
                bool flipX = tileData.flipX;
                bool flipY = tileData.flipY;

                if (tileData.flipD)
                {
                    std::swap(tileSprite.sourceRect.width,
                            tileSprite.sourceRect.height);

                    if (flipX && !flipY)
                    {
                        rotation = 90.f;
                        flipX = false;
                    }
                    else if (!flipX && flipY)
                    {
                        rotation = -90.f;
                        flipY = false;
                    }
                    else if (flipX && flipY)
                    {
                        rotation = 180.f;
                        flipX = false;
                        flipY = false;
                    }
                }

                if (flipX)
                    tileSprite.sourceRect.width *= -1;

                if (flipY)
                    tileSprite.sourceRect.height *= -1;

                tileTransform.rotation = rotation;

                tileSprite.renderable.tint = {255, 255, 255, (unsigned char)(255 * tileData.opacity)};

                tileSprite.renderable.ySort = tileTransform.position.y + map.tileSize.y;

                AddElement<Sprite>(tile.object, tileSprite);

                if (tileData.isObject)
                {
                    tileTransform.position.x += (tileData.rect.width / 2.f) - (map.tileSize.x / 2.f);
                    tileTransform.position.y += (tileData.rect.height / 2.f) - (map.tileSize.y / 2.f);
                }

                if (localID == 24)
                    std::cout << "ysort: " << tileSprite.renderable.ySort << '\n';
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

void lapCore::ObjectContainer::RemoveElement(Object object, entt::id_type elementType)
{
    auto entry = Reflection::TryGet(elementType);
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