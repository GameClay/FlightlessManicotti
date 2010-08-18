from SCons.Util import flatten
import os

Import('env')

excluded_files = []

# Files used to create the binary
executable_files = ['main.cpp']
executable_files = [f for f in executable_files if f not in excluded_files]

# Files used for shared library which binary requires
lib_files = Glob('*.cpp', strings=True)
lib_files = [f for f in lib_files if f not in excluded_files]     # Omit excluded files
lib_files = [f for f in lib_files if f not in executable_files]   # Omit executable source files

shared_lib = env.SharedLibrary('FlightlessManicotti', 
	lib_files,
	CPPPATH = ['.'],
	LIBS=[], 
	LIBPATH=[]
)

executable = env.Program('FlightlessManicotti',
   executable_files,
   CPPPATH = ['.'],
	LIBS=[], 
	LIBPATH=[]
)

Return(['shared_lib', 'executable'])