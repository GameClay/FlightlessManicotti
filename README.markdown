# Flightless Manicotti
A free-time project. 

Contains some cross-platform implementation of the C1x standard.

## Untested Code
This code is untested.

## License
Unless otherwise stated in a file, this project is licensed under the [Apache License, Version 2.0](http://www.apache.org/licenses/LICENSE-2.0), the text of which can be found at the previous link, or in the LICENSE file.

Each code file has its license located at the top of the file. If any file is missing its license, it should be considered to be licensed under the Apache License, Version 2.0. 

### GPL Compatibility
This project may be used with projects which use version 3 of the GPL. Please see [Apache License v2.0 and GPL Compatibility](http://www.apache.org/licenses/GPL-compatibility.html) for more details.

### Alternate Licensing
If the Apache 2 license is incompatible with your project, and that project is an implementation of the C1x standard, please contact us and we will work something out.

## Code Convention
Code submitted to this repository should adhere to this code convention.

### Indentation and Whitespace
* Be reasonable about line length -- try and stay under 80-ish.
* Three shall be the number of the spaces used to indent, and the number of the spaces used to indent shall be three. Tabs are right out.
* There will be no space between a function-name, and the opening parenthesis.
* Curley-brackets will begin on the line following statement, unless the entire statement can fit on a single line.
* Blocks of code following case and default statements inside a switch statement should either be inline, or begin a new curley-bracket block, with the break inside the scope block.

### Types
* Use standard types, from stdint.h etc, unless not possible.

### Memory usage 
* You will align your memory.
* You will use restrict.
* [You will use const](http://gamesfromwithin.com/the-const-nazi).

### Naming
* Prefix all exposed symbol names with 'gc', and an underscore between 'gc' and the symbol name. `gc_foo()`