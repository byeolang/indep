#pragma once

#include "indep/common.hpp"
#include <vector>
#include <string>
#ifdef BY_BUILD_PLATFORM == BY_TYPE_WINDOWS
#   include <windows.h>
#else
#   include <sys/wait.h>
#   include <unistd.h>
#endif

namespace by {

    typedef std::vector<std::string>> execArgs;
    class _nout process {
        BY(ME(process))


    public:
        process(const std::string& execPath, const execArgs& args = execArgs());

    public:
        int wait();

    private:
#if BY_BUILD_PLATFORM == BY_TYPE_WINDOWS
        PROCESS_INFORMATION _info{};
#else
        int _pid;
#endif
    };
}
