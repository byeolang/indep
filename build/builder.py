#!/usr/bin/python3
# -*- coding: utf-8 -*-
import os
import stat
import sys
import shutil
import platform
import subprocess
import re
from operator import eq
from tempfile import gettempdir

frame = "======================================================="

# Termux compatibility: detect if we're running in Termux environment
def isTermux():
    return os.path.exists('/data/data/com.termux/files/usr')

# Cross-platform system command wrapper
# This fixes SELinux permission issues in Termux while maintaining compatibility
# with Windows, macOS, and regular Linux
def system(cmd):
    if isTermux():
        # In Termux, use subprocess with Termux's shell to avoid SELinux issues
        # SELinux blocks /bin/sh from executing Termux binaries (app_data_file context)
        termux_shell = '/data/data/com.termux/files/usr/bin/sh'
        return subprocess.run(cmd, shell=True, executable=termux_shell).returncode
    else:
        # On other platforms, use standard os.system()
        return os.system(cmd)

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

def rmtree(top):
    for root, dirs, files in os.walk(top, topdown=False):
        for name in files:
            filename = os.path.join(root, name)
            os.chmod(filename, stat.S_IWUSR)
            os.remove(filename)
        for name in dirs:
            os.rmdir(os.path.join(root, name))
    os.rmdir(top)

def printErr(msg):
    print(bcolors.WARNING + " × " + bcolors.ENDC + msg)

def printInfo(msg):
    print(bcolors.OKCYAN + msg + bcolors.ENDC)

def printInfoEnd(msg):
    print(bcolors.OKCYAN + msg + bcolors.ENDC, end=" ")

def printOk(msg):
    print(bcolors.OKGREEN + " v " + bcolors.ENDC + msg)

def printOkEnd(msg):
    print(bcolors.OKGREEN + " v " + bcolors.ENDC + msg, end=' ')

def cmdstr(cmd):
    try:
        ret = str(subprocess.check_output(cmd, stderr=subprocess.STDOUT, shell=True))
        if platform.system() == "Windows":
            ret = ret[2:-5]
        else:
            ret = ret[2:-3]
    except subprocess.CalledProcessError as ex:
        try:
            return f"error: {ex.output.decode('euc-kr').encode('utf-8')}"
        except:
            return "error: "

    return ret

python3 = ""
def branch(command):
    if command == "help":
        return help()
    elif command == "clean":
        return clean()
    elif command == "test":
        arg1 = "" if len(sys.argv) < 3 else sys.argv[2]
        return test(arg1)
    elif command == "format":
        return formatCodesWithDocker(True)
    elif command == "doc":
        return doc()
    elif command == "pubdoc":
        return _publishDoc()

    printErr(command + " is unknown command.")
    return -1

def _cleanParser():
    global stelaDir

    pathDir= stelaDir
    if isWindow():
        stelaPathDir = cwd + "\\..\\module\\stela\\parser\\bison\\"
    else:
        stelaPathDir = cwd + "/../module/stela/parser/bison/"

    printInfoEnd("removing generated parser...")
    filesToRemove = [
        f"{stelaPathDir}stelaLowparser.cpp",
        f"{stelaPathDir}stelaLowparser.hpp",
        f"{stelaPathDir}stelaLowscanner.hpp",
        f"{stelaPathDir}stelaLowscanner.cpp",
    ]

    for file in filesToRemove:
        try:
            os.remove(file)
        except Exception as e:
            printErr(f"failed to remove {file}: {e}")

    printOk("done")

def _cleanIntermediates():
    printInfoEnd("removing intermediate outputs...")
    if isWindow():
        print("del /s /f /q " + cwd + "\\xml")
        system("del /s /f /q " + cwd + "\\xml")
        system("del /s /f /q " + cwd + "\\*.tmp")
    else:
        system("rm -rf " + cwd + "/xml")
        system("rm -rf " + cwd + "/*.tmp")
    system("git config --unset user.name") # remove local config only
    system("git config --unset user.email")
    printOk("done.")

def slash():
    return '\\' if isWindow() else '/'

def _isDockerExist(sudo, docker, containerName):
    return subprocess.run(
        [sudo, docker, "ps", "-a", "-q", "-f", f"name={containerName}"],
        stdout=subprocess.PIPE,
        text=True).stdout.strip()

def _isAppleSilicon():
    return platform.system() == "Darwin" and platform.machine() in ("arm64", "aarch64")

dockerRepo = "ghcr.io/homebrew/core/clang-format:18.1.8"
dockerRepo = "ams21/clang-format:18.1.8"

