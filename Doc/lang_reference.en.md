# Language Reference

This reference describes procedural programming using Standard Programming Paradigm library.

## General Concepts

### Identifiers

Identifiers are used to refer to functions, variables, or classes. They start with an alphabetic
character from English or Arabic, or the underscore symbol `_`. Identifiers can contain digits 0-9.

### Separating Statements

To separate statements, we use semicolons `;` in a way similar to a regular comma that separates
items in a list. In other words, a semicolon is not part of the statement and can be ignored if
no following statement exists.

### Round And Square Brackets

Round brackets are used in places evaluated at runtime, such as summing items in a container or
passing arguments to functions. On the other hand, square brackets are used in places evaluated
at compile time, such as specifying pointer or array types. In other words, if the information
specified by the brackets will be evaluated at compile time, we use square brackets; otherwise,
we use round brackets.

### Curly Brackets

Curly brackets are used to group statements in a block. These blocks are used in conditionals,
loop bodies, classes, functions, and modules.

### Definitions

All definitions must be done with the command `def`, which takes the following form:

```
 def &lt;identifier&gt; : &lt;definition&gt;
```

The definition can be a class name, which makes the definition result an instance of that class.
Alternatively, it can one of the definition statements, such as functions or classes.

The following example defines a variable of type `Int`:

```
 def i : Int
```

We can also define constants in the same way by putting the value itself instead of the class.
This can be done with integers, floating numbers, as well as strings literals, as shown in the
following example:

```
  def hello: "Hello World";
  def pi: 3.141592;
  def daysPerWeek: 7;
```

### General Program Structure

Every program using Alusus language must import "libalusus_spp.so" library either directly or indirectly.
That is because this library contains definitions for basic commands related to procedural programming.
For that reason, every program starts with the command "import".

```
import "libalusus_spp.so";
```

Often, you don't have to import the Standard Programming Paradigm library directly. Instead, you import
other libraries that import the SPP library, for example:

```
import "Srl/Console.alusus";
```

Programs usually contain a module definition, which includes all the definitions of that program (
classes, functions, macros, global variables, constants).
Any code that exists outside functions get executed as they are encountered. Therefore programs
usually contain a call to the entry point function, and this is usually placed at the end, after
all program elements are defined.

```
  module HelloWorld
  {
    function start {
      ...
    }
  }

  HelloWorld.start(); // Call the program's entry point.
```

### Comments

Line comments in Alusus starts with the symbol `//` and ends with line end. Block comments start
with `/*` and end with `*/`. This is similar to comments in C++.

### Modifiers

Source code elements can be tagged with modifiers that are used later for various reasons.
Modifier can change the way tagged elements are compiled and executed, or they can only add some
metadata. Modifiers have the following syntax:

```
  @&lt;keyword&gt; &lt;def_of_element_to_be_tagged&gt;
  @&lt;keyword&gt;[&lt;arguments&gt;] &lt;def_of_element_to_be_tagged&gt;
```

The compiler depends on pre-defined modifiers to add certain properties to different definitions
as shown in different parts of this document.
User can add additional custom modifiers and access them later using `Spp.astMgr`.

```
  @mymodifier["arg1", "arg2"] func myFunc { ... };
```

## Variables

Variables are defined like this:

```
def &lt;var_name&gt;: &lt;var_type&gt;;
```

For example, to define a variable of an integer type:

```
def x: Int;
```

And to initialize the variable with a value (or values) we pass those values
in parenthesis following the type, like this:

```
def x: Int(5);
```

* When a variable is define inside a function body, the variable is allocated
on the stack, so it's allocated upon entry to the function and is freed
upon leaving the function.

* When a variable is defined inside the body of a class it becomes a member
variable.

* When a variable is defined at the root scope, or within the body of a module
it becomes a global varaible allocated on the heap, and it remains valid
throughout the entire execution of the program.


### Shared Variables

Shared varialbes can be defined inside function bodies or class bodies by
attaching the `@shared` modifier to the definition. This makes the variable
allocated on the heap and makes it similar to global variables with the only
difference being that it's scoped within the body of the class or function.

```
@shared def x: Int;
```

### Global Variable Initialization

Global variables are initialized in global constructors before the program's
execution starts. The order of initialization of those variables is dependant
on the compilation process. The compiler tries to initialize them in the right
order so that variables are initialized after the initialization of other
varaibles that are used during the initialization. However, in some cases,
depending on the structure of the prgram being compiled, the compiler might
not be able to make the correct ordering decision.

The user can manually give priority to certain variables through the
`@priority` modifier. This modifier doesn't take any params and it makes the
variables carrying it have higher initialization priority than variables
without this modifier.

```
@shared def x: Int(5);
```

## Basic Types

### Int

Integer. We specify the number of bits inside Square brackets. Specifying the number of bits is optional, the default value is 32.

```
  Int // 32 bits
  Int[1]
  Int[8]
  Int[16]
  Int[32]
  Int[64]
```


### ArchInt

Integer with number of bits equal to system architecture, which means 32 on 32bit systems, and 64 on 64bit systems.



### Word

Positive integer. We specify the number of bits inside Square brackets. Specifying the number of bits is optional, the default value is 32.

```
    Word // 32 bits
    Word[1]
    Word[8]
    Word[16]
    Word[32]
    Word[64]
```

### ArchWord

positive integer with number of bits equal to system architecture, which means 32 on 32bit systems, and 64 on 64bit systems.

### Float

Float number. We specify the number of bits inside Square brackets. Specifying the number of bits is optional, the default value is 32.

```
Float // 32 bits
Float[32]
Float[64]
```

### Char

Character. This is just an alias for `Word[8]`.

### Bool

Binary value (true or false), this just an alias for `Word[1]`.

### array

An array of variables. Defined by specifying variables' type and their number inside Square brackets as follows:

```
array[the_type, element_count]
```

### ptr

A pointer to a variable. Variable's type specified inside Square brackets as follows:

```
ptr[the_type]
```


## Operators And Expressions

### Operators

What follows is a list of the operators sorted in ascending order by priority, which means first element has the least priority.

**Note: ** Some of these operators are not yet supported, but will be included in later release.

#### Assigment operator

* `=`   Set a new value.
* `+=` 	Add a value to the current value.

* `-=` 	Subtract a value from the current value.

* `*=` 	Multiply the current value by another value.

* `/=` 	Divide the current value by another value.

* `%=` 	Divide the current value by another value and keep the remainder instead of the result.

* `&=`  Apply `and` logical operation on the current value.

* `|=` 	Apply `or` logical operation on the current value.

* `$=`  Apply `xor` logical operation on the current value.

* `<<=` Shift all bits to the left by the given number.

