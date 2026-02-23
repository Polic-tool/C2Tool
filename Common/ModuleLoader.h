#pragma once
#include "ModuleInterface.h"
#include <map>
#include <memory>
#include <string>


class ModuleLoader {
public:
    ModuleLoader();
    ~ModuleLoader();

    
    bool LoadModule(const std::string& modulePath, const std::string& moduleName);

    
    void UnloadModule(const std::string& moduleName);

    
    ModuleResult ExecuteModule(const std::string& moduleName, const std::vector<uint8_t>& params);

    
    std::vector<std::string> GetLoadedModules() const;

    
    bool IsModuleLoaded(const std::string& moduleName) const;

private:
    struct ModuleEntry {
        HMODULE handle;
        IModule* instance;
        CreateModuleFunc createFunc;
        DestroyModuleFunc destroyFunc;
    };

    std::map<std::string, ModuleEntry> modules_;
};
