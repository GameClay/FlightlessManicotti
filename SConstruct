import os, sys, platform

# Set up an environment to specify our own OS/CPU defines
platform_defines = Environment()
platform_defines.SetDefault(ZIG_OS_ANDROID = 'KL_OS_ANDROID')
platform_defines.SetDefault(ZIG_OS_DARWIN = 'KL_OS_DARWIN')
platform_defines.SetDefault(ZIG_OS_IOS = 'KL_OS_IOS')
platform_defines.SetDefault(ZIG_OS_WINDOWS = 'KL_OS_WINDOWS')

platform_defines.SetDefault(ZIG_CPU_ARM = 'KL_ARCH_ARM')
platform_defines.SetDefault(ZIG_CPU_X86 = 'KL_ARCH_X86')
platform_defines.SetDefault(ZIG_CPU_X64 = 'KL_ARCH_X64')

# Include the core
env = SConscript(['Ziggurat/SConscript'], exports = 'platform_defines')

# Compile options
AddOption('--sse', dest='sse', nargs=1, type='int', default=1, help='set SSE used (0-4) on 32 bit x86. Defaults to 1 (SSE1).')
AddOption('--microalloc', dest='microalloc', nargs='?', default=0, help='include John Ratcliff\'s MicroAllocator (c++)')

#if sys.platform=="win32":
#   if env.GetOption('sse')==1: env['CCFLAGS']+=[ "/arch:SSE" ]
#   elif env.GetOption('sse')>=2: env['CCFLAGS']+=[ "/arch:SSE2" ]
#      if   env.GetOption('sse')>=3: env['CPPDEFINES']+=[("__SSE3__", 1)]
#      if   env.GetOption('sse')>=4: env['CPPDEFINES']+=[("__SSE4__", 1)]
#else:
#   if env.GetOption('sse'):
#      env['CCFLAGS']+=["-mfpmath=sse"]
#      if env.GetOption('sse')>1: env['CCFLAGS']+=["-msse%s" % str(env.GetOption('sse'))]
#      else: env['CCFLAGS']+=["-msse"]

# Am I building for Windows or POSIX?
if (env['OS_GROUP'] == 'windows'):
   
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
   # Common GCC flags
   env.Append(CPPFLAGS = ['-fno-exceptions'])

   if env['OS'] == 'ios':
      env.Append(CPPFLAGS=['-Wno-constant-conversion',
                           '-Wno-unused-value'])

   if env['VARIANT'] == 'release':
      env.Append(CPPFLAGS=['-Os'])
   else:
      env.Append(CPPFLAGS=['-O0','-ggdb'])

## Build the libraries the runtime depends on
dependencies = [
   'amp',
   'lua',
   'sqlite3',
   'lsqlite3',
   'openctm',
   'kiss_fft'
]

if env.GetOption('microalloc'):
   dependencies += 'MicroAllocator'
   env['CPPDEFINES'] += ['KL_USE_MICROALLOCATOR']

#if not (env['OS'] == 'android'):
#   dependencies += 'nedmalloc'
#   env['CPPDEFINES'] += ['KL_USE_NEDMALLOC']

prev_lib = None
for lib in dependencies:
   lib_toplevel = 'runtime/src/thirdparty/' + lib

   built_lib = env.SConscript(
      lib_toplevel + '/SConscript',
      variant_dir = '$OBJDIR/lib/' + lib,
      duplicate = False)
   env.Prepend(LIBS = built_lib)
   env.Install('$DISTDIR/lib', built_lib)
   if not (prev_lib == None):
      env.Depends(built_lib, prev_lib)
   prev_lib = built_lib

## Now build the runtime and example
runtime_library = env.SConscript('runtime/SConscript', variant_dir='$OBJDIR/runtime', duplicate = False)

# Install in distribution folder
#env.Install('$DISTDIR/inc/GorillaGraph', env.Glob('inc/GorillaGraph/*.h'))
env.Install('$DISTDIR/lib', runtime_library)
env.Append(LIBPATH = [env.Dir('$DISTDIR/lib')])
env.Prepend(LIBS = runtime_library)

example = env.SConscript('example/SConscript', variant_dir='$OBJDIR/example', duplicate=False)