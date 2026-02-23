#include "ModuleLoader.h"
#include <iostream>

ModuleLoader::ModuleLoader() {}

ModuleLoader::~ModuleLoader() {
    for (auto& pair : modules_) {
        if (pair.second.instance) {
            pair.second.instance->Cleanup();
            pair.second.destroyFunc(pair.second.instance);
        }
        if (pair.second.handle) {
            FreeLibrary(pair.second.handle);
        }
    }
}

bool ModuleLoader::LoadModule(const std::string& modulePath, const std::string& moduleName) {
    if (IsModuleLoaded(moduleName)) {
        return false;
    }

    HMODULE handle = LoadLibraryA(modulePath.c_str());
    if (!handle) {
        return false;
    }

    CreateModuleFunc createFunc = reinterpret_cast<CreateModuleFunc>(
        GetProcAddress(handle, "CreateModule"));
    DestroyModuleFunc destroyFunc = reinterpret_cast<DestroyModuleFunc>(
        GetProcAddress(handle, "DestroyModule"));

    if (!createFunc || !destroyFunc) {
        FreeLibrary(handle);
        return false;
    }

    IModule* instance = createFunc();
    if (!instance) {
        FreeLibrary(handle);
        return false;
    }

    if (!instance->Initialize()) {
        destroyFunc(instance);
        FreeLibrary(handle);
        return false;
    }

    ModuleEntry entry;
    entry.handle = handle;
    entry.instance = instance;
    entry.createFunc = createFunc;
    entry.destroyFunc = destroyFunc;

    modules_[moduleName] = entry;
    return true;
}

void ModuleLoader::UnloadModule(const std::string& moduleName) {
    auto it = modules_.find(moduleName);
    if (it == modules_.end()) {
        return;
    }

    if (it->second.instance) {
        it->second.instance->Cleanup();
        it->second.destroyFunc(it->second.instance);
    }

    if (it->second.handle) {
        FreeLibrary(it->second.handle);
    }

    modules_.erase(it);
}

ModuleResult ModuleLoader::ExecuteModule(const std::string& moduleName, const std::vector<uint8_t>& params) {
    auto it = modules_.find(moduleName);
    if (it == modules_.end()) {
        ModuleResult result;
        result.success = false;
        result.errorMessage = "Module not loaded";
        return result;
    }

    return it->second.instance->Execute(params);
}

std::vector<std::string> ModuleLoader::GetLoadedModules() const {
    std::vector<std::string> result;
    for (const auto& pair : modules_) {
        result.push_back(pair.first);
    }
    return result;
}

bool ModuleLoader::IsModuleLoaded(const std::string& moduleName) const {
    return modules_.find(moduleName) != modules_.end();
}
