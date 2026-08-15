# Spp Module Reference

The module `Spp` contains functions to deal directly with `spp` library.
Which is the library that is responsible for supporting standard programming paradigm. The elements of this module allow users to deal
directly with the compiler from their programs.

## buildMgr Object

This singleton object allows the user to deal with the executable code generator, and contains the following functions:

#### dumpLlvmIrForElement

```
  handler this.dumpLlvmIrForElement (element: ref[TiObject]);
```

`dumpLlvmIrForElement` function prints the intermediate code for a specific element from the source code. The printed intermediate code
is a `LLVM IR` code.

The function accepts one argument which is a pointer to the AST for that element. 

It is possible to get this pointer using `~ast` command as
shown in the next example:

```
  Spp.buildMgr.dumpLlvmIrForElement(myFunc~ast);
```

#### buildObjectFileForElement

```
  handler this.buildObjectFileForElement (
    element: ref[TiObject],
    filename: ptr[array[Char]],
    targetTriple: ptr[array[Char]]
  ): Bool;
```

This function creates object file for the fiven element. Later it is possible to pass that file to the linker to create an executable file.

The third argument is a value that determines the architecture used in the building. In case of passing 0 to this argument, the system's current
architecture will be used. For example, to build an executable code with web assembly architecture we should pass "wasm32-unknown-unknown".

For more information about this value, it is possible to refer to `LLVM` documentation. 

This function returns 1 in case of success, 0 otherwise.

```
  Spp.buildMgr.buildObjectFileForElement(MyModule~ast, "output_filename", 0);
```

#### raiseBuildNotice

```
  func raiseBuildNotice (
    code: ptr[array[Char]], severity: Int, astNode: ref[TiObject]
  );
```

This function allows the programmer to raise a build notification programitically during preprocessing. 

This function takes 3 arguments:

* **code:** Error notification code we want to show to the user. The list of available codes could be found inside `Notices_L18n` folder.

* **severity:** A value that indicates the severity of the notification and could be one of the following values:

  * **0:** The notification was for a root error and build could not proceed after it.

  * **1:** An error notification but the compiling could proceed to find other errors if exists.

  * **2:** An imporatant error notification that user should revise it.

  * **3:** Non important error notification that could be ignored.

  * **4:** An informative notification which is not an error.

* **astNode:** A reference for the ast element related to the notification. The notification will point to the location
  of the source code that the element appears in.


Example:

```
  def errorCode: "SPPH1006";
  Spp.buildMgr.raiseBuildNotice(errorCode, 1, funcArgNode);
  // The above line will show an build error: Invalid function argument name.
```

## grammarMgr Object

This singleton object allows the user to create new grammar rules for the language, and contains the following functions:

#### addCustomCommand

```
def TiObject: alias Core.Basic.TiObject;

handler this.addCustomCommand (
    identifier: ptr[array[Char]],
    grammarAst: ref[TiObject],
    handler: ptr[func (SrdRef[TiObject]): SrdRef[TiObject]]
);
```

This function is used to dynamically add a grammar rule for a new command to the language. This function accepts three arguments:

* **identifier:** A string used as an identifier for the new rule.
  It is possible to pass any value for this identifier as long as we use letters and digits only and this identifier does
not conflict with an existing identifier.

* **grammarAst:** An ast that explains the new command. This tree takes the following form:

  ```
  ast {
      keywords: &lt;keywords of the command&gt;;
      args: &lt;description of the args following the keyword&gt;;
  }
  ```

  The command could have one keyword, or multiple ones (separated by | in the definition). The command could accepts no
  arguments, or accepts one argument or multiple arguments (separated by + in the definition). Every argument from these arguments
  consits of the path that leads to the parsing rule for the argument followed by * sign followed by parentheses that contain the lower
  and upper limits (respectively) for this argument appearence inside the command.
  Refer to the source code for Alsus rules for more information about the available commands that could be used as arguments.

* **handler**: A function the accepts the ast that compiler created when parsing the source code.
It processes it then then return an ast that inserted as a final result for parsing this command.

Example:

```
Spp.grammarMgr.addCustomCommand(
    "TestCommand",
    ast {
        keywords: "test_cmd";
        args: "module".Expression(0, 2) + "module".Set*(1,1);
    },
    func (args: SrdRef[TiObject]): SrdRef[TiObject] { ... }
);
```

The above example defines a new command that starts with the keyword `test_cmd` then followed by an argument with struct type
that could appear once or twice consecutively or not at all, these expressions are followed by an expression with type set that
must appear exactly once.

