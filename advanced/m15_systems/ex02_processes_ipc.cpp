// m15_systems/ex02_processes_ipc — sozdanie processa, pipe-kak-IPC, signal.
// Windows: CreateProcess/CreatePipe/ReadFile; POSIX: fork/exec/pipe/sigaction.
// Determinizovano. Standart: C++23. Zapusk: ./run.ps1 m15_systems/ex02_processes_ipc
#include <windows.h>

#include <csignal>
#include <cstddef>
#include <iostream>
#include <print>
#include <string>
#include <vector>

namespace {

volatile std::sig_atomic_t g_stop = 0;

extern "C" void handle_signal(int /*signal_number*/) { g_stop = 1; }

int run_child() {
    std::println("child: started");
    std::println("child: work item 1");
    std::println("child: work item 2");
    std::cout.flush();
    return 7;
}

std::wstring current_executable() {
    std::vector<wchar_t> buffer(MAX_PATH);
    for (;;) {
        const DWORD length =
            GetModuleFileNameW(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
        if (length == 0) {
            return {};
        }
        if (length < buffer.size() - 1) {
            return std::wstring(buffer.data(), length);
        }
        buffer.resize(buffer.size() * 2);
    }
}

std::string read_all(HANDLE pipe) {
    std::string result;
    char buffer[256];
    for (;;) {
        DWORD count = 0;
        const BOOL ok = ReadFile(pipe, buffer, sizeof(buffer), &count, nullptr);
        if (ok == FALSE || count == 0) {
            break;
        }
        result.append(buffer, count);
    }
    return result;
}

}  // namespace

int main(int argc, char** argv) {
    if (argc > 1 && std::string(argv[1]) == "--child") {
        return run_child();
    }

    std::signal(SIGINT, handle_signal);
    std::raise(SIGINT);
    std::println("signal handled = {}", g_stop != 0 ? "yes" : "no");

    SECURITY_ATTRIBUTES attributes{};
    attributes.nLength = sizeof(attributes);
    attributes.bInheritHandle = TRUE;
    attributes.lpSecurityDescriptor = nullptr;

    HANDLE read_pipe = nullptr;
    HANDLE write_pipe = nullptr;
    if (CreatePipe(&read_pipe, &write_pipe, &attributes, 0) == FALSE) {
        std::println("CreatePipe failed (error {})", GetLastError());
        return 1;
    }
    SetHandleInformation(read_pipe, HANDLE_FLAG_INHERIT, 0);

    STARTUPINFOW startup{};
    startup.cb = sizeof(startup);
    startup.dwFlags = STARTF_USESTDHANDLES;
    startup.hStdOutput = write_pipe;
    startup.hStdError = write_pipe;
    startup.hStdInput = GetStdHandle(STD_INPUT_HANDLE);

    std::wstring command = L"\"" + current_executable() + L"\" --child";
    std::vector<wchar_t> command_line(command.begin(), command.end());
    command_line.push_back(L'\0');

    PROCESS_INFORMATION process{};
    const BOOL created =
        CreateProcessW(nullptr, command_line.data(), nullptr, nullptr, TRUE, 0, nullptr, nullptr,
                       &startup, &process);
    CloseHandle(write_pipe);
    if (created == FALSE) {
        std::println("CreateProcess failed (error {})", GetLastError());
        CloseHandle(read_pipe);
        return 1;
    }

    const std::string child_output = read_all(read_pipe);
    WaitForSingleObject(process.hProcess, INFINITE);

    DWORD exit_code = 0;
    GetExitCodeProcess(process.hProcess, &exit_code);
    CloseHandle(process.hThread);
    CloseHandle(process.hProcess);
    CloseHandle(read_pipe);

    std::print("child output:\n{}", child_output);
    std::println("child exit code = {}", exit_code);

    const bool ok = exit_code == 7 && g_stop != 0 && child_output.find("work item 2") != std::string::npos;
    std::cout << "processes signals done\n";
    return ok ? 0 : 1;
}