* `=>>` Shift all bits to the right by the given number.

#### Logical operators

* `and` Logical and operation.

* `or`  Logical or operation.

* `||`  A symbolic form for or.

* `&&`  A symbolic form for and.

#### Comparison operators

* `==`  Equality check.

* `!=`  Inequality check.

* `<`   Less than check.

* `>`   Greater than check.

* `<=`  Less than or equal check.

* `>=`  Greater than or equal check.

#### Addition and subtraction operators

* `+` Addition

* `-` Subtraction

#### Multiplication and divide operators

* `*` Multiplication

* `/` Divide

* `%` Divide remainder.

#### Bitwise operators

These operators are used to apply logical operations on bit level, or apply them between the bits for two numbers. Also, these operators include shift operations.

* `|`  or operation.

* `$`  Xor operation.

* `&`  And operation.

* `<<` Shift the bits to the left by the given number.

* `>>` Shift the bits to the right by the given number.

#### Single preceding operators

* `++` Increase by 1.

* `--` Decrease by 1.

* `+` Positive number sign.

* `-` Negative number sign.

* `!` Bitwise negation sign.

* `!!` Logical negation sign.

#### Single following operators

* `++` Increase by 1.

* `--` Decrease by 1.

#### Other operators

* `.` Access an element's member.

* `{}` Statements pack.

### Controlling Priorities

Round brackets are used to control priorities in expressions. For example, the following expression execute
the multiplication before addition.

```
x = y + z * n
```

Whereas the next one execute the addition first:

```
x = (y + z) * n
```

### Literals

In addition to variables, literals can be used in expressions. This a list of literals supported in Alusus:

#### Integers
Written as decimal number, and represented with 32bit. For example: 123.

#### Floating Point Numbers
Written as real number, and represented with 32bit. For example: 1.23.

#### Strings
Enclosed in double quotations `""`, as follows:
```
Hello world!
```
Special symbols can be written using a preceding `\`. The following is a list of these symbols:

* `\n` New line.

* `\r` Return to the line start.

* `\t` Insert a tab.

* `\"` Insert a quotation.

* `\\` Insert a `\`.

## Commands

### Conditionals

Conditionals are written as follows:

```
  if &lt;condition expression&gt; &lt;statement&gt;
  if &lt;condition expression&gt; { &lt;group of statements&gt; }
  if &lt;condition expression&gt; &lt;statement_or_block&gt; else &lt;statement_or_block&gt;
```

It is optional to enclose the condition expression with a round brackets.

### Loops

Loops written as follows:

```
  while &lt;condition expression&gt; &lt;statement&gt;
  while &lt;condition expression&gt; { &lt;group of statements&gt; }
  for &lt;counter initialization&gt;, &lt;condition expression&gt;, &lt;counter update&gt; &lt;statement&gt;
  for &lt;counter initialization&gt;, &lt;condition expression&gt;, &lt;counter update&gt; { &lt;group of statement&gt; }
```

As in conditionals, it is optional to enclose the loop header (condition expression in while loops, or the three expressions of for loop)
in round brackets. For example:

```
  while r!=0 r=performOperation();
  while r!=0 { r = performOperation() }
  for i=0, i&lt;10, i++ print("%d\n", i);
  for (i=0, i&lt;10, i++) print("%d\n", i);
  for i=0, i&lt;10, i++ { print("%d\n", i) };
```

### `continue` Command

It is used to skip what left from the current iteration, and start the next one.
It is possible to specify the loop number that we want to skip in case of nested loops.

```
  continue;
  continue 2; // Break out of current loop and continue outer loop.
```

### `break` Command

Break the loop.
It is possible to specify the loop number that we want to break in case of nested loops.

```
  break;
  break 2; // Break out of current and outer loops.
```

## Functions

Functions are defined using `function` as the definition in `def` command.

```
def &lt;func name&gt; : function (&lt;arguments&gt;) =&gt; &lt;return_type&gt; { &lt;function_body&gt; }
```

Arguments take the following form:

```
&lt;arg_name&gt;:&lt;arg_type&gt;, &lt;arg_name&gt;:&lt;arg_type&gt; ...
```

For example:

```
  def factorial : function (i:Int) => Int
  {
    if i==1 return 1;
    return i*factorial(i-1);
  }
```

It is also possible to use a short form without the `def` command, as follows:

```
function &lt;func name&gt; (&lt;arguments&gt;) =&gt; &lt;return_type&gt; { &lt;function_body&gt; }
```

For example:

```
  function factorial (i:Int) => Int
  {
    if i==1 return 1;
    return i*factorial(i-1);
  }
```

Calling a function is done by using the function's name followed by the arguments list enclosed in round brackets.
In case of no arguments, the function name is followed by empty round brackets.

```
  x = readNumber();
  y = readNumber();
  drawPoint(x, y);
```

### Variadic Arguments

It is possible to define a function with variadic arguments (variadic function), which allows the
user to call the function with any number of arguments. Unlike other languages like C, in Alusus,
it is possible to specify the type of these arguments in addition to setting minimum and maximum
limits on their number. To specify that the arguments are variadic, the `...` operator is used
when defining the type of the argument. Therefore, if we prefix the type with `...`, it makes it
variadic, allowing the user to pass any number of arguments of that type.

```
  &lt;arg_group_name&gt;: ...&lt;args_type&gt;
  &lt;arg_group_name&gt;: ...[&lt;args_type&gt;, &lt;min_count&gt;, &lt;max_count&gt;]
```

Examples:

```
  // Function receiving unspecified number of args with unspecified type.
  function print (format: ptr[array[char]], ...any) { ... }
  // Function receiving unspecified number of args with type Float.
  function print (count: Int, args: ...Float) { ... }
  // Function receiving Float args counting between 2 and 5.
  function print (count: Int, args: ...[Float, 2, 5]) { ... }
```

#### Calling A Variadic Function

Calling a variadic function is the same as calling a regular one. But if the function takes the number of arguments followed by
the arguments, then it is possible to pass the arguments enclosed in curly brackets and the compiler will convert that to the number
of arguments followed by the arguments, as shown in the next example:

```
  function print (count: Int, args: ...Float) { ... }
  print({ 5.5, 3.7, 1.0 });
  // The upper call is identical to the one below; Alusus automatically inserts
  // the count (3) before the args.
  print(3, 5.5, 3.7, 1.0);
