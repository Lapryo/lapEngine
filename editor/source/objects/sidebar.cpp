#include "objects/sidebar.hpp"
#include "editor.hpp"

std::vector<ObjectInfo> objectsInSidebar;

void OpenSelectionMenu(Scene* scene, Object object)
{
    auto selectionMenu = scene->FindObject("selectionMenu").info.object;
    auto *frame = scene->FindElement<Frame>(scene->objects, selectionMenu);
    if (!frame)
        return;

    // Check here for if the laodProject has any scenes, if not, change the menu list to "Add Scene" only
    if (lapEditor::loadedProjectJson["scenes"].empty()) {
        std::cout << "[SidebarObject] No project loaded or project has no scenes.\n";

        // Change menu list here

        
        
    }

    rl::Vector2 mousePos = GetMouseInViewportSpace(scene->logicalResolution.x, scene->logicalResolution.y);

    frame->origin.position.offset = mousePos;
    frame->renderable.visible = true;
}

void RefreshSidebar(Scene* scene, Object object)
{
    std::cout << "refreshing sidebar...\n";
    auto *frame = scene->FindElement<Frame>(scene->objects, object);
    if (!frame) std::cout << "could not find frame for the sidebar.\n";

    frame->renderable.visible = true;

    // Clear out current sidebar

    if (!lapEditor::loadedProjectJson["scenes"].empty())
    {
        for (auto sceneJson : lapEditor::loadedProjectJson["scenes"])
        {
            std::string sceneName = sceneJson["name"];
            if (sceneName == "main")
            {
                // Set to the first open scene
                if (!sceneJson["objects"].empty())
                {
                    for (auto objJson : sceneJson["objects"])
                    {
                        std::string objectName = "Object";
                        if (scene->objectMap.find(objectName) != scene->objectMap.end())
                        {
                            int suffix = 2;
                            while (scene->objectMap.find(objectName) != scene->objectMap.end())
                            {
                                objectName = "Object " + std::to_string(suffix);
                                suffix++;
                            }
                        }

                        scene->AddObjectFromPrefab("object-entry-gui", objectName);
                    }
                }
            }
        }
    }
}

void SidebarObject::RegisterLogic()
{
    ScriptRegistry::onCreateFunctions["setup-sidebar"] = [](Scene* scene, Object object) {
        ConnectECSEvent(scene, object, "open-selection-menu", OpenSelectionMenu);
        ConnectECSEvent(scene, object, "refresh-sidebar", RefreshSidebar);
    };
}