#### addCustomGrammar

```
def TiObject: alias Core.Basic.TiObject;

handler this.addCustomGrammar (
    identifier: ptr[array[Char]],
    baseIdentifier: ptr[array[Char]],
    grammarAst: ref[TiObject]
);
```

This function is used to dynamically add a new grammar rule to the language. It is different from `addCustomCommand` function in that the new
rule is not restricted to be a command (it could be an expresssion or anything) and the new rule is derived from another rule.

This function accepts three arguments:

* **identifier:** A string that is used as an identifier for the new rule.
  It is possible any value to this identifier as long as it consists of letters and digits only, and does not conflict with an already
  existed identifier.

* **baseIdentifier:** The identifier of the base rule that this rule is derived from it.

* **grammarAst:** An ast that describes the changes that will applied on the new rule compared to the base rule.

  This tree takes the following form:

  ```
  ast {
      &lt;path_to_value_to_update&gt;: { &lt;new_value&gt; };
      ...
  }
  ```

  Rule's tree contains a set of inputs, each input is consist from the path to the value we want to change then the new value.
  The path is relative to the root of the rule.

In the next example a new rule will be created that is derived from the rule `FuncSigExpression` with one change which is changing the
arguments of the sub-rule `BitwiseExp`. Look at Alusus rules to know the rules that could be derived from and what you need to change
when you derive from them.

```
Spp.grammarMgr.addCustomGrammar(
    "ClosureSigExp",
    "FuncSigExpression",
    ast {
        BitwiseExp.vars: { enable: 1 }
    }
);
```

## astMgr Object

This singleton object contains a functions for dealing with AST. It contains the following functions:

#### findElements

```
handler this.findElements (
    comparison: ref[Core.Basic.TiObject],
    target: ref[Core.Basic.TiObject],
    flags: Word
): Array[ref[Core.Basic.TiObject]];

handler this.findElements(
    comparison: ref[TiObject],
    target: ref[TiObject],
    flags: Word,
    modifierKwd: CharsPtr,
    kwdTranslations: ref[Map[String, String]]
): Array[ref[TiObject]];
```

Searches through the soruce code for elements that match the given search criteria.

The first two arguments are references on two ASTs. The first one to an expression that represents the search criteria while the second is the ast that we want to search in it.

The third argument could be one of the following values:

* **SeekerFlags.SKIP_OWNERS**

  Tell the seeker to not search in namespaces that the current search location is in. For example, if you search in
  `Main.Sub` then it will look in `Sub` first then in `Main`, so if you add this option it will look only in `Sub`.

* **SeekerFlags.SKIP_USES**

  Tells the seeker to ignore `use` statements while searching. Without this option, the seeker will search in namespaces
  where these expressions point to.

* **SeekerFlags.SKIP_CHILDREN**

  Tells the seeker to not look in sub-namespaces. For example, if you search in `Main` namespace and this namespace contains
  the sub-namespace `Sub` then the seeker will search in `Main` then in `Sub` if you not give it this option. If you do it will look
  in `Main` only.

The fourth argument is the keyword of the modifier to filter by, if the user wants to filter the results also by modifiers.
 
The fifth argument is a list of translations for modifier keywords to enable the user to filter by modifiers regardless of the localization
of the source code.

* **Note on kwdTranslations direction**: the map must be keyed by the alternate/localized keyword, with the canonical keyword (the one you pass as `modifierKwd`) as the value — i.e. `{localized_keyword: canonical_keyword}`, not the reverse.

The next example search in all functions inside a specific class:

```
  elements = Spp.astMgr.findElements(
    ast { elementType == "function" },
    MyClass~ast,
    Spp.SeekerFlags.SKIP_OWNERS | Spp.SeekerFlags.SKIP_USES | Spp.SeekerFlags.SKIP_CHILDREN
  );
```

It is possible to use search criteria by type or by the modifiers applied on the element. It is also possible to use a composite
condition that contains `and` and `or` operators. Some examples of search criteria:

```
  elementType == "function" // search for functions
  elementType == "type" // search for types
  elementType == "module" // search for modules
  elementType == "var" // serach for variables
  modifier == "public" // search for elements with @public modifier
  elementType == "func" && modifier == "public" // search for functions with @public modifier
```

#### getDefinitionName

```
  handler this.getDefinitionName (
      element: ref[Core.Basic.TiObject]
  ): String;
```

Returns the name of the given element, which is the name of the definition owning it.

#### getModifiers

```
  handler this.getModifiers (
      element: ref[Core.Basic.TiObject]
  ): ref[Core.Basic.Containing];
```

