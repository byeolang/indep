#include "indep/fsystem/fsystem.hpp"
#include "indep/helper/typeTrait.hpp"
#include "indep/macro/when.hpp"

namespace by {

    BY_DEF_ME(fsystem)

    using namespace std::filesystem;

    const path& me::iterator::get() const { return _nowPath; }

    nbool me::iterator::next() {
        // assume that all of data to _entries was valid:
        //  which means, if returning value from readdir() was null,
        //  then the most top element of entries should be removed.
        while(!isEnd()) {
            entry& e = _entries[_entries.size() - 1];
#ifdef BY_BUILD_PLATFORM_IS_WINDOWS
            int res = _findnext(e.dir, &e.file);
            if(res == -1) {
#else
            struct dirent* file = readdir(e.dir);
            if(!file) {
#endif
                _popDir();
                continue;
            }
#ifdef BY_BUILD_PLATFORM_IS_WINDOWS
            std::string name = e.file.name;
#else
            std::string name = file->d_name;
#endif
            if(name == ".." || name == ".") continue;
            path child = e.path / name;
#ifdef BY_BUILD_PLATFORM_IS_WINDOWS
            if(e.file.attrib & _A_SUBDIR) {
#else
            if(file->d_type == DT_DIR) {
#endif
                _addDir(child);
                continue;
            }

            if(_pattern.has() && !regex_match(name, *_pattern)) continue;

            _nowPath = child;
            return true;
        }

        _nowPath.clear();
        return false;
    }

    me::iterator::iterator(const path& newPath) {
        path fileName = newPath.filename();

        if(!_isGlobPattern(fileName) && is_directory(newPath)) _addDir(newPath);
        else {
            _pattern.set(_convertToRegex(fileName));

            path dir = newPath.parent_path();
            _addDir(dir.empty() ? path(".") : dir);
        }
    }

    me::iterator::iterator(const std::string& newPath): iterator(path(newPath)) {}

    me::iterator::iterator(const nchar* newPath): iterator(path(newPath)) {}

    me::iterator::~iterator() { rel(); }

    const path& me::iterator::operator*() { return get(); }

    me::iterator& me::iterator::operator++(int) {
        next();
        return *this;
    }

    me::iterator::operator nbool() const { return !isEnd(); }

    const path* me::iterator::operator->() const { return &get(); }

    void me::iterator::rel() {
        while(!isEnd())
            _popDir();
    }

    path me::iterator::getDir() const { return get().parent_path(); }

    nbool me::iterator::isEnd() const { return _entries.size() == 0; }

    void me::iterator::_addDir(const path& dirPath) {
        if(dirPath.empty()) return;
#ifdef BY_BUILD_PLATFORM_IS_WINDOWS
        _finddata_t file;
        intptr_t newDir = _findfirst((dirPath / "*.*").string().c_str(), &file);
        if(newDir == -1) return;
        _entries.push_back(entry{file, newDir, dirPath});
#else
        DIR& newDir = opendir(dirPath.string().c_str()) OR.ret();
        _entries.push_back(entry{&newDir, dirPath});
#endif
    }

    void me::iterator::_popDir() {
        entry& e = _entries[_entries.size() - 1];
#ifdef BY_BUILD_PLATFORM_IS_WINDOWS
        _findclose(e.dir);
#else
        closedir(e.dir);
#endif
        _entries.pop_back();
    }

    nbool me::iterator::_isGlobPattern(const path& p) { return p.string().find_first_of("*?") != std::string::npos; }

    std::regex me::iterator::_convertToRegex(const path& globPattern) {
        std::string src = globPattern.string();
        std::string ret;
        ret.reserve(src.length() * 2); // rough estimate

        for(char c: src) {
            switch(c) {
                case '*': ret += ".*"; break;
                case '?': ret += "."; break;
                // every other regex metacharacter is a literal here. plain file
                // names reach this too, and they do carry '+', '(' and friends.
                case '.':
                case '\\':
                case '^':
                case '$':
                case '+':
                case '(':
                case ')':
                case '[':
                case ']':
                case '{':
                case '}':
                case '|': ret += '\\'; [[fallthrough]];
                default: ret += c;
            }
        }

        return std::regex(ret, std::regex::icase);
    }

    me::iterator me::find(const path& newPath) { return iterator(newPath); }

    path me::getCurrentDir() {
        constexpr ncnt BUF_LEN = 256;
        char buf[BUF_LEN] = {};
        return path(getcwd(buf, BUF_LEN));
    }
} // namespace by
