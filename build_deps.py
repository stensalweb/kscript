#!/usr/bin/env python3
""" build_deps.py - dependency building script

"""

import os
import requests
from enum import Enum

JOBS = 16

# colors (from blender)
class bcol:
    # reset to default output
    RESET    = '\033[0m'

    HEADER   = '\033[95m'

    OKBLUE   = '\033[94m'
    OKGREEN  = '\033[92m'

    # warn, fail, statuses
    WARN     = '\033[93m'
    FAIL     = '\033[91m'

    # bold, underline, text
    BOLD     = '\033[1m'
    ULIN     = '\033[4m'


""" Platform - enumerates the well known platforms, and also contains 'UNKNOWN' as a fallback

TODO: add an exact platform string that can be checked

"""
class Platform(Enum):

    Unknown     = 0

    Windows     = 1
    MacOS       = 2

    Linux       = 3



    # static method to get the current platform
    def get():
        if hasattr(Platform, "_this_platform"):
            return Platform._this_platform
        else:
            Platform._this_platform = Platform.Linux
            return Platform._this_platform


""" Var - represents a lazy variable reference, which can be expanded later


"""
class Var:

    # create a new variable with a given name reference
    def __init__(self, name):
        if not name.isidentifier():
            raise Exception(f"Invalid variable name '{name}'; Variable names must be identifiers (a-zA-Z0-9_)")
        self.name = name

    # convert to string 
    def __str__(self):
        global VARS
        try:
            return str(VARS[self.name])
        except:
            raise Exception(f"Unknown variable '{self.name}'")
    
    # print detailed information
    def __repr__(self):
        return f"${self.name}"

    # flattens to string
    def eval(self):
        return str(self)

    def __add__(self, other):
        return VarExpr(self, other)

    def __radd__(self, other):
        return VarExpr(other, self)

""" VarExpr - internal class to allow string concatenation with 

"""
class VarExpr:

    def __init__(self, *args):
        self.args = args

    def __str__(self):
        return "".join(map(str, self.args))

    def __repr__(self):
        rs = ""
        for arg in self.args:
            if type(arg) is str:
                rs += arg
            else:
                rs += repr(arg)
        return rs
    # flattens to string  
    def eval(self):
        return str(self)


    def __add__(self, other):
        return VarExpr(*self.args, other)

    def __radd__(self, other):
        return VarExpr(other, *self.args)


""" Platf

"""
class PlatformVar:

    def __init__(self, choices):
        if type(choices) is not dict:
            raise Exception(f"Invalid choices for platform-discrimination '{choices}'; Must be a dictionary")

        self.choices = choices

    def __str__(self):
        plat = Platform.get()
        return self.choices.get(plat, "")

    def __repr__(self):
        return f"PlatformVar({repr(self.choices)})"

""" BuildError - raised when an error in building occurs

"""
class BuildError(Exception):

    def __init__(self, target, step, message):
        self.target = target
        self.step = step
        self.message = message
        super().__init__(f"{bcol.FAIL}FAIL:{bcol.RESET} While building: {bcol.OKBLUE}{self.target.name if self.target else self.target}{bcol.RESET}, in step: {bcol.OKBLUE}{self.step}{bcol.RESET}, reason: {bcol.WARN}{self.message}{bcol.RESET}")



class Step:
    """

    This class is the abstract base class of all singular build steps.

    Any build steps should override the `execute()` method

    """

    def __init__(self, *args, **kwargs):
        self.args = args
        self.kwargs = kwargs
        # alias
        self.get = self.kwargs.get

    def __str__(self):
        return f"{type(self).__name__}(args={self.args}, kwargs={self.kwargs})"

    def __repr__(self):
        return str(self)

    def log(self, action, desc):
        print (f"{bcol.HEADER}{action}:{bcol.RESET} {desc}{bcol.RESET}")

    def success(self, desc=""):
        print (f"  {bcol.OKGREEN}Success! {bcol.RESET}{desc}")

    # execute the build step. This is the only method that should be overriden by base classes
    def execute(self, _target):
        raise BuildError(None, self, "execute() method not overriden!")


