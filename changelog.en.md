# Alusus Changelog

## Version 0.x.x (202x-xx-xx)

### Updates to the Compiler

* Added support for member macros.
* Casting of pointers and references from a user type to the type of the first member variable of that user type is no
  longer implicit unless if that member var is defined as an injection. In other words, casting is only implicit in
  cases of inheritance.
* Added two new template argument types: module and ast_ref.
* Improved build notice reporting.
* Fixed an issue in compiling logical oeprators when the second operand uses temporary variables.
* Enabled using the `~cast` operator to cast ref types to other ref types.

### Standard Libraries Breaking Changes

* Renamed the file version of `Net.get` into `Net.getFile`.

### Updates to the Standard Libraries

* Added function for computing result type of an expression AST.
* Enabled importing multiple files at once from a package using a single call to Apm.importFile. This is useful for
  avoiding unnecessary network requests that would result from multiple calls to Apm.importFile on the same package.
* Added `String.realloc` method.
* Added `Core.addLocalization` function.
* Added `isNull` to SrdRef, WkRef, and UnqRef.
* Added `isNull` to closures.
* Enable auto casting of `SrdRef` and `WkRef` to refs of parent classes.
* Added `Regex.Matcher` type to allow initializing a regular expression once and useing it with multiple match
  operations instead of re-initializing the regex with every match operation.
* Added authentication option to `Net.getFile`.
* Added `Net.putFile` function for uploading files.

### Compiler Fixes

* Fixed an issue with imports when symbolic links are involved causing multiple imports of the same file to appear to
  the compiler as imports of different files resulting in duplicate code.
* Fixed a bug with global variable initialization when the same var is used in jit as well as preprocess during the
  execution of the same root statement, resulting in the variable being used before initialization.
* Fixed `not` operator issue causing the operator not to accept classes having Bool casting handlers.
* Fixed an issue with callee lookup of assign operators where indirect casting operators are given equal rank as
  direct casting operators resulting in multiple-match error.
* Fixed an issue with callee lookup of assign operators where multiple-match errors are not reported correctly.
* Fixed an issue with temp var definitions when the var needs to be casted using a custom caster before it can be
  assigned to the target var.
* Fixed an issue with the compiler when defining a class inside the init handler of another class, causing the
  initialization of the inner class to fail.
* Fixed an issue with `handler` command when using the `set_ptr` option multiple times in the same class on the same
  function to override the previous implementation.
* Fixed a bug with `~no_deref` operator causing an exception in certain cases.
* Fixed a bug with handling multiple callee match errors resulting in compiler crash.

### Standard Libraries Fixes

* Fixed an issue in closure pointer initialization.
* Fixed an issue with closures causing conflict with closure payload variables named `d`.
* Fixed issues in Arabic localizatoin of `Srl.Net` module.

### Other Updates and Fixes

* Modernize Alusus build scripts and make it depend on vcpkg to manage dependencies in addition to other various
  improvements to the build scripts.
* Build wasm-ld and include it with Alusus to avoid potential conflict between the version of LLVM used by
  Alusus and the version LLVM used by the wasm-ld provided by the OS.
* Fixed an issue in `apm link` command when dealing with spaces in folder names.


## Version 0.9.0 (2022-06-20)

### Updates to the Compiler

* Added support for function and handler templates.
* Added support for creating custom grammars, like custom expressions.
* Added ~use_in operator which can be used to write command packs with the ability to specify the name of the target
  variable instead of always defaulting to `this`.
* Implemented PreGenTransformStatement class which allows preprocess statements to insert code translation that
  would be performed right before generation instead of being performed during preprocessing.
* Detect circular class code generation that can be triggered in some cases of nested preprocess statements.
* Make callee tracing smarter about using or skipping the auto defined `this`.
* Minor refactoring for better source readability and maintainability.

### Updates to the Standard Libraries

* Added support for closure payload capture modes.
* Added comparison operator to closures.