Get the list of modifiers applied on the given element.

#### findModifier

```
  handler this.findModifier(
      modifiers: ref[Core.Basic.Containing],
      kwd: ptr[array[Char]]
  ): ref[Core.Basic.TiObject];
```

Find a modifier inside a list of modifiers. Seach is done using the keyword of the modifier. For example, to seach for
`@expname` you need to pass the keyword `expname`.

#### findModifierForElement

```
  handler this.findModifierForElement(
    element: ref[Core.Basic.TiObject],
    kwd: ptr[array[Char]]
  ): ref[Core.Basic.TiObject];

  handler this.findModifierForElement(
    element: ref[Core.Basic.TiObject],
    kwd: ptr[array[Char]],
    kwdTranslations: ref[Map[String, String]]
  ): ref[Core.Basic.TiObject];
```

Find the modifier by the given keyword on the given element. The second form of this method translates
modifiers against the given translations map before comparing them against the requested keyword.

#### getModifierKeyword

```
  handler this.getModifierKeyword(
    modifier: ref[Core.Basic.TiObject]
  ): Srl.String;
```

Returns the keyword for the given modifier.

#### getModifierParams

```
handler this.getModifierParams(
    modifier: ref[Core.Basic.TiObject],
    result: ref[Array[Core.Basic.TiObject]]
) => Bool;
```

Returns a list of all arguments passed to the modifier.
This functions returns a boolean with value 1 on success, and 0 on failure.

#### getModifierStringParams

```
handler this.getModifierStringParams(
    modifier: ref[Core.Basic.TiObject],
    result: ref[Array[String]]
) => Bool;
```

Returns a list of all string arguments passed to the modifier. For example, if we have `deps["lib1", "lib2"]` modifier and we use
this function, then we get from it a list for two elements, the first is "lib1", and the second is "lib2".
This functions returns a boolean with value 1 on success, and 0 on failure.

#### getClassVars

```
handler this.getClassVars (parent: ref[TiObject]): Array[ref[TiObject]];

handler this.getClassVars (
    parent: ref[TiObject],
    kwd: ptr[array[Char]],
    kwdTranslations: ref[Map[String, String]]
): Array[ref[TiObject]];
```

Gets the list of variables of a given type. The second version brings only the variables carrying a
specific modifier.

#### getClassVarNames

```
handler this.getClassVarNames (parent: ref[TiObject]): Array[String];

handler this.getClassVarNames (
    parent: ref[TiObject],
    kwd: ptr[array[Char]],
    kwdTranslations: ref[Map[String, String]]
): Array[String];
```

Gets the list of names of variables of a given type. The scond version brings only the names of
variables carrying a given modifier.

#### getClassFuncs

```
handler this.getClassFuncs (parent: ref[TiObject]): Array[ref[TiObject]];

handler this.getClassFuncs (
    parent: ref[TiObject],
    kwd: ptr[array[Char]],
    kwdTranslations: ref[Map[String, String]]
): Array[ref[TiObject]];
```

Get the list of functions of a given type. The second version brings only the functions carrying
a given modifier.

#### getClassFuncNames

```
handler this.getClassFuncNames (parent: ref[TiObject]): Array[String];

handler this.getClassFuncNames (
    parent: ref[TiObject],
    kwd: ptr[array[Char]],
    kwdTranslations: ref[Map[String, String]]
): Array[String];
```

Gets the list of names of functions of a given type. The second version gets only the names of
functions having a specific modifier.

#### getFuncArgTypes

```
handler this.getFuncArgTypes (element: ref[TiObject]): ref[TiObject]
```

Gets the list of argument definitions for the given function.

#### getFuncArgType

```
handler this.getFuncArgType (element: ref[TiObject], index: Int): ref[TiObject]
```

Gets the definition of the given function's argument at the given index.

#### getSourceFullPathForElement

```
handler this.getSourceFullPathForElement(
    element: ref[Core.Basic.TiObject]
) => String;
```

Returns the full file name with the path for the source code file that contains the given element.

#### getSourceDirectoryForElement

```
handler this.getSourceDirectoryForElement(
    element: ref[Core.Basic.TiObject]
) => String;
```

Returns the full folder path which contains the source code file that contains the given element.

#### insertAst

```
handler this.insertAst(
    element: ref[Core.Basic.TiObject],
    interpolations: ref[Map[String, ref[Core.Basic.TiObject]]]
) => Bool;
handler this.insertAst(
    element: ref[Core.Basic.TiObject],
    interpolations: ref[Map[String, SrdRef[Core.Basic.TiObject]]]
) => Bool;
```