class Goto(Step):
    """

    Goto(folder)

    Goes to a folder/directory (i.e. changes directory)

    NOTE: This changes future steps, and is persistent

    """

    def execute(self, _target):
        if len(self.args) != 1:
            raise BuildError(None, self, "Goto should be constructed like: Goto(folder)")

        folder = str(self.args[0])

        folder_real = os.path.realpath(folder)
        cwd_real = os.path.realpath(os.getcwd())

        self.log("GOTO", f"Changing directory to {bcol.OKBLUE}{folder}{bcol.RESET} (full: {bcol.OKBLUE}{folder_real})")

        if folder_real != cwd_real:
            os.chdir(folder_real)
        
        self.success()


class Download(Step):
    """

    Download(dest, url)

    Build step that downloads a file

    """


    def execute(self, _target):
        if len(self.args) != 2:
            raise BuildError(None, self, "Download should be constructed like: Download(dest, url)")

        # get the destination file & URL
        dest, url = map(str, self.args)

        self.log("DOWNLOAD", f"Downloading file {bcol.OKBLUE}{dest}{bcol.RESET} from {bcol.OKBLUE}{url}")

        if not os.path.exists(dest):
            print (f"  ... requesting '{url}' ...")
            rq = requests.get(url, allow_redirects=True)
            mkdir(os.path.dirname(dest))

            fp = open(dest, 'wb')
            fp.write(rq.content)
            fp.close()

        self.success()


class Extract(Step):
    """

    Extract(dest, archive)

    Build step that extracts a file to a target directory

    """

    def execute(self, _target):
        if len(self.args) != 2:
            raise BuildError(None, self, "Extract should be constructed like: Extract(dest, archive)")

        # get the destination file & URL
        dest, archive = map(str, self.args)

        self.log("EXTRACT", f"Extracting into {bcol.OKBLUE}{dest}{bcol.RESET} from {bcol.OKBLUE}{archive}")

        donefile = f"{dest}/archive{hash(archive)}.done"

        # if it's not been created, we need to do it
        if not os.path.exists(donefile):
            # ensure directory exists
            mkdir(os.path.dirname(donefile))

            if ".tar" in archive:
                if not shell(f"tar xf {archive} -C {dest}"):
                    raise BuildError(None, self, "'tar' command failed!")
            elif ".zip" in archive:
                if not shell(f"unzip -q -o {archive} -d {dest}"):
                    raise BuildError(None, self, "'zip' command failed!")
            else:
                raise BuildError(None, self, "Unknown archive format for file: {bcol.OKBLUE}{archive}")

            # now, signal we are done so it is not rebuilt
            touch(donefile)


        self.success()

class Configure(Step):
    """

    Configure()

    Configures an autoconf-like project, i.e., runs:

    ./configure

    """

    def execute(self, _target):

        # what should the prefix be?
        prefix = str(self.get("prefix", Var('PREFIX')))

        opts = str(self.get("opts", ""))

        self.log("CONFIGURE", f"Configuring in directory {bcol.OKBLUE}{os.getcwd()}")

        donefile = f"./configure.done"

        if not os.path.exists(donefile):

            if not shell(f"./configure -q --prefix={prefix} {opts}"):
                raise BuildError(None, self, "Configuration command failed!")

            # now, signal we are done so it is not rebuilt
            touch(donefile)
        
        self.success()


class CMakeConfigure(Step):
    """

    CmakeConfigure()

    Configures an CMAKE project, i.e., runs:

    cmake . -G 'Unix Makefiles' ...

    """

    def execute(self, _target):

        # what should the prefix be?
        prefix = str(self.get("prefix", Var('PREFIX')))

        opts = str(self.get("opts", ""))

        self.log("CMAKE", f"Configuring in directory {bcol.OKBLUE}{os.getcwd()}")

        donefile = f"./cmake.done"

        if not os.path.exists(donefile):

            if not shell(f"cmake . -G 'Unix Makefiles' -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX:PATH={prefix} {opts} > /dev/null"):
                raise BuildError(None, self, "Configuration command failed!")

            # now, signal we are done so it is not rebuilt
            touch(donefile)
        
        self.success()

