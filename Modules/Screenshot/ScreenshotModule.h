#pragma once
#include "ModuleInterface.h"

class ScreenshotModule : public IModule {
public:
    ScreenshotModule();
    ~ScreenshotModule() override;

    ModuleInfo GetInfo() const override;
    bool Initialize() override;
    ModuleResult Execute(const std::vector<uint8_t>& params) override;
    void Cleanup() override;

private:
    std::vector<uint8_t> CaptureScreen();
};
