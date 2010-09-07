import os
import SCons
from SCons.Script.SConscript import SConsEnvironment

# Find files that a target depends on
def FindAllSourceFiles(self, target):
    def _find_sources(tgt, src, hdr, all):
        for item in tgt:
            if SCons.Util.is_List(item):
                _find_sources(item, src, hdr, all)
            else:
                if item.abspath in all:
                    continue

                all[item.abspath] = True

                if item.path.endswith('.c'):
                    if not item.exists():
                        item = item.srcnode()
                    src.append(item.abspath)
                elif item.path.endswith('.h'):
                    if not item.exists():
                        item = item.srcnode()
                    hdr.append(item.abspath)
                else:
                    lst = item.children(scan=1)
                    _find_sources(lst, src, hdr, all)

    sources = []
    headers = []
    
    _find_sources(target, sources, headers, {})

    return sources, headers    
SConsEnvironment.FindAllSourceFiles = FindAllSourceFiles

# Auto-build a header which declares all of the module initialization
# functions from SWIG, and provides a macro to call them all.
def BuildSwigHeader(target, source, env):
   t_name = str(target[0])
   
   # Auto-generate header for swig files
   swig_hdr = open(t_name, "w")

   swig_hdr.write("#ifndef _SWIG_AUTOGEN_H_\n")
   swig_hdr.write("#define _SWIG_AUTOGEN_H_\n")

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
SConsEnvironment.BuildSwigHeader = BuildSwigHeader