```

#### Using The Variadic Arguments Inside The Function

Accessing variadic arguments is done by using the `~next_arg` operator on the arguments list name and
providing the argument type. This operator requires the type because the definition may not have the
type specified. Therefore, the user must specify the expected arguments, for example, by using a
string param that specifies the types of the following params, similar to `printf` in C. It is
important to note that each use of the `~next_arg` operator will remove an argument from the list,
allowing only sequential access. Therefore, it is not possible to access the argument twice or access
the arguments randomly.

Additionally, determining the number of arguments and stopping when reaching the end is the
responsibility of the programmer, as the language does not provide a way to know the number of
arguments automatically. This necessitates the programmer to include an initial argument that
represents the number of subsequent arguments, as shown in the previous examples.

The following example demonstrates a function that prints variadic arguments of `Int` or `Float` type.
We specify the number of arguments and the type of each one by passing a `format` string.

```
  function print (format: ptr[Char], args: ...any) {
    while format~cnt != 0 {
      if format~cnt == '#' printInteger(args~next_arg[Int])
      else printFloat(args~next_arg[Float]);
      format = format + 1;
    }
  }
  print("#$##$", 5, 5.5, 8, 7, 2.3);
```

### Anonymous Functions

Anonymous functions are unnamed functions defined inside an expression. They are usually used when
we need to pass a pointer to a function as an argument to another function. The definition for these
functions is done by writing them like regular functions but without a name. In this case, the passed
value will be a pointer to that function, as shown in the following example:

```
  func sort (
      records: ref[Array[Record]],
      compare: ptr[func (ref[Record], ref[Record])]
  ) {
      def i: Int;
      def j: Int;
      for i = 0, i < records.getLength() - 1, ++i {
          for j = i + 1, j < records.getLength(), ++j {
              if compare(records(i), records(j)) {
                  // Replace i with j;
              }
          }
      }
  }
  // Sort ascending.
  sort(records, func (r1: ref[Record], r2: ref[Record]) {
      return r1.x > r2.x;
  });
  // Sort descending.
  sort(records, func (r1: ref[Record], r2: ref[Record]) {
      return r1.x < r2.x;
  });
```

Anonymous functions can access global variables, but they cannot access local variables inside the
enclosing function. This limitation exists because the enclosing function may end and its variables
can be removed from memory before calling the anonymous function. To access local variables in the
enclosing function, the user can use [clousers](./Srt_Reference/closure.en.md) instead.

### Functions Templates

Function templates can be defined by specifying the arguments that the function uses in its body.
These arguments are passed to the function when calling it, along with the function's regular
arguments. The key difference between the two sets of arguments is that template arguments are passed
at compile time, whereas function arguments are passed at runtime. The definition and usage of
function templates follow the following form:

```
  func &lt;func_name&gt; [&lt;template_arg_defs&gt;] (&lt;func_arg_defs&gt;): &lt;ret_type&gt; {
      &lt;func_body&gt;
  }
  &lt;func_name&gt;[&lt;template_args&gt;](&lt;func_args&gt;);
```

As shown in the next example:

```
  func getMax [T: type] (a: T, b: T): T {
    if a > b return a else return b;
  }
  getMax[Int](5, 7); // returns 7
  getMax[Float](5.3, 7.7); // returns 7.7
```

There are a lot of arguments to pass to the template, see [types templates](types templates) for more information about types of template arguments.

## User-Defined Types

Classes are defined using `class` command as the definition in `def` command.

```
  def &lt;type_name&gt; : class { &lt;definition_statements&gt; }
```

Accessing class members is done using `.` operator, for example:

```
  def Point : class {
    def x : Float;
    def y : Float
  };
  .
  .
  def p : Point;
  p.x = readValue();
  p.y = readValue();
```

It is possible to use the short form without `def` as follows:

```
  class &lt;type_name&gt; { &lt;definition_statements&gt; }
```

As show in the next example:

```
  class Point {
    def x : Float;
    def y : Float
  };
  .
  .
  def p : Point;
  p.x = readValue();
  p.y = readValue();
```

### Class Templates

Classes templates could be defined by defining arguments that the class uses in its body, these arguments are passed
when defining a variable from that class.
Definition and usage take the following form:

```
  class &lt;type_name&gt; [&lt;template_arg_defs&gt;] { &lt;type_body&gt; }

  def &lt;var_name&gt;: &lt;type_name&gt;[&lt;template_args&gt;];
```

As shown in the next example:

```
  class Point [T: type] {
    def x: T;
    def y: T;
  };

  def intPoint: Point[int];
  def floatPoint: Point[float];
```

Templates arguments could be one of the following types:

* `type`

* `function`

* `integer`

* `string`

* `module`

* `ast`

* `ast_ref`

Arguments with type `ast` could be used in the preprocessing of the class.
It allows the user to pass source code as an argument to the template, then at
preprocessing time that AST can be processed and inserted into the generated template instance.
It is worth noting that preprocessing of class templates is repeated with each instance of that
template.

It is possible to set default values for template arguments, as shown in the following example:

```
  class Point [T: type, V: integer = 0] {
    def x: T = V;
    def y: T = V;
  }

  def point1: Point[Int]; // x and y will be 0.
  def point2: Point[Int, 1]; // x and y will be 1.
```

### Methods

Methods are functions defined inside classes and called using an instance of that class.
A method is defined by defining a function inside the body of the class, adding @member modifier
to that function, and making the first argument to that function a reference to the class.

```
class &lt;type_name&gt; {
  @member func &lt;method_name&gt; (this: ref[this_type], &lt;arg_definitions&gt;): &lt;ret_type&gt; {
    &lt;method_body&gt;
  };
};

&lt;object_name&gt;.&lt;method_name&gt;(&lt;arguments&gt;);
```

Inside the method body it is possible to access the object related to that call by using
the first argument for the function which is `this`, as if the user passed the object manually.
Example:

```
  class Point {
    def x: float;
    def y: float;
    @member func getDistance (this: ref[this_type]):float {
      return Math.sqrt(this.x * this.x + this.y * this.y);
    };
  }:
  def p: Point;
  p.x = 3;
  p.y = 4;
  print(p.getDistance()); // prints 5
```

It is also possible to define the methods using `handler` command, which is just a more pleasant syntax
to the previous way.

```
class &lt;type_name&gt; {
  handler this.&lt;method_name&gt; (&lt;arg_definitions&gt;): &lt;ret_type&gt; {
    &lt;method_body&gt;
  };
};

&lt;object_name&gt;.&lt;method_name&gt;(&lt;arguments&gt;);
```

Example:

```
  class Point {
    def x: float;
    def y: float;
    handler this.getDistance ():float {
      return Math.sqrt(this.x * this.x + this.y * this.y);
    };
  }:
  def p: Point;
  p.x = 3;
  p.y = 4;
  print(p.getDistance()); // prints 5
