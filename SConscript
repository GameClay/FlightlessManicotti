from SCons.Util import flatten
import os

Import('env')
coreEnv = env.Clone()

excluded_files = []

# Files used to create the binary
executable_files = ['example/main.cpp']
executable_files = [f for f in executable_files if f not in excluded_files]

# Files used for shared library which binary requires
core_files = Glob('engine/core/*.c', strings=True)
c1x_files = Glob('lib/C1x/*.c', strings=True)
script_files = Glob('engine/script/*.c', strings=True)

lib_files = core_files + c1x_files + script_files

lib_files = [f for f in lib_files if f not in excluded_files]     # Omit excluded files
lib_files = [f for f in lib_files if f not in executable_files]   # Omit executable source files

# TODO: If Windows...
platform_cpppath = ['engine/windows']

core_lib = coreEnv.SharedLibrary('FlightlessManicotti', 
	lib_files,
	CPPPATH = ['engine','lib/C1x','lib/amp/src/c','lib/lua-5.1.4/src'] + platform_cpppath,
	CCFLAGS = ['-g','-std=c99'],
	LIBS=['amp','lua'], 
	LIBPATH=['.','lib/amp','lib/lua-5.1.4/src']
)

executable = coreEnv.Program('FlightlessManicotti',
   executable_files,
   CPPPATH = ['engine'] + platform_cpppath,
   CXXFLAGS = ['-g'],
   LIBS=['FlightlessManicotti'], 
   LIBPATH=['.']
)
coreEnv.Requires(executable, core_lib)

Return(['core_lib', 'executable'])