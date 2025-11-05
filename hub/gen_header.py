import json

with open("build/hub/assets/proj.lapengine.json") as f:
    data = f.read()

with open("hub/include/projdata.hpp", "w") as out:
    out.write('#pragma once\n')
    out.write('const char proj_json[] = R"json(\n')
    out.write(data)
    out.write('\n)json";\n')