class Make(Step):
    """

    Make(target="", opts="")

    Configures an autoconf-like project, i.e., runs:

    ./configure

    """

    def execute(self, _target):

        # find target
        target = ""
        opts   = ""

        if len(self.args) >= 1:
            target = str(self.args[0])

        if len(self.args) >= 2:
            opts = str(self.args[1])


        self.log(f"MAKE ({target})", f"Makefile-ing {bcol.OKBLUE}{os.getcwd()}")

        if not shell(f"make {target} -j{Var('JOBS')} -s {opts} > /dev/null"):
            raise BuildError(None, self, "Configuration command failed!")

        self.success()

class AddArtifacts(Step):
    """

    AddArtifacts([list_of_artifacts, ...], wildcard=False)

    Adds artifacts to the target, and if wildcard==True, then just add whatever can be found
      (if wildcard==False (default), then all are added and may cause errors)

    """

    def execute(self, _target):

        # which artifacts
        artifacts = list(map(str, self.args[0]))

        wildcard = self.args[1] if len(self.args) >= 2 else False

        astr = " ".join(artifacts)

        self.log("ADDARTIFACTS:", f"Adding artifacts: {bcol.OKBLUE}{astr}")

        if wildcard:
            for art in artifacts:
                if os.path.exists(art):
                    _target.add_artifact(art)
        else:
            for art in artifacts:
                _target.add_artifact(art)

        self.success()


class Target:
    """

    Abstract base class describing a target

    """

    def __init__(self, name, steps, deps=None, **kwargs):
        self.name = name
        self.steps = steps
        self.deps = deps if deps else []

        self.artifacts = []

        self.kwargs = kwargs

    def add_artifact(self, art):
        self.artifacts += [art]

    def __str__(self):
        return f"{type(self).__name__}(name='{self.name}', kwargs={self.kwargs})"
    
    def __repr__(self):
        return str(self)

    def build(self):

        # results from the build
        self.artifacts = []

        # build dependencies
        for dep in self.deps:
            dep.build()

        print (f"{bcol.HEADER}")
        print ("-" * 80)
        print ("")
        print (f"    --  {self.name} --")
        print ("")
        print ("-" * 80)
        print (f"{bcol.RESET}")


        # just perform all steps
        for step in self.steps:
            exc = None
            try:
                step.execute(self)
            except BuildError as be:
                exc = BuildError(self, be.step, be.message)
            if exc: raise exc

        astr = " ".join(self.artifacts)

        print (f"{bcol.OKGREEN}BUILT:{bcol.RESET} {self.name}{bcol.RESET} (got: {bcol.OKBLUE}{astr}{bcol.RESET})")
        



# -*- HELPER FUNCS


# messages

def fail():
    print()
    exit(1)

# basic utilities

# run a shell command, and return whether it was successful
def shell(cmd):
    cmd = str(cmd)
    print (f" $ {cmd}")
    return os.system(cmd) == 0

# touches a given file (i.e. creates an empty file with that name, overriding anything before it)
def touch(fl):
    fp = open(fl, "w")
    fp.close()
    print (f" $ touch {fl}")

# make a directory (does nothing if it already exists)
def mkdir(dr):
    dr = str(dr)
    if not os.path.exists(dr):
        os.makedirs(dr)
        print (f" $ mkdir {dr}")


# environment variables (to replace variable references with)
VARS = {
    "JOBS": 16,
    "SOURCE": f"{os.getcwd()}/deps/src",
    "PREFIX": f"{os.getcwd()}/deps/prefix",
}

# declare variables
PREFIX = Var('PREFIX')
SOURCE = Var('SOURCE')
JOBS   = Var('JOBS')

# add prefix/bin to the path
os.putenv("PATH", os.getenv("PATH", "") + ":" + str(PREFIX) + "/bin")

# original cwd
orig_cwd = os.getcwd()


