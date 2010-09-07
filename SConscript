import os
import sys
from SCons.Util import flatten

Import('env')
runtime_env = env.Clone()
bin_env = env.Clone()

#runtime_env.Replace(CC = "clang")

# Files used for runtime library
core_files = Glob('runtime/core/*.c', strings=True)
c1x_files = Glob('lib/C1x/*.c', strings=True)
script_files = Glob('runtime/script/*.c', strings=True) + Glob('runtime/script/modules/*.c', strings=True)

# Autogenerated Lua wrappers
runtime_env.Append(SWIGFLAGS=['-lua'])
swig_files = Glob('runtime/script/swig/*.i', strings=True)
runtime_env.Command("swig_autogen.h", swig_files, runtime_env.BuildSwigHeader)

# Assemble the runtime lib source files
lib_files = core_files + c1x_files + script_files + swig_files

# Prune out excluded files
excluded_files = []

lib_files = [f for f in lib_files if f not in excluded_files]     # Omit excluded files


# Bring in additional standard  C requirements

# Nobody supports C1x yet, so include this by default.
additional_std_lib_includes = ['std/C1x']

# TODO: Change this to Visual Studio, and check by-version
if (sys.platform == 'win32' or sys.platform == 'cygwin'):
    additional_std_lib_includes += ['std/C99']

# Dependencies of the runtime lib
lib_src_dir = 'runtime/lib/'
    
runtime_lib_includes = [
   lib_src_dir + 'amp/src/c',
   lib_src_dir + 'lua/src',
   lib_src_dir + 'nedmalloc',
   lib_src_dir + 'MicroAllocator'
]

runtime_lib_paths = [
   lib_src_dir + 'lua/.build',
   lib_src_dir + 'amp/.build',
   lib_src_dir + 'MicroAllocator/.build',
   lib_src_dir + 'nedmalloc/x86/Release'
]

# Build runtime library
runtime_env.Append(CPPDEFINES=['KL_BUILD_LIBRARY'])
runtime_lib = runtime_env.SharedLibrary('FlightlessManicotti', 
	lib_files,
	CPPPATH = ['.','runtime'] + runtime_lib_includes + additional_std_lib_includes,
	CCFLAGS = ['-g','-std=c99'],
	LIBS=['amp','lua','nedmalloc','MicroAllocator','libstdc++'], 
	LIBPATH=['.'] + runtime_lib_paths
)

# Find the headers
# sources, headers = env.FindAllSourceFiles(runtime_lib)

# Build test binary
executable_files = ['example/main.cpp']
executable_files = [f for f in executable_files if f not in excluded_files]

executable = bin_env.Program('FlightlessManicotti',
   executable_files,
   CPPPATH = ['runtime'] + additional_std_lib_includes,
   CXXFLAGS = ['-g'],
   LIBS=['FlightlessManicotti'], 
   LIBPATH=['.']
)
bin_env.Requires(executable, runtime_lib)

Return(['runtime_lib', 'executable'])