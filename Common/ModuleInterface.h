#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <windows.h>


#define MODULE_INTERFACE_VERSION 1


enum class ModuleType {
    COMMAND_EXECUTION,
    FILE_OPERATION,
    PROCESS_MANAGEMENT,
    SCREENSHOT,
    KEYLOGGER,
    PERSISTENCE,
    SYSTEM_INFO
};


struct ModuleInfo {
    const char* name;
    const char* version;
    const char* description;
    ModuleType type;
    uint32_t interfaceVersion;
};


struct ModuleResult {
    bool success;
    std::vector<uint8_t> data;
    std::string errorMessage;
};


class IModule {
public:
    virtual ~IModule() = default;

    
    virtual ModuleInfo GetInfo() const = 0;

    
    virtual bool Initialize() = 0;

    
    virtual ModuleResult Execute(const std::vector<uint8_t>& params) = 0;

    
    virtual void Cleanup() = 0;
};


typedef IModule* (*CreateModuleFunc)();
typedef void (*DestroyModuleFunc)(IModule*);


#define EXPORT_MODULE(ClassName) \
    extern "C" __declspec(dllexport) IModule* CreateModule() { \
        return new ClassName(); \
    } \
    extern "C" __declspec(dllexport) void DestroyModule(IModule* module) { \
        delete module; \
    }
