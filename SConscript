from SCons.Util import flatten
import os

Import('env')
coreEnv = env.Clone()

excluded_files = []

# Files used to create the binary
executable_files = ['main.cpp']
executable_files = [f for f in executable_files if f not in excluded_files]

# Files used for shared library which binary requires
core_files = Glob('core/*.c', strings=True)
c1x_files = Glob('C1x/*.c', strings=True)

lib_files = core_files + c1x_files

lib_files = [f for f in lib_files if f not in excluded_files]     # Omit excluded files
lib_files = [f for f in lib_files if f not in executable_files]   # Omit executable source files

core_lib = coreEnv.StaticLibrary('FlightlessManicotti', 
	lib_files,
	CPPPATH = ['.','C1x'],
	CCFLAGS = ['-g'],
	LIBS=[], 
	LIBPATH=['.']
)

executable = coreEnv.Program('FlightlessManicotti',
   executable_files,
   CPPPATH = ['.','C1x'],
   CXXFLAGS = ['-g'],
   LIBS=['FlightlessManicotti'], 
   LIBPATH=['.']
)
coreEnv.Requires(executable, core_lib)

Return(['core_lib', 'executable'])