### Breaking Changes

* Fixed a typo in the Arabic name of the String.slice function.

### Compiler Fixes

* Fixed a bug in handling auto created this, where an auto created `this` (for class constructors) is incorrectly
  picked up during callee lookup in some cases.
* Fixed a bug in handling missing template arguments.
* Fixed a bug in handling invalid member variables.
* Fixed a bug in handling parsing errors in handler operation.
* Fixed a bug causing exceptions during preprocessing when handling parenthesis in some cases.
* Fixed a bug in handler command when specifying a different type for `this`.
* Fixed various issues in build notices:
  - Reduce unnecessary build notice duplicates in some cases.
  - Prevent loss of source location stack when a preprocess is triggered.
  - Fixed missing build notice location stack in the ~size operator.
* Other minor fixes.

### Standard Libraries Fixes

* Reduced build notice duplicates in closures.
* Fixed an issue when closures try to access `this` in command packs or in class bodies outside of functions or
  handlers.
* Fixed an issue with String._release.
* Fixed a bug where a closure cannot use `this` of a containing member function due to conflict with the closure's
  own `this` variable.
* Other minor bug fixes.

### Other Updates

* Fix the build on arm64 architecture.
* Documentation updates and typo fixes.
* Updated the logo in the documentation.
* Various updates to GTK syntax highlighting.


## Version 0.8.1 (2021-09-27)
<details>

* Fixed a bug in closures when declared inside a template and using a template argument as the type of a closure
  argument.
* Fixed a bug in using user types as arguments to variadic functions.
* Fixed a bug in comparing variadic function pointers.
* Updated the error message for type-mismatched assign operations when the assignee is a function pointer.
</details>


## Version 0.8.0 (2021-09-06)
<details>

### Updates to the Compiler

* Added support for command packs, which are sets of statements applied on an object using the `.{}` operator. This
  enables applying those statements on the object without repeating that object name in every statement.
* Added support for anonymous functions, which are functions defined inside expressions without a name.
* Enable the `handler` command to define member functions in a syntactically cleaner way.
* Added support for object properties which can be defined by applying `@operation` modifier on functions.
* Enable the `handler` command to define class properties in a syntactically cleaner way.
* Enable the `handler` command to define class methods and properties as function pointers rather than regular
  functions. This is useful for polymorphism.
* Added the `@no_preprocess` modifier to `ast` command to prevent preprocessing on the body of the command. This feature
  enables the user to postpone preprocessing on `ast` bodies until that body is inserted into the target location.
* Added a new type of template arguments, which is `ast` type. This type enables the user to pass a code as an argument
  to templates.
* Enable the `()` operator to be customized on classes (rather than objects). By default, applying the `()` op
  on types results in a temp variable of that type on the stack. By customizing this operator the user can override
  this behavior; for example, by creating an object on the heap instead of a temp variable on the stack.
* Enable putting statement in nested blocks, i.e. enable writing a code block inside another code block without the
  inner block being the body of some command (like if statements for example). This feature is useful in controlling
  the lifecycle of local variables by defining them alongside the statements that use them inside a block, so it can
  be automatically destructed once control goes out of the block.
* Improve performance of type lookup by keeping temporary copies of lookup results to speed up future lookups.
* Some internal refactoring and clean ups.

### Updates to the Standard Libraries

* Added a library to enable closures.
* Enable the user to define custom commands by adding new entries to the grammar dynamically.
* Added `astMgr.getVariableDomain` function for enquiring about the domain in which a variable is defined.
* Enable raising build messages programmatically.
* Enable programmatically enquiring about the current location of AST insertion point.
* Enable instantiating AST objects from within Alusus programs.

### Breaking Changes

