import os, sys, platform

env = Environment()

# Compile options
AddOption('--debugbuild', dest='debug', nargs='?', const=True, default=1, help='enable debug build')
AddOption('--force32', dest='force32', help='force 32 bit build on 64 bit machine')
AddOption('--sse', dest='sse', nargs=1, type='int', default=1, help='set SSE used (0-4) on 32 bit x86. Defaults to 1 (SSE1).')

# Defaults
architecture="generic"
env['CPPDEFINES']=[]

## Set up the environment

# Am I in a 32 or 64 bit environment? Note that not specifying --sse doesn't set any x86 or x64 specific options
# so it's good to go for ANY platform
if sys.platform=="win32":
    env['ENV']['INCLUDE']=(os.environ['INCLUDE'] if os.environment.contains_key('INCLUDE') else [])
    env['ENV']['LIB']=(os.environ['LIB'] if os.environment.contains_key('LIB') else [])
    env['ENV']['PATH']=(os.environ['PATH'] if os.environment.contains_key('PATH') else [])
    if not env.GetOption('force32') and os.environ.has_key('LIBPATH') and -1!=os.environ['LIBPATH'].find("\\amd64"):
        architecture="x64"
    else:
        architecture="x86"
        if   env.GetOption('sse')==1: env['CCFLAGS']+=[ "/arch:SSE" ]
        elif env.GetOption('sse')>=2: env['CCFLAGS']+=[ "/arch:SSE2" ]
        if   env.GetOption('sse')>=3: env['CPPDEFINES']+=[("__SSE3__", 1)]
        if   env.GetOption('sse')>=4: env['CPPDEFINES']+=[("__SSE4__", 1)]
else:
    if not env.GetOption('force32') and ('x64' in platform.machine() or 'x86_64' in platform.machine()):
        architecture="x64"
    elif platform.machine() in ['i386', 'i486', 'i586', 'i686']:
        architecture="x86"
        if env.GetOption('sse'):
            env['CCFLAGS']+=["-mfpmath=sse"]
            if env.GetOption('sse')>1: env['CCFLAGS']+=["-msse%s" % str(env.GetOption('sse'))]
            else: env['CCFLAGS']+=["-msse"]

# Am I building a debug or release build?
if env.GetOption('debug'):
    env['CPPDEFINES']+=["DEBUG", "_DEBUG"]
    variant=architecture+"/Debug"
else:
    env['CPPDEFINES']+=["NDEBUG"]
    variant=architecture+"/Release"
    
## Build the libraries the runtime depends on

# Will fill in these to pass to the runtime/example build
env['KL_DEP_LIBPATH'] = []
env['KL_DEP_INCPATH'] = []
env['KL_DEPS'] = []

env['KL_DEP_ROOT'] = 'runtime/lib/'
dependencies = [
    ['amp','src/c'],
    ['lua','src'],
    ['MicroAllocator','.'],
    #['nedmalloc','.']
]

dep_build_objects = []
for lib,src_path in dependencies:
    lib_toplevel = env['KL_DEP_ROOT'] + lib
    
    env['KL_DEPS'] += [lib]
    env['KL_DEP_INCPATH'] += [os.path.abspath(lib_toplevel + '/' + src_path)]
    env['KL_DEP_LIBPATH'] += [os.path.abspath(lib_toplevel + '/' + variant)]
    
    dep_build_objects += SConscript(lib_toplevel + '/SConscript', 
      variant_dir=lib_toplevel+'/'+variant, 
      duplicate=False, 
      exports=['env'])
    
## Now build the runtime and example
    
# Where are the libs and includes located for the runtime?
env['KL_LIBPATH'] = [os.path.abspath('runtime/' + variant)]
env['KL_INCPATH'] = [os.path.abspath('runtime/')]

# If this is Visual Studio, it doesn't have some C99 headers
if sys.platform =="win32":
    env['KL_INCPATH'] += [os.path.abspath('std/C99')]
    

runtime_library = SConscript('runtime/SConscript', variant_dir='runtime/'+variant, duplicate=False, exports=['env'])
example = SConscript('example/SConscript', variant_dir='example/'+variant, duplicate=False, exports=['env'])