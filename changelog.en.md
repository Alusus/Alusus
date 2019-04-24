# Alusus Changelog

## Version 0.4.1 (2019-03-13)

### What's New:

  * Added support for passing process arguments to the running start function.
  * If the start function returns a non-zero value the result is used as an exit
    code for the Core.


## Version 0.4.0 (2019-02-25)
<details>

### What's New:

  * Added support for macros which allow the programmer to put a group of
    commands into a macro that can then be reused elsewhere. This feature is
    conceptuatlly similar to C macros, but its implementation is different.
    While macros in C are executed as a preprocessor working on characters, in
    Alusus macros are part of the parsing process, which leads to the following
    differences:
    - In Alusus macros are limited to the scope in which they are defined unlike
      C macros which can't be scoped. In other words, if you define a macro in
      Alusus within a certain scope (a module for example) then tried to use it
      outside of that scope the compiler will error out unless you mention the
      full path leading to that macro.
    - In Alusus you can define multiple macros with the same name if they are
      defined in different scopes. In C you can't do that.
    - In Alusus parsing errors inside macros are detected during the parsing of
      the macro itself, whereas in C parsing errors won't be detected until the
      macro is used somewhere.
    - In Alusus you can't define macros containing partial commands or partial
      rules like you can in C.
  * Added the command `use` which allows the programmer to tell the compiler to
    search for identifiers within a certain scope by default. For example,
    instead of having to mention `Srl.Console.print` multiple times, the user
    can type `use Srl.Console` and then elsewhere only type `print`.
  * Added support for constant definitions, which allow the user to give a name
    to a certain constant value. For example: `def PI: 3.141592`.
  * Empty brackets can now be dropped from function definitions if those
    functions take no args and return nothing.
  * Definining a pointer without specifying its content type now defaults it to
    a void pointer.
  * Allow the definition of global vars and functions inside the bodies of user
    types. This is similar to the use of `static` keyword in C++ class members.
  * Pointer to user types are now implicitly casted to pointers of the type of
    the first member within that user type. For example, if you have a user type
    whose first element is an Int, then a pointer to that user type will now
    implicitly be castable to an Int pointer.
  * Added an experimental GUI library based on GTK. This is still expecimental
    and it only includes a small subset of GTK, but the user can easily add
    any missing parts. The included elements are windows, header bars, message
    dialogs, buttons, toggle buttons, checkboxes, menus, entry fields as well as
    images. If the user needs anything outside of this it can be added by simply
    definining the needed functions. The user can refer to Alusus GTK library to
    know how it's done.

### Fixes:

  * Fixed a bug in parsing expression lists.
  * Fixed a bug in function code generation.
  * Defining global variables of type array or user types is now possible.
  * Fixed an issue in detecting the system's display language.
  * Fixed a problem in parsing function args when those args have no names.
  * Few other internal fixes.
</details>

## Version 0.3.0 (2018-09-26)
<details>

### What's New:

  * Improved the support for basic data types. The list now includes:
    - 8, 16, 32, and 64 bit integers.
    - 8, 16, 32, and 64 bit words (unsigned ints).
    - 32 and 64 bit floats.
    - boolean.
  * Basic types are now templates that take the size as argument. e.g. Int[16],
    Float[64]. Default size is considered if no params are provided.
  * Added support for function pointers.
  * Enabled pointer comparisons and casting between pointers and integers.
  * Added support for `else` clause in if statements.
  * Added support for `break` command in loops.
  * Added support for `continue` command in loops.
  * Added support for modifiers.
  * Added proper support for namespacing.
  * Enable merging definitions through the @merge modifier. This will allow
    adding more definitions into existing modules or types.
  * Signature can be omitted now from function definitions if it has no args and
    return void.
  * Added a Standard Runtime Library which includes the following sub modules:
    - Console: Functions used in the terminal.
    - System: Misc system functions.
    - Memory: Memory allocation and other related functions.
    - String: Manipulating and accessing strings.
    - File: File access functions.
    - Math: Math functions.
  * Removed the `link` command and replaced it with a modifier on normal
    function definitions.
  * Declaring user types is now done with the `type` keyword instead of
    `struct`.
  * Referencing arrays is now done with parenthesis instead of square brackets.
  * Renamed SCG (Standard Code Generator) into SPP (Standard Programming
    Paradigm).
  * Added `dump_ast` command to dump the AST tree of a certain element.
  * Renamed the `build` command, which only dumped the LLVM IR code, to
    `dump_llvm_ir`.
  * The `run` command now takes a reference to the entry point function, which
    now can be named anything.
  * Supported internationalization in build notices.
  * Improvements to implicit casting.
  * Various small improvements in expressions.
  * Added a simple interactive mode, though this is mostly useful for Alusus
    compiler or code generator developers rather than end users.

