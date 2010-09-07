# Flightless Manicotti
A free-time project.

Contains some cross-platform implementation of the C99, and C1x standards.

## Untested Code
This code is untested. Most of it has been coded on a couch. Some of it may be kind of stupid, too.

I've found a bunch of bugs, bad assumptions, and generally-dumb ideas in my code. There's no reason why someone else won't find more that I've missed.

## License
Unless otherwise stated in a file, this project is licensed under the [Apache License, Version 2.0](http://www.apache.org/licenses/LICENSE-2.0), the text of which can be found at the previous link, or in the LICENSE file.

Each code file has its license located at the top of the file. If any file is missing its license, it should be considered to be licensed under the Apache License, Version 2.0. 

### GPL Compatibility
This project may be used with projects which use version 3 of the GPL. Please see [Apache License v2.0 and GPL Compatibility](http://www.apache.org/licenses/GPL-compatibility.html) for more details.

### Alternate Licensing
If the Apache 2 license is incompatible with your project, and that project is an implementation of a C standard, please contact us and we will work something out.

## Platform Compatibility
FlightlessManicotti is intended for use across many architectures and operating systems. While all attempts will be made to keep compatibility with the widest range of platforms, the project does have priorities.

### Architecture Priorities
* ARM
* x86/x64
* PPC

### Operating System Priorities
* iOS/Android
* Windows
* MacOS
* Linux

# Philosophy
At its core, FlightlessManicotti is an implementation of several ideas and architectural-philosophies that my colleagues, and I have talked about over beer for several years. 

## Goals
### Runtime
* Target many-core platforms.
* Be able to run in a single thread.
* Trust the programmer.
* Minimize memory [re]allocation.
* Be nice to the data cache.
* Use SIMD-friendly data-structures.

### Scripting
* Communicate with other sub-systems asynchronously.
* Empower the scripter.

## Runtime
_Keep it small. Keep it simple. Keep it fast._

The runtime is a shared library, but can be compiled as a static library if necessary for the target-platform. The runtime is fast, minimal, and focused on real-time tasks. 

The runtime appears to be 'missing' several features which you would expect.

* Resource loading.
* String management.
* File format conversion.

This is intentional. These are not real-time tasks and should, instead, be performed in script.

## Scripting
_Scripting will never be fast enough._

That may not be true, but it is the philosophy for scripting in FlightlessManicotti. The runtime is coded with the assumption that scripting will never be fast enough for real time, so lets not try to do it in real time. This makes it possible to handle scripting tasks in a thread, if the platform supports it, or allows it to be processed synchronously.

The overwhelming majority of communication to-and-from script will be done via queues using a producer-consumer relationship. Immediate, synchronous handling of events is possible, however it is discouraged unless there is no other way of accomplishing the task.

The runtime is not overly concerned about the language used to implement the event handlers. This is intentional, because while FlightlessManicotti uses Lua as a scripting language, by default, there is no reason why a different mechanism for handling events should be prohibited.

## Memory Allocation
_Don't._

The philosophy for memory allocation in FlightlessManicotti is: don't. 

Unfortunately one can only go so far using this methodology, and so several allocators exist. Each allocation function has a purpose and the allocators are function-pointers which can be reassigned by the user. All allocation from the runtime must use these allocators.

**Todo**: Ensure SWIG, and all libs use these functions.

### kl_heap_alloc
The `kl_heap_alloc` and `kl_heap_free` functions allocate and free memory aligned on a 16-byte boundary.

These function pointers default to results from the [nedmalloc](http://www.nedprod.com/programs/portable/nedmalloc/) library.

### kl_heap_aligned_alloc
The `kl_heap_aligned_alloc` and `kl_heap_aligned_free` functions allocate and free memory which is aligned to a boundary other than 16-bytes. `kl_heap_aligned_alloc` will assert if an alignment of 16-bytes is requested, as you should be using `kl_heap_alloc` instead.

These function pointers default to a function which manually aligns results from the [nedmalloc](http://www.nedprod.com/programs/portable/nedmalloc/) library.

### kl_micro_alloc
The `kl_micro_alloc` and `kl_micro_free` functions allocate and free memory which is 256-bytes or smaller. Any allocation greater than 256-bytes will cause an assertion.

These function pointers default to results from the [MicroAllocator](http://codesuppository.blogspot.com/2009/09/free-open-source-micro-allocator-in-c.html) library.

# Tools
Tools used to build FlightlessManicotti.

## SCons
Currently FlightlessManicotti uses [SCons](http://www.scons.org/) to do the build. 

### Why SCons?
* It works pretty well.
* Integrates easily with SWIG.
* I dislike other build systems more than SCons.

## SWIG
FlightlessManicotti uses [SWIG](http://www.swig.org/) to wrap structures and functions for use in Lua.

## LLVM/Clang
FlightlessManicotti will use [LLVM](http://llvm.org/) and [Clang](http://clang.llvm.org/) to compile in order to perform static analysis, and link-time code generation. 

Currently Clang can be used to compile by uncommenting a line in the top-level `SConscript` file, but no static analysis has been attempted yet.

## Doxygen
FlightlessManicotti uses [Doxygen](http://www.doxygen.org/) to generate documentation.

### lua2dox
FlightlessManicotti includes a slightly-modified version of the [doxygen-lua](http://github.com/alecchen/doxygen-lua) project. The modifications are based on revision alecchen/doxygen-lua@a7b2809425bb08174c154b83018194284f189bf7.

This is included with FlightlessManicotti and is located at `tools/doxygen-lua`

# Libraries
All library dependencies are included within the repository. These libraries must be built before FlightlessManicotti can be linked.

This section is unfinished.

## amp
FlightlessManicotti uses [amp](http://github.com/bjoernknafla/amp) revision bjoernknafla/amp@a84380c4e2a6d66283bc7c33d70e6fe8f0e1e71f.

### Building
To build *amp* go to `lib/amp` and run `scons`.

## nedmalloc
FlightlessManicotti uses [nedmalloc](http://www.nedprod.com/programs/portable/nedmalloc/) revision ned14/nedmalloc@3f80a00da9d9198788ad8fca0b31f8fcb665a624.

### Building
To build *nedmalloc* go to `lib/nedmalloc` and run `scons --static-lib --notests`.

## uthash
FlightlessManicotti uses [uthash](http://github.com/ned14/uthash) revision ned14/uthash@22bf8c92ba46b3071f782e932b568597d7a9b374.

### Building
This lib is entirely implemented in headers, and no building is necessary.

## lua
FlightlessManicotti currently uses [Lua](http://www.lua.org/) version **5.1.4**.

### Building
To build *lua*, go to `lib/lua` and run `scons`

## MicroAllocator
FlightlessManicotti currently uses [MicroAllocator](http://codesuppository.blogspot.com/2009/09/free-open-source-micro-allocator-in-c.html) [**r5**](http://code.google.com/p/microallocator).

### Building
To build *MicroAllocator* go to `lib/MicroAllocator` and run `scons`.

# Languages
## C
The use of C for the runtime is as much a philosophical choice as it is practical. The runtime is intended to be fast, portable, and minimalistic.

If you are implementing a feature in the runtime, and saying to yourself, "Gosh, self, I wish I was using a higher-level language," than you should re-consider implementing the feature in script. For example: The runtime does not do file parsing. Instead it provides functions for allocating, and filling the data-structures it requires. File parsing is a non-realtime task, and as such it is not performed by the runtime.

### Why C?
* Portable.
* Fast.
* Just about everything can bind to it. 
* Doesn't lend itself to over-engineering.

### Code Convention

#### Indentation and Whitespace
* Be reasonable about line length -- try and stay under 80-ish.
* Three shall be the number of the spaces used to indent, and the number of the spaces used to indent shall be three. Tabs are right out.
* There will be no space between a function-name, and the opening parenthesis.
* Curley-brackets will begin on the line following statement, unless the entire statement can fit on a single line.
* Blocks of code following case and default statements inside a switch statement should either be inline, or begin a new curley-bracket block, with the break inside the scope block.

#### Comments
Comments can use both C `/* ... */` and C++ `// ...` styles. I prefer the C++ style unless I am hacking out a block of code, but it is just my preference. Any C compiler worth talking about allows usage of both styles, so do whatever you like for code comments.

Doxygen comments, however, must follow the specified convention.

#### Doxygen Comment Style
Doxygen comments will use C++ style comments followed by a bang `//!`.

##### Doxygen tags
All Doxygen tags should use the `@` character to denote a tag, for example `@tag` 

##### Inline documentation.
    //!< Inline doccumentation will use this notation.
    
##### Documentation blocks.
    //! Autobrief is a single line that ends with a peroid.
    //! 
    //! Detailed description may be many lines, but in between
    //! section there should be a blank line.
    //! 
    //! @note Note blocks come after details.
    //! 
    //! @attention Attention blocks come after note blocks.
    //!            If you need more than one line for any tag,
    //!            it should be indented past that tag.
    //! 
    //! @param name Parameters should be doccumented as such, and if
    //!        they require multiple lines, only indent past the 'param'. 
    //! @return Return value should be specified in the same section as 
    //!         the parameters.

#### Types
Use standard types unless not possible. This is a short table of types and the header needed to use them.
<table>
   <tr>
    <td>#include</td>
    <td>To get types...</td>
   </tr>
   <tr>
     <td>&lt;<a href="http://en.wikipedia.org/wiki/Stdint.h">stdint.h</a>&gt;</td>
      <td>`int8_t`, `uint8_t`, `int16_t`, `uint16_t`, `int32_t`, `uint32_t`, `int64_t`, `uint64_t`,
          `intptr_t`, `uintptr_t`, and more...</td>
   </tr>
   <tr>
      <td>&lt;<a href="http://en.wikipedia.org/wiki/Stdbool.h">stdbool.h</a>&gt;</td>
      <td>`bool`</td>
   </tr>
   <tr>
      <td>&lt;<a href="http://en.wikipedia.org/wiki/Stddef.h">stddef.h</a>&gt;</td>
      <td>`size_t`, `wchar_t`, `ptrdiff_t`</td>
   </tr>
</table>
If a header is not available on a given platform/compiler, than a header will be provided for that platform/compiler to allow for easy use. For example, `stdbool.h` is not provided by Visual Studio, but an implementation is provided in `std/C99/stdbool.h` and is only put into the include-path when compiling using Visual Studio.

#### Memory usage 
* You will align your memory.
* You will use restrict.
* [You will use const](http://gamesfromwithin.com/the-const-nazi).

#### Naming
* Prefix all exposed symbol names with 'kl', and an underscore between 'kl' and the symbol name. `kl_foo()`

## Lua
The scripting language for FlightlessManicotti is [Lua](http://www.lua.org/). 

### Why Lua?
* Small.
* Written in C.
* Portable.
* Performs well on embedded devices.
* Has compatible licensing.
* Powerful.
* Has an LLVM front-end, [llvm-lua](http://code.google.com/p/llvm-lua/).

### Code Convention
I haven't used Lua enough yet to put together a code convention.

#### Indentation and Whitespace
* Be reasonable about line length -- try and stay under 80-ish.
* Three shall be the number of the spaces used to indent, and the number of the spaces used to indent shall be three. Tabs are right out.

#### Doxygen Comment Style
Doxygen support for Lua is achieved using [lua2dox](http://github.com/alecchen/doxygen-lua)

Doxygen comments will use this notation `--!` 

##### Doxygen tags
All Doxygen tags should use the `@` character to denote a tag, for example `@tag`