```

### Properties

Properties are methods, but they are different in that they are called using the operators specified in the definition
instead of called like a function, as shown in the example below. Properties are defined like methods but with an additional
`operation` modifier, as follows:

```
class &lt;type_name&gt; {
  @member
  @operation["&lt;operator&gt;"]
  func &lt;method_name&gt; (this: ref[this_type], &lt;arg_def&gt;): &lt;ret_type&gt; {
    &lt;method_body&gt;
  };
};

&lt;object_name&gt;.&lt;method_name&gt; &lt;operator&gt; &lt;arguments&gt;;
```

the next example explain how to define `=` operation on a property.

```
  class Period {
    def start: float;
    def end: float;
    @member @operation["="] func length (this: ref[this_type], l:float) {
      this.end = this.start + l;
    };
  }:
  def p: Period;
  p.start = 10;
  p.length = 50; // p.end will be set to 60
```

In the previous example, assignment operation is defined on `length` property without any other operation,
which means that any other operation could not be applied on this property, including reading its value.
To be able to read property value, we need to define a new operation for the property and set operation's argument
as empty string instead of a string with the required operator. For example:

```
  class Period {
    def start: float;
    def end: float;
    @member @operation[""] func length (this: ref[this_type]): float {
      return this.end - this.start;
    };
  }:
  def p: Period;
  p.start = 10;
  p.end = 60;
  printFloat(p.length) // prints 50
```

It is also possible to use `handler` command to simplify defining those properties, as follows:

```
  class Period {
    def start: float;
    def end: float;
    handler this.length = float {
        this.end = this.start + value;
        return value;
    };
    handler this.length: float {
      return this.end - this.start;
    };
  }:
  def p: Period;
  p.start = 10;
  p.length = 50; // p.end will be 60;
  printFloat(p.length) // prints 50
```

Note that in the previous example, assignment operation contains a return statement, because `operation` command automatically defines a return type
for the property function that matches argument type.

### Handler Templates

As the case with the general functions, it is possible to define a template for methods, either by defining it as a function
and use the required modifiers to convert it to a method, or by using `operation` command and give it template arguments definition
in the following form:

```
handler [&lt;template_arg_defs&gt;] &lt;operation_expression&gt; {
    &lt;func_body&gt;
}

&lt;var_name&gt;.&lt;func_name&gt;[&lt;template_args&gt;] ...;
```

As shown in the following example:

```
  class Math {
    handler [T: type] this.getMax(a: T, b: T): T {
      if a > b return a else return b;
    }
  }
  def m: Math;
  m.getMax[Int](5, 7); // returns 7
  m.getMax[Float](5.3, 7.7); // returns 7.7
```

There are different types of arguments to pass to the template, see [class templates](#types-templates) for more information
about the types of template arguments.

### Shared Members

Shared class variables and functions are not tied to a specific instance of that class.
Instead they behave like global variables and functions. The only difference is that these global
definitions are scoped inside that class. For example:

```
  class Point {
    @shared def count: int;
    def x: int;
    def y: int;
    func getPoint (a: int, b: int): Point {
      ++count;
      x = a; // error
      y = b; // error
      def p: Point;
      p.x = a;
      p.y = b;
      return p;
    };
  }:
  def p: Point = Point.getPoint(1, 2);
  print(Point.count); // prints 1
  print(p.count); // error
  print(Point.x); // error
```

As shown in the previous example, shared variable definitions need the `@shared` modifier. Whereas
shared functions do not need any special modifier, i.e any function defined in the class is
considered shared, unless we add `member` modifier to it.

### Method Pointers

Defining a pointer to a function with `member` modifier, makes it a pointer to a method.
Which means you cannot set its value to point to a regular function, instead it must point to a method
in the same class. The following example explains that:

```
  class MyType {
    @member func doSomething (this: ref[this_type], j: Int) { ... }
    def pf: ptr[@member func (Int)];
  }

  def mt: MyType;
  mt.pf = MyType.doSomething~ptr; // correct
  mt.pf(5);

  func doSomething2 (j: Int) { ... }
  mt.pf = doSomething2~ptr; // error
```

Pointers to methods are useful for some OOP properties like polymorphism.
Note that methods must start with `this` argument, otherwise, it is not possible to call the function.

But what if we want a pointer to a general function, which is not part of the object that contains the pointer.
Defining a pointer to a function without `member` modifier is the solution, as shown in the next example:

```
  class MyType {
    @member func doSomething (this: ref[this_type], j: Int) { ... }
    def pf: ptr[func (Int)];
  }

  def mt: MyType;
  mt.pf = MyType.doSomething~ptr; // error

  func doSomething2 (j: Int) { ... }
  mt.pf = doSomething2~ptr; // correct
```

It is possible to use `handler` command to simplify defining pointers to methods. Defining a pointer
to a method is similar to defining a method, with one difference which is adding `as_ptr` at the end
of the definition before the method's body, as shown in the following example.

```
  class MyType {
    handler this.mp(i: Int) as_ptr { ... };
  }
```

The previous definition defines the pointer and the function at the same time. Also, it assigns the
pointer's value while initializing the object to point to the function.

It is also possible to use `handler` command to define the pointer only without a function to point to, as follows:

```
  class MyType {
    handler this.mp(i: Int) as_ptr;
  }
```

In case of already defined pointer, and if we want to define a function and initialize the pointer to point to that function,
we could use the following form of the `handler` command:

```
  class MyType {
    handler this.mp(i: Int) set_ptr { ... };
  }
```

Defining the pointer separately from assigning its value is useful in OOP, especially in polymorphism,
where we define a pointer in the base class, and assign its value in a child class. However, one thing
to note is that setting the method pointer in a child class rather than the same class requires
specifying the type of `this`, otherwise the compiler will look for that method pointer inside the
current class rather than the parent class. For example:

```
  class MyType {
    handler (this:ParentType).mp(i: Int) set_ptr { ... };
  }
```

### Custom Object Initialization

Users can customize object initialization, i.e. define a constructor, using `handler` command, with the following syntax:

```
  handler this~init (&lt;argument_definitions&gt;) { &lt;body&gt; };
```

When defining this operation inside the class, the compiler call this function every time it needs to initialize new object.
It is possible to define arguments for this function if we want to initialize an object from another as the case when returning
this object from a function by value, or receiving it in as function argument.
When initializing the object in one of these cases, compiler call this function and passed the original object so that the function
could copy it to `this`. Whereas in the case of not customizing initialization operation, the compiler copy object memory content.

**Note: ** when defining any customization operation, the compiler will not generate a default one in all cases, so it we must
define all initialization cases, which includes initialization without arguments and initialization form another object.
The next example explain custom initialization:

```
  class Point {
    def x: int;
    def y: int;

    handler this~init() {
      this.x = 0;
      this.y = 0;
    };
    handler this~init(src: ref[Point]) {
      this.x = src.x;
      this.y = src.y;
    };
  }:

  def p: Point; // calls this~init()
  func getPoint (): Point {
    def p: Point;
    return p; // calls this~init(src)
  };