### Fixes:

  * Improved build error messages.
  * Fixed issues where build error location was not reported correctly.
  * Fixed an issue in generating functions with non-ascii names.
  * Improved automated test coverage and unified all tests around simpler E2E
    tests.
  * Many bug fixes.

### Internal Changes:

  * Complete re-write of the standard library to streamline it and make it more
    maintainable.
  * Major refactoring in the Core to simplify the code base.
  * Split target specific code in the standard library (SPP) from the rest of
    the library, making it easier in the future to target platforms other than
    LLVM.
  * Added support for template interfaces.
  * Implemented dynamic interfaces.
  * Standard libraries can now be dynamically modified at run time.
  * Removed dependency on Boost and Catch.
  * Switched the build to C++17.
  * Few changes in naming conventions.
  * Disabled RTTI, which is no longer needed.
</details>

## Version 0.2.1 (2015-11-28)
<details>

### What's New:

  * Support for casting between different pointer types.
  * Support for casting from pointer to integer.
  * Added ~size operator for obtaining the size of a variable or an expression
    in memory.
  * Added char data type.
  * Refactored the Core's data framework to make it generic and streamlined.
    This change is needed by SCG to enable it to use the Core's data framework
    to store compiled code. This work is a preparation to start implementing
    namespaces and other object-oriented features.
  * Improved performance of the parser.

### Fixes:

  * Fixed the naming convention of the SCG source code.
  * Various cleanup and minor bug fixes.
</details>

## Version 0.2.0 (2015-04-27)
<details>

### What's New:

  * Support for automatic type deduction during variable definition. For example
    this statement will automatically determine the type of i as integer:
    def i = 5;
  * Support for explicit and implicit casting.
  * Support for unary -, prefix ++, and prefix -- operators.
  * Support for calling functions defined later in the module.
  * Enabled linking to third party shared libraries.
  * Replaced some operators with more familiar operators. The assignment
    operator is now = instead of :=, and the comparison operator is now
    == instead of =. The negation operator is now using ! instead of ^.
  * The 'import' command now searches through multiple paths for the requested
    file rather than only the current directory.
  * Support for UTF8 in the lexer.
  * Added 'alias' definition type. This is useful for localization of the
    language.
  * Arabic localization. It's now possible to write your source code in Arabic.
  * Improvement to compilation error reporting:
    - Bracketed code blocks are properly skipped now when trying to find the end
      of the statement after an error.
    - Error location is properly reported now. Error messages include the source
      file, line, and column at which the error is found.
    - More errors are reported before the compiler quites.
  * Eliminated the need to modify LD_LIBRARY_PATH environment variable to run
    the compiler.
  * Internal refactoring:
    - The SCG now uses the Core's exception classes.
    - Streamlined the classes in the Core::Data namespace to improve the design
      and eliminate unneeded complexity.
    - Streamlined the Core namespace by reducing the number of inner namespaces.
    - Switched the lexer to use the same data classes defined in Core::Data
      rather than its own separate classes.
    - Improved the design of the data references subsystem of the Core making it
      more generic and extensible.
    - Removed ParsedDataBrowser and replaced its references with calls to the
      improved data references subsystem.

### Fixes:

  * Expressions with multiple binary operators now work properly.
  * Handling some memory leaks.
  * Various bug fixes.
</details>