# configure, make, install target creation
def target_CMI(name, folder, tarball, config_opts, arts=[], deps=[]):
    folder = str(SOURCE + "/" + folder)
    # tar file name
    tarname = tarball[tarball.rfind('/')+1:]
    return Target(name, [
        Download(SOURCE + "/" + tarname, tarball),
        Extract(SOURCE, SOURCE + "/" + tarname),
        Goto(folder),
        Configure(opts=config_opts),
        Make(),
        Make("install"),
        AddArtifacts([
            PREFIX + f"/lib/lib{name}.a",
            PREFIX + f"/lib/lib{name}.lib",
            PREFIX + f"/lib/lib{name}.so",
            PREFIX + f"/lib/lib{name}.dylib",
            PREFIX + f"/lib/lib{name}.dll",
        ] + arts, True)
    ], deps)


# CMake, make, install target creation
def target_CCMI(name, folder, tarball, config_opts, arts=[], deps=[]):
    folder = SOURCE + "/" + folder
    # tar file name
    tarname = tarball[tarball.rfind('/')+1:]
    return Target(name, [
        Download(SOURCE + "/" + tarname, tarball),
        Extract(SOURCE, SOURCE + "/" + tarname),
        Goto(folder),
        CMakeConfigure(opts=config_opts),
        Make(),
        Make("install"),
        AddArtifacts([
            PREFIX + f"/lib/lib{name}.a",
            PREFIX + f"/lib/lib{name}.lib",
            PREFIX + f"/lib/lib{name}.so",
            PREFIX + f"/lib/lib{name}.dylib",
            PREFIX + f"/lib/lib{name}.dll",
        ] + arts, True)
    ], deps)


# which libraries to build?
libs = [
    target_CMI("gmp", "gmp-6.2.0", "https://ftp.gnu.org/gnu/gmp/gmp-6.2.0.tar.xz", 
        "--disable-shared --enable-static --disable-assembly CFLAGS='-fPIC'"),
    target_CMI("ffi", "libffi-3.3", "https://github.com/libffi/libffi/releases/download/v3.3/libffi-3.3.tar.gz", 
        "--disable-shared --enable-static CFLAGS='-fPIC' >/dev/null"),

    # libav
    target_CMI("ffmpeg", "ffmpeg-192d1d3", "https://git.ffmpeg.org/gitweb/ffmpeg.git/snapshot/192d1d34eb3668fa27f433e96036340e1e5077a0.tar.gz",
        "--pkgconfigdir='" + PREFIX + "/pkgconfig' --pkg-config-flags='--static' --extra-cflags='-I" + PREFIX + "/include' --extra-ldflags='-L" + PREFIX + "/lib' --extra-libs='-lpthread -lm' --enable-static --disable-debug --disable-shared --disable-ffplay --disable-doc --enable-openssl --enable-gpl --enable-version3 --enable-nonfree --enable-pthreads --enable-libvpx --enable-libmp3lame --enable-libopus --enable-libtheora --enable-libvorbis --enable-libx264 --enable-libx265 --enable-runtime-cpudetect --enable-libfdk-aac --enable-avfilter --enable-libopencore_amrwb --enable-libopencore_amrnb --enable-filters --enable-libvidstab --enable-libaom", [], [
        # sub dependencies
        target_CMI("yasm", "yasm-1.3.0", "https://www.tortall.net/projects/yasm/releases/yasm-1.3.0.tar.gz",
            ""),
        target_CMI("nasm", "nasm-2.14.02", "https://www.nasm.us/pub/nasm/releasebuilds/2.14.02/nasm-2.14.02.tar.gz",
            "--disable-shared --enable-static"),
        target_CMI("opencore", "opencore-amr-0.1.5", "https://deac-riga.dl.sourceforge.net/project/opencore-amr/opencore-amr/opencore-amr-0.1.5.tar.gz",
            "--disable-shared --enable-static"),
    ])
]

# artifacts
arts = []

for lib in libs:
    os.chdir(orig_cwd)
    lib.build()
    arts += lib.artifacts

print ("-" * 80)

print (f"{bcol.HEADER}ARTIFACTS:{bcol.RESET}")

for art in arts:
    print ("  " + str(art))