def _runDocker(sudo, docker, containerName):
    global stelaDir, dockerRepo
    uid = os.getuid()
    gid = os.getgid()
    if _isAppleSilicon():
        subprocess.run([sudo, docker, "run", 
            "--user", f"{uid}:{gid}",
            "--platform", "linux/amd64",
            "-d", "--name", containerName, "-v", f"{stelaDir}:/src",
            dockerRepo, "tail", "-f", "dev/null"
        ])


    elif isWindow():
        subprocess.run([sudo, docker, "run",
            "-d", "--name", containerName, "-v", f"{stelaDir}:/src",
            dockerRepo, "tail", "-f", "dev/null"
        ])

    else:
        subprocess.run([sudo, docker, "run",
            "--user", f"{uid}:{gid}",
            "-d", "--name", containerName, "-v", f"{stelaDir}:/src",
            dockerRepo, "tail", "-f", "dev/null"
        ])


def formatCodesWithDocker(showLog):
    docker = DockerDependency()
    if checkDependencies([docker]):
        return -1

    global stelaDir
    containerName = "stela-clang-format-container__"
    sudo = "" if isWindow() else "sudo"
    if not _isDockerExist(sudo, docker.binary, containerName):
        _runDocker(sudo, docker.binary, containerName)

        if not _isDockerExist(sudo, docker.binary, containerName):
            printErr("docker container still now working!")
            return -1

    root = stelaDir
    if showLog: print("code formatting:")
    for path, dirs, files in os.walk(root):
        if "../module/" not in path: continue
        if "/stela/parser/bison" in path: continue
        for file in files:
            filePath = os.path.join(path, file) # absolute path
            filePath = f"src/{os.path.relpath(filePath, root)}" # to relative path
            ext = os.path.splitext(file)[1]
            if  ext != ".cc" and ext != ".cpp" and ext != ".hpp" and ext != ".inl":
                continue
            if showLog: 
                printInfoEnd("\t formatting ")
                print(filePath + ", ext=" + ext + " file...")

            res = subprocess.run(
                [sudo, docker.binary, "exec", containerName, "clang-format", "-i", filePath],
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True,
            )
            if len(res.stderr) > 0:
                print(res.stderr)

    subprocess.run([sudo, docker.binary, "stop", containerName])
    return subprocess.run([sudo, docker.binary, "rm", containerName]).returncode

# currently this application only supports window and linux.
def isWindow():
    return platform.system().find("Windows") >= 0

class ver:
    def __init__(self, major, minor, patch, onlyThisVer):
        self.major = major
        self.minor = minor
        self.patch = patch
        self.onlyThisVer = onlyThisVer
        self.exist = False

    @classmethod
    def fromVerString(self, verString):
        if verString[:3] == "err" or verString == "":
            return ver(0, 0, 0, False)
        else:
            ret = ver(0, 0, 0, False)
            res = re.findall(r"[0-9]+\.[0-9]+[\.0-9]+", verString)
            verStr = res[0].split('.') if len(res) > 0 else ""
            if verStr != "":
                ret.major = int(verStr[0])
                ret.minor = int(verStr[1])
                if len(verStr) > 2:
                    ret.patch = int(verStr[2])
                else:
                    ret.patch = 0
                ret.exist = True
                return ret

            res = re.findall(r"[0-9]+\.[0-9]+", verString)
            verStr = res[0].split('.') if len(res) > 0 else ""
            if verStr != "":
                ret.major = int(verStr[0])
                ret.minor = int(verStr[1])
                ret.patch = 0

            ret.exist = True
            return ret

    def isVerNotSpecified(self):
        return self.major == 0 and self.minor == 0 and self.patch == 0

    def isValid(self, expectVer):
        if expectVer.isVerNotSpecified():
            return self.doesExist()

        if expectVer.onlyThisVer:
            return self.major == expectVer.major and self.minor == expectVer.minor and self.patch == expectVer.patch
    
        if self.major < expectVer.major: return False
        if self.major > expectVer.major: return True 
        # okay. major version is same.

        if self.minor < expectVer.minor: return False
        if self.minor > expectVer.minor: return True 

        # okay. even minor version is also same.
        return self.patch >= expectVer.patch

    def toString(self):
        if self.major == 0 and self.minor == 0 and self.patch == 0:
            return ""
        flag = "" if self.onlyThisVer else "+"
        return f"{self.major}.{self.minor}.{self.patch}{flag}"

    def doesExist(self):
        return self.exist

