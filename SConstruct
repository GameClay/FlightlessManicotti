import os, sys, platform

env = Environment()

# Compile options
AddOption('--debugbuild', dest='debug', nargs='?', const=True, default=1, help='enable debug build')
AddOption('--force32', dest='force32', help='force 32 bit build on 64 bit machine')
AddOption('--sse', dest='sse', nargs=1, type='int', default=1, help='set SSE used (0-4) on 32 bit x86. Defaults to 1 (SSE1).')
AddOption('--iphone', dest='iphone', nargs='?', default=0, help='cross-compile library for iPhone')
AddOption('--iphonesimulator', dest='iphonesimulator', nargs='?', default=0, help='cross-compile library for iPhoneSimulator')
AddOption('--microalloc', dest='microalloc', help='include John Ratcliff\'s MicroAllocator (c++)')

# Defaults
architecture="generic"

env['CPPDEFINES']=[]
env['CCFLAGS']=[]
env['LINKFLAGS']=[]

env['KL_PLATFORM_INC_PATH']=[]
env['KL_PLATFORM_LIB_PATH']=[]
env['KL_CROSS_TARGET']=0

## Set up the environment

# Force scons to always use absolute paths in everything
env['CCCOM']   =    env['CCCOM'].replace('$CHANGED_SOURCES','$SOURCES.abspath')
env['SHCCCOM'] =  env['SHCCCOM'].replace('$CHANGED_SOURCES','$SOURCES.abspath')
env['CXXCOM']  =   env['CXXCOM'].replace('$CHANGED_SOURCES','$SOURCES.abspath')
env['SHCXXCOM']= env['SHCXXCOM'].replace('$CHANGED_SOURCES','$SOURCES.abspath')

if (env.GetOption('iphone') or env.GetOption('iphonesimulator')):
   if env.GetOption('iphone'):
      architecture="armv6"
      ccprefix="arm"
      platform="iPhoneOS"
   else:
      architecture="i386"
      ccprefix="i686"
      platform="iPhoneSimulator"
      
   iphoneos_dev_target="4.2"
   env['KL_CROSS_TARGET']=1
	
   platform_dir="/Developer/Platforms/"+platform+".platform/Developer"
   platform_bin_dir=platform_dir+"/usr/bin"
   platform_sdk_dir=platform_dir+"/SDKs/"+platform+iphoneos_dev_target+".sdk"

   env['CCFLAGS']+=['-isysroot '+platform_sdk_dir]
   env['LINKFLAGS']+=['-isysroot '+platform_sdk_dir]

   env['KL_PLATFORM_INC_PATH']=[
      platform_sdk_dir+'/usr/include',
      platform_dir+'/usr/lib/gcc/'+ccprefix+'-apple-darwin10/'+env['CCVERSION']+'/include',
      
      platform_sdk_dir+'/usr/include/c++/'+env['CCVERSION'],
      platform_sdk_dir+'/usr/include/c++/'+env['CCVERSION']+'/'+ccprefix+'-apple-darwin10',
      platform_sdk_dir+'/usr/include/c++/'+env['CCVERSION']+'/'+architecture+'-apple-darwin10',
   ]
   
   env['KL_PLATFORM_LIB_PATH']=[
      platform_sdk_dir+'/usr/lib',
      platform_dir+'/usr/lib/gcc/'+ccprefix+'-apple-darwin10/'+env['CCVERSION'],
   ]

   env.Replace(CC=platform_bin_dir+"/"+ccprefix+"-apple-darwin10-gcc-"+env['CCVERSION'])
   env.Replace(CXX=platform_bin_dir+"/"+ccprefix+"-apple-darwin10-g++-"+env['CCVERSION'])
   env.Replace(LD=platform_bin_dir+"/"+ccprefix+"-apple-darwin10-gcc-"+env['CCVERSION'])

else:
   # Am I in a 32 or 64 bit environment? Note that not specifying --sse doesn't set any x86 or x64 specific options
   # so it's good to go for ANY platform
   if sys.platform=="win32":
       env['ENV']['INCLUDE']=(os.environ['INCLUDE'] if os.environ.has_key('INCLUDE') else [])
       env['ENV']['LIB']=(os.environ['LIB'] if os.environ.has_key('LIB') else [])
       env['ENV']['PATH']=(os.environ['PATH'] if os.environ.has_key('PATH') else [])
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

# Set proper preprocessor defines
if architecture == "x86":
   env['CPPDEFINES']+=["KL_ARCH_X86"]
elif architecture == "i386":
   env['CPPDEFINES']+=["KL_ARCH_IPHONE_SIMULATOR"]
elif architecture == "x64":
   env['CPPDEFINES']+=["KL_ARCH_X64"]
elif architecture == "arm":
   env['CPPDEFINES']+=["KL_ARCH_ARM"]