```

In case of initialization operations with arguments, we must pass them to the variable's definition, which becomes as follows:

```
  def &lt;var_name&gt;: &lt;var_type&gt;(&lt;arguments&gt;);
```

The next example shows that:

```
  class Point {
    def x: int;
    def y: int;

    handler this~init(x: int, y: int) {
      this.x = x;
      this.y = y;
    };
  }:

  def p: Point(5, 10);
```

It is also possible to customize initialization without arguments by writing initialization operationss inside the body
of the class. This is the same as defining `handler this~init`, as shown in the next example:

```
  class Point {
    def x: int;
    def y: int;

    this.x = 0;
    this.y = 0;

    handler this~init(src: ref[Point]) {
      this.x = src.x;
      this.y = src.y;
    };
  }:
```

**Note: ** If the class contains variables with custom initialization, then this class is considered a class with custom
initialization even if the user does not define initialization operations for the class.
The reason behind that is that the compiler automatically defines initialization functions to initialize the inner elements.

### Custom Object Termination

As the case with object initialization, user could customize object termination operation (i.e. define a destructor) by adding
the following definition to the class:

```
  handler this~terminate () { &lt;body&gt; };
```

When execution exits any scope, compiler automatically call this function for all objects defined in this scope.
This function could be used to release any resources reserved by that object.

### Object Initialization Order

When initializing an object with custom initialization, the compiler starts by initializing every variable for the object
before calling the initialization function for the object itself. The same order is followed when initializing every
variable, in case it also has variables with custom initialization. In other words, initialization always starts from children
to the root. Termination follows a reverse direction, i.e. from the root to the children. So termination is
done for main object first, then for its variables then their variables, and so on, which is similar to the way depth first search
algorithm works.

### Custom Operators

It is possible to customize operators applied on the objects by using `handler` command in a similar way to customizing
initialization but with the required operation instead of `~init` command. The following example explain how to customize
equality operation on an object.

```
  class String {
    ...
    handler this = ptr[array[char]] {
      this.copyChars(value);
    };
    handler this == ptr[array[char]] {
      return this.compare(value);
    };
  };

  def s: String;
  s = "hello world";
  if s == "hello world" print("text is identical");
```

Using the method it is possible to customize any operation, including comparison, arithmetic, and bitwise operations.
It is also possible to customize parentheses as shown in the next example:

```
  class String {
    ...
    handler this(pos: int): char {
      return this.getChar(pos);
    };
  };

  def s: String;
  s = "!@#$";
  print(s(1)); // prints @
```

### Temporary Variables Custom Initialization

It is possible to customize temporary variables initialization, which initialized by writing the class name followed by
parentheses. Customizing this operation is done by customizing parentheses operation on the class itself and not on an
object from it, which means on `this_type` instead of `this`. The next example shows how this can be done.

```
class Record {
    ...
    handler this_type(): ref[Record] {
        def r: ref[Record];
        r~ptr = Memory.alloc(Record~size)~cast[ptr[Record]];
        r~init();
        return r;
    }
};

myFunc(Record()); // Will call the custom operation and allocate dynamically
    // on the heap instead of creating a temp var on the stack.
```

### Definition Injection

Definition injection allows the user to make the elements of a specific definition available immediately in the outer scope.
This feature is one of the building blocks that can be used to build other features like inheritance, smart pointers, and more.
Everything needed to make definition's elements available inside the enclosing scope is tagging the definition with `injection` modifier,
as shown in the next example:

```
  class Inner {
    def x: Int;
    func printX { ... };
  };

  class Outer {
    @injection def i: Inner;
    def y: Int;
    func printY { ... };
  };

  def o: Outer;
  o.x = 1; // compiler translates it to o.i.x
  o.y = 2;
  o.printX(); // compiler translates it to o.i.printX()
  o.printY();
```

## Type Inference

It is possible to infer information about classes at compile time using the following operators:

### ~type Operator

Using this operator it is possible to inference the type of a given variable as shown in the next example:

```
  def x: Int[64];
  def y: x~type; // y here has the same type as x, which is Int[64].

  def p: ptr;
  p = x~cast[ptr];
  p = x~cast[p~type]; // Equivalent to the upper statement.
```

Basically, this operator is useful in templates and macros where the type of the variable is not known until using
the template or the macro.

### ~size Operator

This operator is used to find the size of a class or variable in the memory, which means the number of bytes that the class
used in the memory. This operator can be used on a class, variable, or even an expression, as shown in the next example:

```
  def x: Int[16];
  def y: Int[32];
  func getInt (): Int { ... };

  Console.print(x~size); // Prints 2.
  Console.print(y~size); // Prints 4.
  Console.print(Int[64]~size); // Prints 8.
  Console.print(getInt()~size); // Prints 4.
  Console.print((x + y)~size); // Prints 4.
```

## Pointers

Pointers are used to point to locations in memory and control the contents of those locations.
They are defined by using the `ptr` built-in type, followed by the content type between square brackets.

```
  def &lt;ptr_name&gt; : ptr[&lt;content_type&gt;]
```

It is possible to access the content pointed to by a pointer with the `~cnt` operator.
Also, it is possible to get the location of any variable using `~ptr` operator, as shown in the next example:

```
  def p : ptr[Int];
  def x : Int;
  x = 5;
  p = x~ptr;
  p~cnt = 1;
  // x is now equal to 1, not 5.
```

Addition and subtraction operations are allowed on pointers. In that case the amount of increase in
pointer's value will be a multiple of class size that the pointer points to.
For example, if we add 1 to a pointer to `Int`, the value of the pointer will increase by `Int` size, which means
4 bytes when `Int` is represented using 32bit.

```
  def p1: ptr[Int[32]] = ...;
  def p2: ptr[Char] = ...;
  p1 = p1 + 1; // Incremented by 4.
  p1 = p1 + 5; // Incremented by 5 * 4.
  p2 = p2 + 1; // Incremented by 1.
  p2 = p2 + 5; // Incremented by 5.
```

## References

References are similar to how pointers work, except that it is simpler to deal with because it requires only defining the variable
as reference, then dealing with it in the same way as we deal with the regular variables, which means it is possible to access the
content without the need to use `cnt` operator.

```
  def &lt;ref_name&gt; : ref[&lt;content_type&gt;]