class dependency:
    binary = ""

    def getExpectVer(self):
        return ver(0, 0, 0, False)

    def getName(self):
        names = self.getNames()
        if(len(names) == 1):
            return names[0]

        return " or ".join(names)

    def getNames(self):
        return [""]

    def getFlag(self):
        return "--version"

    def onSetBinary(self, newBinary):
        self.binary = newBinary

    def getInstalledVer(self):
        ret = ver(0, 0, 0, False)
        for name in self.getNames():
            res = self.onGetInstalledVerString(name)
            if res[:5] != "error": # usual case
                givenVer = ver.fromVerString(res)
                if givenVer.isValid(ret):
                    self.onSetBinary(name)
                    ret = givenVer
                continue

            # oddly enough, for some binaries, it always returns an error code even if the file
            # exists. so in this case, you can't check the version and you can't use the output
            # msg to determine whether the file exists.
            # In this case, you have to use shutil to determine directly whether the binary exists
            # in the PATH.
            if shutil.which(name):
                givenVer = ver.fromVerString("0.0.0")
                if givenVer.isValid(ret):
                    self.onSetBinary(name)
                    ret = givenVer
        return ret

    def isValid(self):
        if self.isActivated() == False:
            return True
        return self.getInstalledVer().isValid(self.getExpectVer())

    # if `isActivated` returns true, it means that this dependency has to be checked in all
    # circumstances.
    def isActivated(self):
        return True

    def onGetInstalledVerString(self, name):
        return cmdstr(f"{name} {self.getFlag()}")

    def showErrMsg(self):
        installedVer = self.getInstalledVer()
        if installedVer.doesExist() == False:
            printErr(f"{self.getName()} version {self.getExpectVer().toString()} is not installed")
        else:
            printErr(f"{self.getName()} version should be {self.getExpectVer().toString()} but yours is {self.getInstalledVer().toString()}")

    def showSuccessMsg(self):
        if self.isActivated():
            printOkEnd(self.binary)

class GitDependency(dependency):
    def getNames(self):
        return ["git"]

class DoxygenDependency(dependency):
    def getNames(self):
        return ["doxygen"]

    def getExpectVer(self):
        return ver(1, 10, 0, False)

class JREDependency(dependency):
    def getNames(self):
        return ["java"]

    def getExpectVer(self):
        return ver(1, 8, 0, False)

class DockerDependency(dependency):
    def getNames(self):
        return ["docker"]

def checkDependencies(deps):
    printInfoEnd("checking dependencies...")

    hasErr = False;
    for d in deps:
        if d == None: continue
        if d.isValid() == False:
            d.showErrMsg()
            hasErr = True
        else:
            d.showSuccessMsg()
    print("")
    return hasErr

def help():
    print("Usage: builder.py <command> <arg1> <arg2> ...")
    print("")
    print("command list:")
    print("\t * help")
    print("\t * clean  clear all cache files of cmake outputs.")
    print("\t * format apply our code convention rules to current repository. it'll be done by clang-format docker")
    print("\t          image. and as you may know, that could lead you to download a pretty much big image file.")
    print("\t * doc    generate documents only.")

def clean():
    printInfo("Clearing next following files...")
    global cwd, binDir
    _clean(cwd)
    _cleanIntermediates()
    _cleanDir(binDir)
    _cleanDir(cwd + "/_deps") # for googleTest & flagStacker
    _cleanParser()
    if isWindow():
        system("del /f /s /q " + cwd + "\\html")
    else:
        system("rm -rf " + cwd + "/html")
    printOk("was removed successfully.")

def _clean(directory):
    for path, dirs, files in os.walk(directory):

        for file in files:
            file_path = os.path.join(path, file)
            ext = os.path.splitext(file_path)[1]
            if  ext == ".sln" or ext == ".vcproj" or ext == ".vcxproj" or ext == ".cmake" or \
                file == "CMakeCache.txt" or ext == ".stamp" or ext == ".depend" or file == "Makefile" or \
                ext == ".exe" or ext == ".dll" or ext == ".log" or ext == ".a" or ext == ".so":
                print("\t * " + file_path)
                os.remove(file_path)

        for dir in dirs:
            if dir == "CMakeFiles" or dir == "tmp" or dir == "generated" or dir == "cmake-build-debug" or dir == "Testing":
                abs_dir = os.path.join(path, dir)
                print("\t * " + abs_dir)
                _cleanDir(abs_dir)

def _cleanDir(dir):
    if os.path.isdir(dir) == False: return
    rmtree(dir)

# arg is "" for dbg or "silent" for rel
def test(arg):
    print("")
    printInfoEnd("let's initiate unit tests...")
    global cwd, binDir

    originDir = os.getcwd()
    os.chdir(binDir)
    failedCnt = 0
    ret = 0
    if isWindow():
        res = system(".\\test verbose " + arg)
    else:
        res = system("./test verbose " + arg)
    if res != 0:
        printErr("test was failed!")
        ret = -1
        failedCnt += 1

    if failedCnt > 0:
        printErr("total " + str(failedCnt) + " TC files has reported that failed.")
    else:
        printOk("all TCs have been passed!")
    os.chdir(originDir)
    return ret

