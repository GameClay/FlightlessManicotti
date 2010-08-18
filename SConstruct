from SCons.Util import flatten
import os

env = Environment(CXXFILESUFFIX='.cpp')

excluded_files = []

source_files = Glob('*.cpp', strings=True)
source_files = [f for f in source_files if f not in excluded_files]

shared_lib = env.SharedLibrary('FlightlessManicotti', 
	source_files,
	CPPPATH = ['.'],
	LIBS=[], 
	LIBPATH=[]
)
Return('shared_lib')