```

Before using the reference we need to specify the pointer's value for that reference, which vary depending on whether
the reference is an argument for some function or not.
If the reference is a function's argument then all we need is passing a variable with the same type of the reference's content
of that function, and the compiler will automatically pass a pointer to that variable and use it for that reference, as shown
in the next example.

```
  func twice (ri: ref[int]) { ri *= 2 };

  def i: int = 5;
  twice(i);
  // now i == 10
```

In the case of the reference being defined as a regular variable and not an argument to a function, we need to specify
its pointer manually as shown in the next example:

```
  def ri: ref[Int];
  def i: Int;
  ri~ptr = i~ptr;
  ri = 3;
  // now i == 3
```

It is also possible to use `~ptr` operator to make the reference point to a dynamically allocated memory.

```
  def r: ref[MyType];
  r~ptr = Memory.alloc(MyType~size)~cast[ptr[MyType]];
```

It is also possible to define a reference to a reference as shown in the next example:

```
  def rri: ref[ref[Int]];
  def ri: ref[Int];
  def i: Int;
  ri~ptr = i~ptr;
  rri~ptr~ptr = ri~ptr~ptr;
  rri = 3;
  // now i == 3
```

Note that `~ptr` operator starts always with the content. In other words, if we define x as `ref[ref[Int]]` and y as `ref[Int]`
then `x~ptr` and `y~ptr` both return a pointer to `Int`. Also, operations on references are always applied on the content regardless of
reference depth, therefore `x = 5` and `y = 5` both modify the content despite the fact that `x` is a double reference.

### Temporary Reference

If the function accepts a reference, then it is not possible to call it using a value. For example, if the function `f` return a value,
and the function `g` accepts a reference to the same class, then it is not possible to pass the value returned from `f` as an argument
to `g`. This is done for the purpose of avoiding unintended mistakes that may lead to segmentation fault, because the value only exist
temporarily in the memory so keeping a reference to that value will later cause illegal memory access.
But in some situations accepting a reference to a temporarily value is safe because the need to that temporarily value ends when exiting
the function. In that situations it is possible to define the reference as temporary reference, and the compiler will automatically pass
a reference even if we give it a value, not a variable (in that case it will convert the value to a reference automatically).
For example:

```
  func receiveRef1 (r: ref[Int]) { ... }
  func receiveRef2 (r: temp_ref[Int]) { ... }

  receiveRef1(7i32); // Error. The functions needs a variable, not a value.
  receiveRef2(7i32); // Accepted. The compiler will automatically generate a reference out of this value.
```

### ~no_deref Operator

In some cases we may need to prevent the compiler from tracking the reference to the value, as in cases where we need to change
the reference itself, not the value that it points to. In those cases we use `~no_deref` operator to tell the compiler that we need
to change the reference itself, not the value. As shown in the next example:

```
  def i: Int;
  def r: ref[Int];

  r = i; // Will change the value pointed to by r.
  r~no_deref = i; // Will make r point to i.
```

Using `~no_deref` operator on a variable that is not a reference has no effect, but it does not lead to an error.

```
  def i: Int;
  i~no_deref = 7; // Equivalent to: i = 7;
```

This operator is important in templates. Without it the templates will behave differently when using it with reference than when
used with other types. See the next example:

```
  class Tp1 [T: type] {
    def x: T;
    x = 0;
  }
  def i: Tp1[Int]; // No problem, x will be set to 0.
  def j: Tp1[ref[Int]]; // Causes segfault for updating a random location in memory.

  class Tp2 [T: type] {
    def x: T;
    x~no_deref = 0;
  }
  def k: Tp2[Int]; // No problem, value of x will be set to 0.
  def l: Tp2[ref[Int]]; // No problem, value of x will be set to 0, i.e. the pointer value.
```

## Arrays

Arrays are defined using the `array` built-in type, followed by the elements' type and count between square brackets.

```
  def &lt;array_name&gt; : array[&lt;element_type&gt;, &lt;element_count&gt;]
```

It is possible to access an element in the array by putting its index inside square brackets, as follows:

```
  def myArray : array[Int, 10];
  def i : Int;
  for i=0, i&lt;10, i++ {
    myArray(i) = factorial(i)
  }
```

## Casting

Variables could be represented by a type other than its real type using `~cast` operator followed by square brackets with the
type to be represented inside them. As shown in the next example.

```
  def f : Float;
  printInteger(f~cast[Int]);
```

## Temporary Objects

It is possible to create objects temporarily without linking it with a variable by using empty parentheses or with initialization
arguments, as shown in the next example:

```
  drawPoint(Point());
  drawPoint(Point(5, 10));
```

In the previous example, a temporarily object is created from the class `Point` when calling `drawPoint` function, and terminate
this object automatically after executing the statement. Terminating temporary objects is always done after the whole statement execution
is finished, not after executing the part that uses the temporary object, For example:

```
  print(calculateDistance(Point(5, 10), Point(20, 25)));
```

In the previous example, the two temporary objects is terminated after executing the whole statement, which means after
executing `print` function, not after executing `calculateDistance` function.

## Command Packs

This feature allows the user to execute a set of commands on an object without exiting the current execution flow, and without
repeating the object's name, that is done by using dot operator followed by a pack of statements enclosed in curly brackets.
As shown in the next example.

```
  class Point {
    def x: Int;
    def y: Int;
  }
  
  def p: Point;
  p.{ x = 5; y = 10 };
  // Upper statement is equivalent to:
  // p.x = 5;
  // p.y = 10;
```

Not exiting the current execution flow means that the program will execute command pack in a blocking fashion before continue
to execute from the point before executing the pack.

```
  drawPoint(p.{ x = 5; y = 10 });
```

In the previous example, command pack is executed on the variable `p` before passing it to `drawPoint` function.
So that is equivalent to the following:

```
  p.x = 5;
  p.y = 10;
  drawPoint(p);
```

It is also possible to execute command pack on a temporarily object, as shown in the next example:

```
  drawPoint(Point().{ x = 5; y = 10 });
```

Inside statements pack, key value `this` point to the object that we execute the statements pack on it.
So it is possible to write the previous example as follows:

```
  Point().{ x = 5; y = 10; drawPoint(this) };
```

It is also possible to use statements pack with basic types, as shown in the next example:

```
  print("Enter a number: ");
  def i: Int(getInt());
  print("Number is %s.\n", ptr[Char]().{
      if i > 0 this = "positive"
      else if i < 0 this = "negative"
      else this = "zero"
  });