def docDoxygen(doxygen):
    global cwd, python3

    def runCommand(cmd):
        res = system(cmd)
        if res != 0:
            printErr("fail to run doxygen.")
            _cleanIntermediates()
        return res

    # build doxygen:
    printInfoEnd("generating english docs using reference using doxygen...")
    res = runCommand(f"{doxygen.binary} {cwd}{slash()}DoxyReference")
    if res != 0: return res
    printOk("done")

def cleanGhPages(git):
    global cwd, python3

    def runCommand(cmd):
        res = system(cmd)
        if res != 0:
            printErr("fail to clone gh-pages repo.")
            _cleanIntermediates()
        return res

    # clean before fetch repo:
    _cleanIntermediates()

    pathHtml = f"{cwd}{slash()}html"
    if isWindow():
        system(f"del /s /f /q {pathHtml}")
    else:
        system(f"rm -rf {pathHtml}")

    # standby gh-pages repo:
    printInfoEnd("cloning gh-pages branch...")
    res = runCommand(f"{git.binary} clone --depth 5 https://github.com/byeolang/website --single-branch {pathHtml}")
    if res != 0: return res
    printOk("done.")
    return 0

def doc():
    doxygen = DoxygenDependency()
    git = GitDependency()
    jre = JREDependency()
    if checkDependencies([doxygen, git, jre]):
        return -1

    if cleanGhPages(git) != 0:
        return -1
    docDoxygen(doxygen)
    return 0

def _publishDoc():
    git = GitDependency()
    if checkDependencies([git]):
        return -1

    # pushing on gh-pages:
    origin = cmdstr(f"{git.binary} rev-parse --verify HEAD")
    print("origin=" + str(origin))
    os.chdir(cwd + "/html")
    system(f"{git.binary} add .")
    system(f"{git.binary} config user.name \"autodocbot\"") # put on local config.
    system(f"{git.binary} config user.email \"kniz@byeol.io\"")
    res = system(f"{git.binary} commit -m \"The our poor little Autobot \\(❍ᴥ❍ʋ)/ generated docs for " + origin + ", clitter-clatter.\"")
    if res != 0:
        printErr("fail to commit on gh-pages.")
        printInfo("but it seems that nothing changed.")
        _cleanIntermediates()
        return 0

    token = os.environ.get("GH_TOKEN")
    if token:
        res = system(f"{git.binary} push https://x-access-token:{token}@github.com/byeolang/website.git main")
        if res != 0:
            printErr("WARNING: Push failed (likely conflict). Continuing as success.")

    else:
        printErr("WARNING: couldn't get token.")

    os.chdir(cwd)
    _cleanIntermediates()
    return 0

def _where(name):
    cmd = ""
    prefix = ""
    if platform.system() == "Linux" or platform.system() == "Darwin":
        cmd = "which"
    elif platform.system() == "Windows":
        cmd = "where"
        prefix = "\""
    else:
        printErr(platform.system() + " unsupported.")
        return ""

    res = cmdstr(cmd + " " + name)
    ret = ""
    if res:
        ret = prefix + res + prefix
    return ret

def _extractEnv():
    global python3
    if "PYTHON" in os.environ:
        python3 = os.environ["PYTHON"]
    else:
        python3 = _where("python3")
        if not python3:
            python3 = _where("python")
        return python3 == ""
cwd = ""
stelaDir = ""
resDir = ""
binDir = ""
generator = "Visual Studio 17 2022" if isWindow() else "Unix Makefiles"
winProp = ""

def _init():
    global cwd, stelaDir, binDir, resDir
    cwd = os.path.dirname(os.path.realpath(sys.argv[0]))
    if isWindow():
        stelaDir = cwd + "\\.."
        binDir = stelaDir + "\\bin"
        resDir = stelaDir + "\\res"
        # in order to color output text in windows terminal, I need this.
        system('color')
    else:
        stelaDir = cwd + "/.."
        binDir = stelaDir + "/bin"
        resDir = stelaDir + "/res"

    return _extractEnv()

def main():
    if _init():
        return -1

    os.chdir(cwd)
    if len(sys.argv) == 1:
        help()
    else:
        return branch(sys.argv[1])

    print("")
    return 0

ret = main()
if ret != None and ret != 0:
    printErr("ends with " + str(ret) + " exit code.")
sys.exit(ret)
