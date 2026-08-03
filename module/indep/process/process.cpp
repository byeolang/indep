#include "process.hpp"

#include <cerrno>

namespace by {
    BY(DEF_ME(process))

    namespace {
#ifdef BY_BUILD_PLATFORM_IS_WINDOWS
        std::string join(const std::string& execPath, const execArgs& args) {
            auto wrapQuote = [](const std::string& v) {
                if(v.find_first_of(" \t\"") == std::string::npos) return v;
                std::string out = "\"";
                for(char ch : v) {
                    if(ch == '"') out += "\\\"";
                    else out += ch;
                }
                return out + "\"";
            };

            std::string out = wrapQuote(execPath);
            for(std::size_t i = 0; i < args.size(); i++) {
                out += " ";
                out += wrapQuote(args[i]);
            }
            return out;
        }
#endif
    }

    int me::create(const std::string& execPath, const execArgs& args) {
#ifdef BY_BUILD_PLATFORM_IS_WINDOWS
        std::string cmd = join(execPath, args);
        STARTUPINFOA si{};
        si.cb = sizeof(si);
        return CreateProcessA(nullptr, cmd.data(), nullptr, nullptr, TRUE,
            0, nullptr, nullptr, &si, &_info) ? 0 : -1;
#else
        std::vector<nchar*> argv;
        argv.reserve(args.size() + 2);
        argv.push_back(const_cast<nchar*>(execPath.c_str()));
        for(const std::string& a : args)
            argv.push_back(const_cast<nchar*>(a.c_str()));
        argv.push_back(nullptr);

        _pid = ::fork();
        if(_pid < 0) return -1;
        if(_pid != 0) return 0;

        ::execv(execPath.c_str(), argv.data());
        ::_exit(127);
        return -1;
#endif
    }

    int me::wait() {
#ifdef BY_BUILD_PLATFORM_IS_WINDOWS
        WaitForSingleObject(_info.hProcess, INFINITE);
        DWORD exitCode = 1;
        GetExitCodeProcess(_info.hProcess, &exitCode);
        CloseHandle(_info.hThread);
        CloseHandle(_info.hProcess);
        return static_cast<int>(exitCode);
#else
        int status = 0;
        while(::waitpid(_pid, &status, 0) < 0) {
            if(errno == EINTR) continue;
            return -1;
        }

        if(WIFEXITED(status)) return WEXITSTATUS(status);
        if(WIFSIGNALED(status)) return 128 + WTERMSIG(status);
        return 1;
#endif
    }
}