```

### ~use_in Operator

In some cases, statements pack are nested, or are inside a member function (method), which could lead to conflicts
in `this` variable between nested statements packs, or between the pack and the member function.
To avoid these kinds of conflicts we can use `~use_in` operator which are used to create statements packs, just
like the `.{}` operator, but it gives the user more control in defining the main variable `this` inside
the body of statements packs. This operator allows the user to set another name to the variable instead of `this`,
as shown in the next example:

```
  class Type {
    def i: int;
    handler isNumPositive (): String {
      return String()~use_in(self){
        if this.i > 0 self = "positive"
        else if this.i < 0 self = "negative"
        else self = "zero"
      };
    }
  }
```

In this example statements pack is created using `~use_in` operator instead of `.{}` operator, and in this pack the word
`this` points to the object instead of the temporary variable of type `String`, which is pointed to by word `self`.

In case of using `~use_in` operator without specifying a name inside the parentheses (or by giving it only a body), the result
will match the use of `.{}` operator.

Using this operator allows the user to skip injection operation in defining `this` (or what matches it) by using `no_injection`
extension, as shown in the next example:

```
  class Type {
    def i: int;
  }
  def t: Type;
  t.{
    t.i = 1; // Updates t.i.
  };
  t~use_in(self) no_injection {
    self.i = 1; // Updates t.i.
    i = 1; // Error: Unknown symbol.
  };
```

## Dynamic Object Allocation And Initialization

User could initialize dynamically created objects using `~init` command, as shown in the next example:

```
  class Point {
    handler this~init() { ... };
    handler this~terminate() { ... };
    ...
  };

  def p: ref[Point];
  p~ptr = Memory.alloc(Point~size)~cast[ptr[Point]];
  p~init();
```

The difference between using `~init` command and calling a regular user-defined function customized for initialization
is that using `~init` ensures initialization of internal variables for this class. Which means that `~init` command ensures
initializing the whole variables tree whatever the depth is instead of initializing each variable manually.
For example, if `Point` class contains a variable from a class with custom initialization which also contains a variables
from classes with custom initialization, then `~init` command ensures the initialization of all those variables, and in
the required order.

As the case with object initialization, use could call termination function manually to terminate dynamically allocated
objects, as following:

```
  p~terminate();
```

## Alias

Aliases can be used to create a definition that is just an alias for another definition. This is similar to the
typedef command in C except that it's more powerful allowing you to create aliases for classes, functions, variavbles
as well as modules. In addition to writing aliases for the sake of simplfying access to otherwise lengthy definitions,
this can be used to write localizations for libraries, allowing the same library to have multiple interfaces in
different languages. Examples:

```
  def int: alias Int[32]; // 'int' is now equivalent to 'Int[32]'
```

## Modules

A module is a scope in which we can put definitions.
Modules can contain functions, variables, classes, constants, aliases, macros, or other modules.
Modules help avoiding conflicts resulting from similarity between definition names since definitions inside a module
is not visible in other modules unless referenced explicitly.
Modules are defined using the `module` command, as follows:

```
  def &lt;module_name&gt; : module { &lt;definitions&gt; };
```

It is also possible to use the following short form to define a module:

```
  module &lt;module_name&gt; { &lt;definitions&gt; };
```

As shown in the next example:

```
  module Outer {
    module Inner1 {
      module Inner2 {
        def v: Int;
      };
    };
  }
```

Accessing definitions inside another module is done using any of the following:


* The current module is inside the module that contain the requested definition:

  ```
    def Outer: module {
      def v: Int;

      def Inner: module {
        def printV: function {
          print(v);
        }
      }
    }
  ```

* Mention the full scope of the definition we want to access, starting from any shared scope, as in the next example:

  ```
    def Outer: module {
      def Inner1: module {
        def v: Int;
      };

      def Inner2: module {
        def printV: function {
          print(Inner1.v);
        }
      }
    }
  ```

* Using `use` command, which is used to make a given scope available inside the current scope:

  ```
    use &lt;full_path_of_targetted_module&gt;;
  ```

  This command is useful to avoid the need to repeat the full scope name.
  The next example explains how to use it:

  ```
    def Outer: module {
      def Inner1: module {
        def Inner2: module {
          def v: Int;
        };
      };

      use Inner1.Inner2;

      def printV: function {
        print(v);
      }
    }
  ```

## Macros

A macro is a set of commands that can be repeated easily anywhere in the program.
Macros can be defined as follows:

```
  def &lt;macro_name&gt; : macro [&lt;arg_list&gt;] &lt;macro_body&gt;
```

A macro's body could be one line or multiple lines enclosed in curly brackets.
A macro could be used later in the program by writing its name followed by the arguments enclosed in square brackets.
As shown in the next example:

```
  def power2: macro [n] n * n;
  .
  .
  s = power2[a];
```

It also possible to use the following short form:

```
  macro &lt;macro_name&gt; [&lt;arg_list&gt;] &lt;macro_body&gt;
```

As shown in the next example:

```
  macro power2 [n] n * n;
  .
  .
  s = power2[a];
```

### String Templates

It is possible to use templates with identifiers and strings inside the macro, which is done in identifiers by enclosing the
variable part of the identifier between two consecutive underscores in each side.
Whereas in strings we enclose the variable part inside double curly brackets, as shown in the next example:

```
  def print_vars: macro [v] {
    print("{{v}}1 = %d\n", __v__1);
    print("{{v}}2 = %d\n", __v__2);
  };
  .
  .
  s1 = 5;
  s2 = 6;
  print_vars[s];

  // will print:
  //  s1 = 5
  //  s2 = 6
```

### Member Macros

Member macros are macros defined inside a class and tagged as macros. A member macro is called using a variable
from the class, similar to methods (member functions). When calling a macro using a variable from that class
the variable (or the statement that returns an instance of that class) is passed to the macro as the first argument.

Member macros are defined in the same way as regular macros, but with the following differences:

* The macro is defined inside the body of a user-defined class.

* The macro is tagged with `@member` modifier.

* An argument is added to macro's arguments, which represents the object or statement that the macro is called in.


Example:

```
  class C1 {
    def i: Int;
    @member macro print_vars [self, v] {
      print("this.i + {{v}}1 = %d\n", self.i + __v__1);
      print("this.i + {{v}}2 = %d\n", self.i + __v__2);
    }
  }
  .
  .
  s1 = 5;
  s2 = 6;
  def c: C1;
  c.i = 2;
  c.print_vars[s];

  // will print:
  //  this.i + s1 = 7
  //  this.i + s2 = 8
  
  func getC(): C1 {
    return C1().{ i = 4 };
  }
  getC().print_vars[s];

  // will print:
  //  this.i + s1 = 9
  //  this.i + s2 = 10
