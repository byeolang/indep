/// @file
#pragma once

#include "indep/common.hpp"
#include "indep/helper/tmay.inl"
#include "indep/macro.hpp"
#include <filesystem>
#include <regex>
#ifdef BY_BUILD_PLATFORM_IS_WINDOWS
#    include <direct.h>
#    include <io.h>
#    define getcwd _getcwd
#else
#    include <dirent.h>
#    include <unistd.h>
#endif
#include <vector>

namespace by {

    /** @ingroup indep
     *  @brief Cross-platform filesystem utilities for recursive file traversal
     *  @details A simple class for recursively traversing files in a specified
     * folder. Provides platform-independent API that works on both Windows and
     * POSIX-compliant operating systems. Core functionality is provided through the
     * iterator class.
     *
     *  @section usage Usage
     *  Basic usage example:
     *  @code
     *      auto e = fsystem::find("../your/path");
     *      while(e.next()) { // Returns false when all files are traversed
     *          const std::filesystem::path& p = *e; // Path of the found file
     *          if(*e == "../your/path/child/helloWorld.cpp") // Always uses
     * relative paths doSomething(e->getDir()); // Returns the folder path of the
     * found file
     *      }
     *  @endcode
     *
     *  @remark Always iterates files only
     *  Empty folders are skipped during iteration.
     *
     *
     *  @section existence Checking whether a path is there
     *  A path naming one file works too, so an existence check is just an
     *  iterator that is not empty.
     *  @code
     *      if(fsystem::find("your/path/config.stela").next())
     *          doSomething();
     *  @endcode
     *
     *  @section this supports partial glob patterns
     *  You can use aterisk (*) and question mark (?) wildcards in the pattern.
     *  @code
     *      auto e = fsystem::find("*.cpp"); // the only difference is here
     *      while(e.next()) {
     *          const std::filesystem::path& p = *e;
     *          if(*e == "your/path/child/helloWorld.cpp")
     *              doSomething(e->getDir());
     *      }
     *  @endcode
     */
    class _nout fsystem {
        BY(ME(fsystem))

        struct entry {
#ifdef BY_BUILD_PLATFORM_IS_WINDOWS
            _finddata_t file;
            intptr_t dir;
#else
            DIR* dir;
#endif
            std::filesystem::path path;
        };

        typedef std::vector<entry> entries;

        /** @ingroup indep
         *  @brief Iterator for traversing filesystem entries
         *  @details Provides sequential access to filesystem entries with
         * platform-specific handling for Windows and Unix-like systems.
         */
        class _nout iterator {
            BY(ME(iterator))

        public:
            /**
             * @param path a directory to walk, or a single file. this can be a
             * glob pattern (supports * and ? wildcards)
             */
            iterator() = default;
            iterator(const std::filesystem::path& path);
            iterator(const std::string& path) : iterator(std::filesystem::path(path)) {}
            iterator(const nchar* path) : iterator(std::filesystem::path(path)) {}
            ~iterator();

        public:
            const std::filesystem::path& operator*();
            me& operator++(int);
            operator nbool() const;
            const std::filesystem::path* operator->() const;

        public:
            void rel();

            /**
             * @brief Advances iterator to next file entry
             * @return true if next file found, false if no more files exist.
             * @note Automatically skips directories and recursively navigates
             * subdirectories
             */
            nbool next();

            const std::filesystem::path& get() const;
            std::filesystem::path getDir() const;
            nbool isEnd() const;

        private:
            void _addDir(const std::filesystem::path& dirPath);
            void _popDir();
            static nbool _isGlobPattern(const std::string& str);
            std::regex _convertToRegex(const std::string& globPattern);

        private:
            entries _entries;
            std::filesystem::path _nowPath;
            tmay<std::regex> _pattern;
        };

    public:
        /**
         * @brief Creates iterator for recursive filesystem traversal
         * @return iterator positioned at first file, or at end if path invalid
         * @note Automatically traverses subdirectories recursively, yielding only
         * files
         */
        static iterator find(const std::filesystem::path& path);
        static iterator find(const std::string& path) { return find(std::filesystem::path(path)); }
        static iterator find(const nchar* path) { return find(std::filesystem::path(path)); }
        static iterator find(const std::string* it) BY_SIDE_FUNC(find);

        static std::filesystem::path getCurrentDir();
    };
} // namespace by