* Replace the `type` keyword with `class` for defining new user types.
* Removed support for `@shared` and `@no_bind` modifiers from function definitions and instead added support for
  `@member` modifier for marking functions as member functions. Defining functions within class bodies no
  longer automatically make them member functions; the user will now need to add the `@member` modifier to make
  them member functions. Also, defining member functions now require manually adding `this` argument to the
  function definition. The aim for these changes is to simplify function definitions and remove the confusion
  caused by these modifiers and by the preprocessing that updates function definitions behind the scenes.

### Compiler Fixes

* Fixed an issue with the `use` command that causes following the `use` target that are deeper than one level.
* Fixed an issue in the grammar of function definitions.
* Fixed an issue with type lookup when the type name is mentioned in parenthesis.
* Fixed an issue with `~ast` operator.
* Fixed an issue in the `preprocess` command where dependencies of the preprocessing code cause the re-compilation
  of the same `preprocess` statement, which results in a segmentation fault.
* Fixed an issue causing global constructors to be called more than once during the same program execution.
* Fixed an issue with `dump_ast` command causing exceptions when the command target is not found.
* Fixed an issue in reporting errors in member variable definitions causing exceptions instead of regular build errors.
* Fixed an issue in calling function pointers when the return type has custom initializations (user defined
  constructors).
* Fixed some minor bugs in callee lookup.

### Standard Libraries Fixes

* Fixed an issue with memory handling in `Array.insert` and `Array.remove` functions.
* Fixed an issue with memory handling in the `String` type.
</details>


## Version 0.7.0 (2021-02-22)
<details>

### Updates to the Compiler

  * Updates to improve object oriented programming:
    - Added definition injection feature using the `@injection` modifier. This will enable type inheritance.
    - Added pointer to member functions which can be used to enable polymorphism.
    - Added support for constructors with arguments.
  * Programming the compiler:
    - Added `preprocess` command to enable execution during compilation.
    - Added `ast` command to allow passing AST (source code trees) as data to compiler functions.
    - Added functions to dynamically generate and insert source code during the `preprocess` execution.
    - Added interoperability between Alusus code and the compiler's C++ code. This allows the programmer access to the
      compiler and its data structures.
    - Added functions for scanning and querying the source code.
  * Enable preprocessing in the root.
  * Preprocessing now happens on demand rather than up-front.
  * Enabled pointer arithmetic.
  * Enabled generating web assembly output.
  * Added `ArchInt` and `ArchWord` types as integer types with bit count equal to the bitcount of pointers on the
    current architecture.
  * Added `temp_ref` type to enable automatic conversion of values to variables during function calls, when the function
    expects a reference rather than a value.
  * Added support for variadic functions.
  * Using the curly brackets in variadic function calls saves the user from having to provide the number of arguments.
  * Template types improvements:
    - Added `~no_deref` operator to force operations to be applied to the reference rather than the referenced object.
      This enables supporting reference in template types.
    - Enabled extending a specific template instance.
    - Enabled default values for template arguments.
  * Added the global var `Process.platform` to detect the current operating system.
  * Enabled detecting the name of the file being compiled.
  * Enabled the `or` (`||`) operator in import statements.
  * Enabled importing source files without specifying file extension.
  * Enabled omitting file extension when specifying the filename in the command line.
  * Upgrade LLVM to version 10.
  * Added support for macOS.

### Updates to the Standard Libraries

  * Added smart references to simplify memory garbage collection.
  * Added the type `Map`.
  * Added a version of `Array.add` that receives a group of elements to be added at once.
  * Additions for `String`:
    - `findLast`
    - `format`
    - `parseInt`
    - `parseFloat`
  * Additions for `Build` module:
    - Support for generating wasm files.
    - Enabled setting custom build flags.

### Breaking Changes

  * Defining a function pointer inside a type makes that a member function pointer instead of a global function pointer.
    To define a global function pointer inside a type the `@shared` modifier has to be used.
  * Removed the `globals.alusus` file and moved its definitions to `Srl/srl.alusus`.
  * Removed the `Build.Exe.new` function in favor of depending on type initialization handlers. Users should now define
    a variable of type `Build.Exe` instead of using `new`.