```

### Comparison With C Macros

Alusus macros are different from their counterparts in C language in the way they are processed.
In C, macros are processed textually in a phase before parsing whereas in Alusus macros are processed in the parsing phase itself,
which leads to the following differences:


* In Alusus, a macro's effect is limited to its definition scope, in contrast to C macros which are not bound by any scope.
In other words, in Alusus if we define a macro inside a certain scope and try to use it in another scope, the compiler will
complain, unless if you reference the macro with its full qualifier.


* It is possible in Alusus to define multiple macros with the same name as long as they are defined in different scopes,
which is not possible in C.

* In Alusus, parsing errors inside the macro is detected immediately in contrast to C that does not detect parsing errors until it
finishes preprocessing and starts the parsing phase.


* In Alusus, it is not possible to define a macro that contains incomplete parts in its body, in contrast to C that allows that.
For example, in C we could define a macro that contains incomplete part from the grammar and the user will group multiple macros
to get a program that is correct, which is not possible in Alusus that prevents incomplete grammar in a macro's body.

* Alusus supports member macros, whereas C doesn't.

## Abstract Syntax Tree

Source code after parsing is converted into a tree of objects called Abstract Syntax Tree, which users can access inside their programs.
Access to this data is useful sometimes when interacting with the compiler, like compiling a function into executable code or similar things.
Also, it is useful in developing new properties through generating new code or read a code programmatically for any purposes.

It is possible to access Abstract Syntax Tree in two ways:

* Using **~ast** operator on any element from the source code to get the tree of that element.

  ```
    buildMgr.buildObjectFileForElement(myFunction~ast)
  ```


* Using **ast** command and give it a source code to get the tree of that code.

  ```
    astMgr.insertAst(ast { i = 0 }, ...)
  ```

  Which means that the difference between `ast` command and `~ast` operator is that the first enable us to write the source
  code directly into the command instead of writing it in a definition then using `~ast` operator on that definition.
  By default, the compiler performs preprocessing on `ast` command's argument before using it in other places in source code.
  For example, suppose that you are using `ast` command to write a source code inserted using `insertAst` function,
  and the source code to be inserted (ast argument) contains a `preprocess` command. By default the compiler will preprocess
  the the body of ast's argument itself, before executing the code that contains `insertAst` call.
  Sometimes we need to delay preprocessing so that it is done after inserting the AST, which means after finishing executing
  `insertAst` function. To do that we add `@no_preprocess` modifier to `ast` command, as shown in the next example:

  ```
    macro m [name] "__{{name}}__";
    astMgr.insertAst(ast m[test]); // inserts: "__test__"
    astMgr.insertAst(@no_preprocess ast m[test]); // inserts: m[test]
  ```



## Preprocess

Preprocessing allows the user to execute a code directly before compiling any function or class, and allows the user to create
and insert new code programmatically.
This feature is useful in generating source code based on a simpler or more clear syntax, i.e. developing the language and adding
new features to it.
The following shows the form of `preprocess` statement.

```
  preprocess &lt;preprocess_body&gt;
```

Inside preprocess body, users can write a code in Alusus itself without any restrictions, and they could
create and enter a new code using object `Spp.astMgr`.
Refer to Standard Runtime Libraries Reference for detailed information about the `Spp` module.
The following example explains how to create a group of assignment statements for variables queried dynamically.

```
  preprocess {
      def vars: Array[String] = findVarNames(...);
      def i: Int;
      for i = 0, i &lt; vars.getLength(), ++i {
          Spp.astMgr.insertAst(
              ast { this.name = 0 },
              Map[String, ref[Core.Basic.TiObject]]()
                  .set(String("name"), Core.Basic.TiStr(vars(i)))
          );
      }
  }
```

`preprocess` statements can be nested, which means a `preprocess` body can contain another `preprocess` statement.
In that case, the compiler execute the inner `preprocess` statement first which insert a code inside the body
of the outer `preprocess` statement. After finishing the inner statement, the outer statement is compiled then executed.

## Merge

Some definitions can be split into multiple parts that are merged later into one definition.
This feature allows the user to distribute large definitions like modules into multiple files, and it allows
appending additions to pre-defined definitions. Merge is done using `@merge` modifier, as shown in the next example:

```
  def Point: class {
    def x: Int;
  };
  .
  .
  @merge def Point: class {
    def y: Int;
  };
  .
  .
  def start: function {
    def p: Point;
    p.x = 10;
    p.y = 12;
    .
    .
  };
```

It is also possible to use this modifier to append an addition on an instance of a class template, but you need
to define an alias for that instance to be able to merge with it, as shown in the next example:

```
  class Point [T: type] {
    def x: T;
    def y: T;
  }

  def IntPoint: alias Point[Int];

  @merge class IntPoint {
    func print { ... } // This function will be added to Point[Int] only, not all template instances.
  }
```

## Import Libraries Or Source Code

`import` command is used to load a source code or a library and use it in the program. This command automatically distinguished
between a source code and a library. Currently, this command supports three types of files: source codes, general dynamic libraries,
and build libraries like libalusus_spp.
It is possible to load any dynamic library regardless of the language it was built from. When loading a dynamic library
all its public functions become available to use inside Alusus program, but you need to define those function manually
using the `function` command with `@expname` modifier, as shown in the next example:

```
  import "libmath.so";
  def sin: @expname[sin] function (f: Float) => Float;
  .
  .
  s = sin(r);
```

It is possible to discard the extension in `import` statement, the Core will add it automatically.
For example it is possible to shorten the following:

```
  import "Srl/String.alusus";
```

into the following:

```
  import "Srl/String";
```

It is also possible to discard the extension and a prefix in dynamic library names.
For example, the following:

```
  import "libmath.so";
```

can be shortened into:

```
  import "math";
```

In the previous case, the core will try to load the file "math.alusus", if not found, it will try to load "libmath.so" on Linux,
or "libmath.dylib" on mac.

In some cases, you may need to give multiple file names choices for the Core to choose from, and it will load the first one it can find.
Providing multiple choices is useful to support multiple operating systems with the same source code.
This is done using `or` inside `import` statement, as shown in the following example:

```
  import "libmath.so" or "libmath.so.0";
```

It is also possible to use `||` instead of `or` in the previous example.

## Process Module

When running Alusus programs in JIT mode (i.e. not pre-built), Alusus defines
`Process` module, which contains the following public variables:

* argCount: `Int`
  
  Number of passed arguments to Alusus compiler from the terminal.


* args: `ptr[array[ptr[array[Char]]]]`
  
  The listed of passed variables to Alusus compiler from the terminal.


* language: `ptr[array[Char]]`
  
  String to describe the system's current language. For example "ar" if system's language is Arabic.


* platform: `ptr[array[Char]]`
  
  String to describe current operations system. Its value is one of the following: linux, windows, macos.