Inserts the given ast in the current position after applying the given interpolations on the given ast in a way similar to how macros work.

The first argument to this function is the tree to be inserted, while the second argument holds the interpolations list.

The position where the ast will be inserted is the current position for the preprocessing, which means the position where `preprocess`
expression that called `insertAst` appeared.

The next example inserts 10 definitions to variables with type `Int`, its names are
`n0` to `n9`:

```
  def i: Int;
  for i = 0, i < 10, ++i {
      def counter: TiStr = String.format("%i", i);
      Spp.astMgr.insertAst(
          ast { def n__counter__: Int },
          Map[String, ref[TiObject]]().set(String("counter"), counter)
      );
  }
```

#### buildAst

```
handler this.buildAst(
    element: ref[Core.Basic.TiObject],
    interpolations: ref[Map[String, ref[Core.Basic.TiObject]]],
    result: ref[SrdRef[Core.Basic.TiObject]]
) => Bool;
handler this.buildAst(
    element: ref[Core.Basic.TiObject],
    interpolations: ref[Map[String, SrdRef[Core.Basic.TiObject]]],
    result: ref[SrdRef[Core.Basic.TiObject]]
) => Bool;
handler this.buildAst(
    element: ref[Core.Basic.TiObject],
    interpolations: ref[Map[String, ref[Core.Basic.TiObject]]]
): SrdRef[Core.Basic.TiObject];
```

This function is similar to `insertAst` function, except that it creates ast and returns it to the caller instead of inserting it
directly in the current position of preprocessing. The user could later insert the result in the current position using `insertAst`.

The next example, is a modification of the previous one to use this function and also clearing the variables that it defines.
The example creates a definition then use that definition as an interpolation in later call to `insertAst` function:

```
  def i: Int;
  for i = 0, i < 10, ++i {
      def counter: TiStr = String.format("%i", i);
      def result: SrdRef[TiObject];
      Spp.astMgr.buildAst(
          ast { def n__counter__: Int },
          Map[String, ref[TiObject]]().set(String("counter"), counter),
          result
      );
      Spp.astMgr.insertAst(
          ast {
              definition;
              n__counter = 0;
          },
          Map[String, ref[TiObject]]()
              .set(String("counter"), counter)
              .set(String("definition"), result)
      );
  }
```

#### insertCopyHandlers

```
handler this.insertCopyHandlers(obj: ref[TiObject]);

@member macro insertCopyHandlers [this];
```

Generates the copy operation and the copy constructor for the current type. It takes care of
copying all member variables of that type. This macro must be used inside the body of a user
type to generate the operations for that typel.

#### insertMixin

```
handler this.insertMixin(obj: ref[TiObject]);

@member macro insertMixin [this, target];
```

