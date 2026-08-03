#pragma once

#include "indep/common.hpp"
#include "indep/def.hpp"
#include "indep/macro.hpp"
#include <string>
#include <vector>
#ifdef BY_BUILD_PLATFORM_IS_WINDOWS
#    include <windows.h>
#else
#    include <sys/wait.h>
#    include <unistd.h>
#endif

namespace by {

    typedef std::vector<std::string> execArgs;

    class _nout process {
        BY(ME(process))

    public:
        process();

    public:
        nbool create(const std::string& execPath, const execArgs& args = execArgs());
        nint wait();

    private:
#ifdef BY_BUILD_PLATFORM_IS_WINDOWS
        PROCESS_INFORMATION _info;
#else
        nint _pid;
#endif
    };
}