### Compiler Fixes

  * Few bug fixes related to callee lookup.
  * Fixed a bug in generating type auto constructors.
  * Fixed a bug in casting references.
  * Fixed a bug in casting user types.
  * Fixed a bug in initializing temporary variables of template types.
  * Fixed a bug in destructing temporary variables.
  * Fixed a bug in arithmetic assign operators.
  * Fixed few bugs in constructing and destructing variables.
  * Fixed a bug in differentiating between user defined references and implicit (compiler generated) references.
  * Fixed a bug in generating global constructors.
  * Fixed a bug with dealing with wrong types in variable definitions.
  * Fixed a bug in the `~ast` operator.
  * Fixed a bug in macros when the macro contains a definition with `@merge` modifier.
  * Fixed a bug in `use` command.
  * Few bug fixes related to dealing with user errors.
  * A lot of other minor fixes.

### Libraries Fixes

  * Fixed `String.append`.
  * Fixed a bug in `Array` type.
  * Fixed a bug in defining dependencies in `Build` module.
  * Improved error messages in `Build` module.
</details>


## Version 0.6.1 (2020-02-18)
<details>

### Fixes

  * Fixed a bug in the code generator resulting in destructors not being called on function arguments.
  * Fixed a bug in the code generator causing a crash when chaining assignment operators (like: x = y = 0).
  * Fixed a bug in function `String.trim`.

### Additions and Changes

  * Added `System.exit` function to SRL.
  * Made the enlargement of Array buffer exponential rather than linear.

### Breaking Changes

  * Unified all definitions of plain strings to `ptr[array[Char]]` instead of `ptr[Char]`.
</details>


## Version 0.6.0 (2020-01-24)
<details>

### What's New

  * Added the type `ref` to simplify dealing with pointers.
  * Initial support for object-oriented programming:
    - Added support to member functions.
    - Enabled customizing type initialization & destruction.
    - Objects with custom initialization are automatically passed by reference.
    - Added the tilde command `~init` to enable initializing dynamically allocated objects.
    - Added the tilde command `~terminate` to enable destroying dynamically allocated objects.
    - Added the command `handler` to enable operator overloading.
  * Added support for template types.
  * Additions to the Standard Runtime Library:
    - `String` type to simplify working with dynamic strings.
    - `Array` type to simplify working with dynamic arrays.
  * Code in module scope or type scope is now being executed.
  * Support for building executables.
  * Made the grammar more tolerant to missing semicolons.
  * Enabled passing multiple arguments to modifiers.
  * User can now read values passed to modifiers.
  * Improvements to Alusus Package Manager (APM):
    - Enabled installing multiple packages with the same name at the same time if they belong to different authors.
    - Enabled installing multiple versions of the same package at the same time.
    - Added the commands `link` and `unlink` to simplify testing packages locally before pushing the changes upstream.
    - Improvements to notifications.
  * Big improvement to tokenizing and parsing performance.

### Breaking Changes

  * Moved regular expressions functions to a separate module (Regex).
  * Moved the function `dumpLlvmIrForElement` to `Spp` module.
  * Renamed the type `Time` to `DetailedTime` in `Time` module.

### Fixes

  * Fixed an issue when executing code at root scope.
  * Fixed an issue with `Regex.match` function.
  * Fixed few issues in APM.
  * Fixed a performance issue with templates.
  * Fixed an issue preventing macros from being used at root scope.
  * Fixed an issue with parsing synchronization after syntax errors are encountered.
  * Fixed an issue with parsing `"["` and `'['`.

### Internal Changes

  * Removed state branching from the parser. This feature wasn't being used and was affecting performance.
  * Simplified the code of the parser and the lexer.
  * Renamed MacroProcessor to AstProcessor.
</details>


## Version 0.5.0 (2019-05-20)
<details>

