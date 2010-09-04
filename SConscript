from SCons.Util import flatten
import os
import sys

Import('env')
lib_env = env.Clone()
bin_env = env.Clone()

lib_env.Append(SWIGFLAGS=['-lua'])
###
def build_my_header(target, source, env):
   t_name = str(target[0])
   
   # Auto-generate header for swig files
   swig_hdr = open(t_name, "w")

   swig_hdr.write("#ifndef _SWIG_AUTOGEN_\n")
   swig_hdr.write("#define _SWIG_AUTOGEN_\n")

   for f in source:
      (dirName, fileName) = os.path.split(str(f))
      (fileBaseName, fileExtension)=os.path.splitext(fileName)
      swig_hdr.write("extern int luaopen_" + fileBaseName + "(lua_State* L);\n")

   swig_hdr.write("\n#define LOAD_SWIG_LIBS(L) {\\\n");
   for f in source:
      (dirName, fileName) = os.path.split(str(f))
      (fileBaseName, fileExtension)=os.path.splitext(fileName)
      swig_hdr.write("luaopen_" + fileBaseName +"(L);\\\n") 

   swig_hdr.write("}\n\n")
   swig_hdr.write("#endif // _SWIG_AUTOGEN_\n")
   swig_hdr.close()
###

excluded_files = []

# Files used to create the binary
executable_files = ['example/main.cpp']
executable_files = [f for f in executable_files if f not in excluded_files]

# Files used for shared library which binary requires
core_files = Glob('engine/core/*.c', strings=True)
c1x_files = Glob('lib/C1x/*.c', strings=True)
script_files = Glob('engine/script/*.c', strings=True)
swig_files = Glob('engine/script/swig/*.i', strings=True)
env.Command("swig_autogen.h", swig_files, build_my_header)

# 
lib_files = core_files + c1x_files + script_files + swig_files

lib_files = [f for f in lib_files if f not in excluded_files]     # Omit excluded files
lib_files = [f for f in lib_files if f not in executable_files]   # Omit executable source files

# Additional paths required for specific platforms
platform_cpppath = []
if (sys.platform == 'win32' or sys.platform == 'cygwin'):
    platform_cpppath = ['engine/windows']

# Build FM dynamic library
lib_env.Append(CPPDEFINES=['GC_BUILD_LIBRARY'])
core_lib = lib_env.SharedLibrary('FlightlessManicotti', 
	lib_files,
	CPPPATH = ['.','engine','lib/C1x','lib/amp/src/c','lib/lua-5.1.4/src'] + platform_cpppath,
	CCFLAGS = ['-g','-std=c99'],
	LIBS=['amp','lua'], 
	LIBPATH=['.','lib/lua-5.1.4/.build', 'lib/amp/.build']
)

executable = bin_env.Program('FlightlessManicotti',
   executable_files,
   CPPPATH = ['engine'] + platform_cpppath,
   CXXFLAGS = ['-g'],
   LIBS=['FlightlessManicotti'], 
   LIBPATH=['.']
)
bin_env.Requires(executable, core_lib)

Return(['core_lib', 'executable'])