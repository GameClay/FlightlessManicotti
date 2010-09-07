import os

# Auto-build a header which declares all of the module initialization
# functions from SWIG, and provides a macro to call them all.
def build(target, source, env):
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