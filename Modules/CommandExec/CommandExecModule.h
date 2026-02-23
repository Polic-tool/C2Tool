#pragma once
#include "ModuleInterface.h"
#include <windows.h>


class CommandExecModule : public IModule {
public:
    CommandExecModule();
    ~CommandExecModule() override;

    ModuleInfo GetInfo() const override;
    bool Initialize() override;
    ModuleResult Execute(const std::vector<uint8_t>& params) override;
    void Cleanup() override;

private:
    std::string ExecuteCommand(const std::string& command);
};