Inserts the given mixen into a class. The function inserts the mixin into the current location,
so it must be used within a `preprocess` statement. The macro is just a helper that calls the
function from a preprocess statement and gives it the AST ptr of the mixen.
Visit [mixins](#Mixins) for more info.

#### getCurrentPreprocessOwner

```
handler this.getCurrentPreprocessOwner(): ref[Core.Basic.TiObject];
```

Returns a reference to the AST element that owns the currenly running preprocessing expression.

#### getCurrentPreprocessInsertionPosition

```
handler this.getCurrentPreprocessInsertionPosition(): Int;
```

Returns the position (inside the owner of the current preprocessing experssion) in which AST elements will be inserted when
calling `insertAst` function.

#### getVariableDomain

```
handler this.getVariableDomain(element: ref[Core.Basic.TiObject]) => Int;
```

Returns a value that show the domain in which the given variable is defined. The result is one of the following values:

```
  def DefinitionDomain: {
      def FUNCTION: 0; // Var is a function local variable.
      def OBJECT:   1; // Var is a class member.
      def GLOBAL:   2; // Var is a global or shared variable.
  }
```

#### traceType

```
handler this.traceType(element: ref[Core.Basic.TiObject]) => ref[Spp.Ast.Type];
```

Traces the type that the given SPP element points to, and returns that type.

#### matchTemplateInstance

```
handler this.matchTemplateInstance(
    template: ref[Spp.Ast.Template],
    templateInput: ref[Core.Basic.TiObject],
    result: ref[SrdRef[Core.Basic.TiObject]]
) => Bool;
```

Returns the template instance that matches the given input. If no existing instance
matches the input, then creates a new instance.

#### isCastableTo

```
handler this.isCastableTo(
    srcTypeRef: ref[Core.Basic.TiObject],
    targetTypeRef: ref[Core.Basic.TiObject],
    implicit: Bool
) => Bool;
```

Checks wether the given type (srcTypeRef) can be casted as a certian other type (targetTypeRef).
The type arguments can be the type itself, or a reference to it. The third argument specifies
wether to check for implicit casting or explicit casting.

#### computeResultType

```
handler this.computeResultType(
    element: ref[Core.Basic.TiObject],
    result: ref[ref[Core.Basic.TiObject]],
    resultIsValue: ref[Bool]
) => Bool;
```

This function computes the result from the given expression and returns it. The result could be a class or any other defintion
like module or function. The last argument tells us if the result is a value (a variable from the given class for example) or the class
itself (which means it tells you whether the expression is a definition for a class, or a variable from that class).

#### cloneAst

```
handler this.cloneAst(element: ref[Core.Basic.TiObject]): Srl.SrdRef[Core.Basic.TiObject] {
    return this.cloneAst(element, nullRef[Core.Basic.TiObject]);
}
handler this.cloneAst(
    element: ref[Core.Basic.TiObject], sourceLocationNode: ref[Core.Basic.TiObject]
): Srl.SrdRef[Core.Basic.TiObject];
```

This function clonse the given AST.

The second form of this function allows the addition of a position in the source code to the source code positions stack related to the generated tree. The next argument in the next form is not the position in source code that we want to add to the stack, instead it is a the AST element we want to take the position from.

#### dumpData

```
handler this.dumpData(obj: ref[Core.Basic.TiObject]);
```

Prints the given ast to the console in a string format.

#### getReferenceTypeFor

```
handler this.getReferenceTypeFor(
    astType: ref[Core.Basic.TiObject]
): ref[Spp.Ast.ReferenceType];
```

Returns the reference type of the given type.

#### tryGetDeepReferenceContentType

```
handler this.tryGetDeepReferenceContentType(
    astType: ref[Spp.Ast.Type]
): ref[Spp.Ast.Type];
```

Returns the content type of the given type. If the given type is a reference then the function returns the content that this
reference points to, otherwise it returns the type itself. If the given type is a reference to a reference then the function
search recursively until it reaches a non-reference type and returns it.

### Mixins

Mixins are available through the `insertMixin` macro of the `AstMgr` class, which can be called
from the body of a class to insert a mixin into it, as shown in this example:

```
def MyMixin: {
  handler this.print() {
    Console.print(this.toString());
  }
}
class MyType {
  Spp.astMgr.insertMixin[MyMixin];
  // Now the class has the `print` method.
  ...
}
```

Mulitple mixins can also be merged into a single mixin usign the & operator, as in the following
example:

```
def Mixin1: {
  handler this.print() {
    Console.print(this.toString());
  }
}
def Mixin2: {
  handler this.save(filename: String) {
    def content: String = this.toString();
    Fs.createFile(filename, content.buf, content.getLength());
  }
}
def Mixins: Mixin1 & Mixin2;
class MyType {
  Spp.astMgr.insertMixin[Mixins];
  // Now the class has the `print` and the `save` methods.
  ...
}
```

## AST Types

### Types

defined inside module `Spp.Ast`.

* Type

  Derived from `Node`.

  The root for all classes in Alusus.

* DataType

  Derived from `Type`.

  The root for all data types.

* IntegerType

  Derived from `DataType`.

* FloatType

  Derived from `DataType`.

* ArrayType

  Derived from `DataType`.

* PointerType

  Derived from `DataType`.

* ReferenceType

  Derived from `DataType`.

* VoidType

  Derived from `DataType`.

* UserType

  Derived from `DataType`.

* FunctionType

  Derived from `Type`.


### Operators

Defined inside module `Spp.Ast`.

Derived from `Node`.

* AstRefOp

* CastOp

* InitOp

* TerminateOp

* NextArgOp

* DerefOp

* NoDerefOp

* ContentOp

* PointerOp

* SizeOp

* TypeOp

* UseInOp


### Statements

Defined inside `Spp.Ast`.

Derived from `Node`.

* IfStatement

* WhileStatement

* ForStatement

* ContinueStatement

* BreakStatement

* ReturnStatement

* PreprocessStatement

* PreGenTransformStatement


### Others

Defined inside module `Spp.Ast`.

Derived from `Node`.

* Template

* TemplateVarDef

* Block

* Function

* Macro

* Module

* Variable

* ArgPack

* AstLiteralCommand

* ThisTypeRef