elif architecture == "ppc":
   env['CPPDEFINES']+=["KL_ARCH_PPC"]
else:
   env['CPPDEFINES']+=["KL_ARCH_UNKNOWN"]

# Am I building for Windows or POSIX?
if sys.platform=='win32':
   
   #
   # Pre-Processor defines
   #
   env['CPPDEFINES']+=["WIN32", "_WINDOWS", "UNICODE", "_UNICODE", "_CRT_SECURE_NO_WARNINGS"]
   
   #
   # Visual Studio Compiler Flags
   #
   env['CCFLAGS']+=["/nologo"]         # Suppress the version spam before every file is compiled
   
   env['CCFLAGS']+=["/GF"]             # String pooling
   env['CCFLAGS']+=["/Gy"]             # Enable function-level linking
   env['CCFLAGS']+=["/Zi"]             # Program database debug info
   
   env['CCFLAGS']+=["/WX"]             # Warnings as errors
   env['CCFLAGS']+=["/W3"]             # Warning level 3
   
   env['CCFLAGS']+=["/EHs"]            # No exceptions
   
   if env.GetOption('debug'):
       env['CCFLAGS']+=["/Od", "/MTd"]
   else:
       env['CCFLAGS']+=["/O2", "/MT"]
       env['CCFLAGSFORNEDMALLOC']+=["/GL"]         # Do link time code generation
       
   #
   # Visual Studio Linker Settings
   #
   env['LINKFLAGS']+=["/DEBUG"]                # Output debug symbols
   env['LINKFLAGS']+=["/LARGEADDRESSAWARE"]    # Works past 2Gb
   env['LINKFLAGS']+=["/DYNAMICBASE"]          # Doesn't mind being randomly placed
   env['LINKFLAGS']+=["/NXCOMPAT"]             # Likes no execute

   #env['LINKFLAGS']+=["/ENTRY:DllPreMainCRTStartup"]
   env['LINKFLAGS']+=["/VERSION:1.10.0"]        # Version
   #env['LINKFLAGS']+=["/MANIFEST"]             # Be UAC compatible
   env['LINKFLAGS']+=["/WX"]                    # Warnings as errors

   if not env.GetOption('debug'):
       env['LINKFLAGS']+=["/OPT:REF", "/OPT:ICF"]  # Eliminate redundants
else:
   env['CPPDEFINES']+=[]
   
   env['CCFLAGS']+=["-Wall","-fno-exceptions"]
   if env.GetOption('debug'):
       env['CCFLAGS']+=["-O0", "-g"]
   else:
       env['CCFLAGS']+=["-O2", "-g"]
       
   env['LINKFLAGS']+=[]

# The variant is all sorted out
env['VARIANT'] = variant
    
## Build the libraries the runtime depends on

# Set up the environment for nedmalloc
env['NEDMALLOC_STATIC_LIB'] = 1
env['CCFLAGSFORNEDMALLOC'] = []
env['NEDMALLOC_LIBRARYNAME'] = 'nedmalloc'
env['NEDMALLOC_NO_TESTS'] = 1
env['NEDMALLOCCPPDEFINES']=["ENABLE_TOLERANT_NEDMALLOC","USE_ALLOCATOR=1","USE_LOCKS=1"]

# Will fill in these to pass to the runtime/example build
env['KL_DEP_LIBPATH'] = []
env['KL_DEP_INCPATH'] = []
env['KL_DEPS'] = []

env['KL_DEP_ROOT'] = 'thirdparty/'
dependencies = [
    ['amp','src/c'],
    ['lua','src'],
    ['nedmalloc','.'],
    ['sqlite3','.'],
    ['lsqlite3','.'],
]

if env.GetOption('microalloc'):
   dependencies += ['MicroAllocator','.']
   env['CPPDEFINES']+=['KL_USE_MICROALLOCATOR']
   
dep_build_objects = []
for lib,src_path in dependencies:
    lib_toplevel = env['KL_DEP_ROOT'] + lib
    env['KL_DEPS'] += [lib]
    env['KL_DEP_INCPATH'] += [os.path.abspath(lib_toplevel + '/' + src_path)]
    env['KL_DEP_LIBPATH'] += [os.path.abspath(lib_toplevel + '/' + variant)]
    
    dep_build_objects += SConscript(
       lib_toplevel + '/SConscript', 
       variant_dir=lib_toplevel+'/'+variant, 
       duplicate=False, 
       exports='env'
    )
     
## Now build the runtime and example
    
# Where are the libs and includes located for the runtime?
env['KL_LIBPATH'] = [os.path.abspath('runtime/' + variant)]
env['KL_INCPATH'] = [os.path.abspath('runtime/')]

runtime_library = SConscript('runtime/SConscript', variant_dir='runtime/'+variant, duplicate=False, exports='env')
example = SConscript('example/SConscript', variant_dir='example/'+variant, duplicate=False, exports='env')