### What's New

  * Enable execution of code in the root scope outside of modules and functions.
  * Enabled `use` command in the root scope.
  * Enabled assigning values in variable definition statements.
  * Added a simple package manager.
  * Enabled shorter syntax for functions, modules, types, and macros. You can now define functions using the `function`
    keyword directly without the need for `def`. The same applies to modules, types, and macros.
  * Enabled multi-line comments.
  * Enabled \u, \U, \x, and \f escape sequences in strings.
  * Added a library for sending network requests.
  * Added a library for extracting zip files.
  * Added regular expression functions.
  * Added `Process.language` global variable to carry the code of the currently
    selected system language.
  * Enabled the % operator on float numbers.
  * Added line numbers to interactive mode.
  * Raise a build error when return statements are missing.
  * Raise a build error for unreachable code.
  * Limit code generation to only the elements needed for the execution rather
    than building the entire source code.
  * Improved implicit casting of pointers to pointers.

### Backwards Incompatible Changes

  * Removed the `run` command which is no longer needed.
  * Replaced the `dump_llvm_ir` command with a function.

### Fixes

  * Fixed an issue with detecting current system language.
  * Fixed an issue with build script

### Internal Changes

  * Use version 7.0.1 of LLVM instead of the old version 3.3.
  * Migrated build scripts to Python instead of Bash.
  * Use regular inheritance instead of virtual inheritance in Alusus classes.
  * Added RootScopeHandler dynamic class to allow easier overriding the handling
    of elements added to the root scope.

That's in addition to many other smaller fixes and additions to the standard
runtime liraries.
</details>


## Version 0.4.1 (2019-03-13)
<details>

### What's New

  * Added support for passing process arguments to the running start function.
  * If the start function returns a non-zero value the result is used as an exit
    code for the Core.
</details>


## Version 0.4.0 (2019-02-25)
<details>

### What's New

  * Added support for macros which allow the programmer to put a group of
    commands into a macro that can then be reused elsewhere. This feature is
    conceptually similar to C macros, but its implementation is different.
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
  * Defining a pointer without specifying its content type now defaults it to
    a void pointer.
  * Allow the definition of global vars and functions inside the bodies of user
    types. This is similar to the use of `static` keyword in C++ class members.
  * Pointer to user types are now implicitly casted to pointers of the type of
    the first member within that user type. For example, if you have a user type
    whose first element is an Int, then a pointer to that user type will now
    implicitly be castable to an Int pointer.
  * Added an experimental GUI library based on GTK. This is still experimental
    and it only includes a small subset of GTK, but the user can easily add
    any missing parts. The included elements are windows, header bars, message
    dialogs, buttons, toggle buttons, checkboxes, menus, entry fields as well as
    images. If the user needs anything outside of this it can be added by simply
    defining the needed functions. The user can refer to Alusus GTK library to
    know how it's done.

### Fixes

  * Fixed a bug in parsing expression lists.
  * Fixed a bug in function code generation.
  * Defining global variables of type array or user types is now possible.
  * Fixed an issue in detecting the system's display language.
  * Fixed a problem in parsing function args when those args have no names.
  * Few other internal fixes.
</details>


## Version 0.3.0 (2018-09-26)
<details>

### What's New

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

### Fixes

  * Improved build error messages.
  * Fixed issues where build error location was not reported correctly.
  * Fixed an issue in generating functions with non-ascii names.
  * Improved automated test coverage and unified all tests around simpler E2E
    tests.
  * Many bug fixes.

### Internal Changes

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

### What's New

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

### Fixes

  * Fixed the naming convention of the SCG source code.
  * Various cleanup and minor bug fixes.
</details>


## Version 0.2.0 (2015-04-27)
<details>

### What's New

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
    - More errors are reported before the compiler quits.
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

### Fixes

  * Expressions with multiple binary operators now work properly.
  * Handling some memory leaks.
  * Various bug fixes.
</details>
