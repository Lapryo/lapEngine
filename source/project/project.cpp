
#include "project/deserializer.hpp"
#include "project/unpacker.hpp"
#include "json/unpacker.hpp"

#include "project/registration.hpp"
#include "eutil.hpp"

using namespace lapCore;

using namespace EUTIL::Functions;
using namespace std;

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
        RegisterElements();
        RegisterEUTIL();
        RegisterRaylib();
        RegisterBox2D();

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