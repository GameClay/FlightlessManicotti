from SCons.Util import flatten
import os
import sys

# Bring in additional standard C requirements
additional_std_lib_includes = ['std/C1x']

if (sys.platform == 'win32' or sys.platform == 'cygwin'): # TODO: Change this to Visual Studio, and check by-version
    additional_std_lib_includes += ['std/C99']
Export('additional_std_lib_includes')   
env = Environment(CXXFILESUFFIX='.cpp')

Export('env')

SConscript('SConscript', build_dir='.build')