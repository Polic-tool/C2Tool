#pragma once
#include "ModuleInterface.h"

class PersistenceModule : public IModule {
public:
    PersistenceModule();
    ~PersistenceModule() override;

    ModuleInfo GetInfo() const override;
    bool Initialize() override;
    ModuleResult Execute(const std::vector<uint8_t>& params) override;
    void Cleanup() override;

private:
    bool InstallPersistence(const std::string& path);
    bool RemovePersistence();
};
