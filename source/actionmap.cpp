#include "actionmap.hpp"

using namespace lapCore;

void ActionMap::RegisterAction(const std::string &actionName, const InputEntry &entry)
{
    actions[actionName] = entry;
}

void ActionMap::RegisterAction(const std::string &actionName, entt::id_type eventID, std::vector<int> codes, bool sustain, InputType inputType, ControlType controlType)
{
    InputEntry entry;
    entry.key.inputType = inputType;
    entry.key.controlType = controlType;
    entry.key.codes = codes;
    entry.sustain = sustain;
    entry.eventID = eventID;

    RegisterAction(actionName, entry);
}