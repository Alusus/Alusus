# Core Module Reference 

`Core` module contains definitions for allowing interop between the Core and the user's program.
This module contains definitions for some classes and functions that the Core uses while processing source code files and
hence allows the programmer to deal directly with the Core and the data it creates. Not every element in the Core is exposed
in the Alusus-based definitions, instead, only definitions useful to end users are exposed.

## importFile Function

```
func importFile (filename: ptr[array[Char]]);
```

Include the file with the given name. This function do the work of `import` directive and does not differ from it
except that it is able to include files dynamically, by using the name of a file generated dynamically at run time.

## addLocalization Function

```
func addLocalization (
    locale: ptr[array[Char]],
    key: ptr[array[Char]],
    value: ptr[array[Char]]
);
```

Adds a translation to translations list for a specific language. If there is no previous entry for the required key
then the translation will be added even if the current language is different from given translation language.
The reason behind this is that having a text with another language is better than no text at all.
But if there is an entry for the given key then the translation will be added only if it matches the current
system language. 

This function is used to add text to custom errors notifications, and key's value will be equal to error's code.

## Type Info

In the module `Core.Basic` there exist definitions to allow objects to dynamically
provide information about their types, i.e. during runtime.
Most used classes in the Core and Spp contain type info.
The following are the definitions that provide type information:

### TiObjectFactory Class

Defined inside `Core.Basic` module 

This class is used to enable the creation of new objects dynamically from a certain class's type info,
which means that that type's info contains a reference to this factory.
Therefore, if you have a reference to a type with information, then using this information you could access
its factory and create new objects from the same type.

`TiObjectFactory` contains pointers to the following functions:

#### createPlain

```
  def createPlain: ptr[@shared @no_bind function ():ref[TiObject]];
```

Create an object and return a regular reference to it after initializing it.

#### createShared

```
  def createShared؛: ptr[@shared @no_bind function ():SrdRef[TiObject]];
```

Create an object and return a `SrdRef` to it after initializing it.

#### initialize

```
  def initialize: ptr[@shared @no_bind function (ref[TiObject])];
```

Intialize an object that already allocated in memory.

#### terminate

```
  def terminate: ptr[@shared @no_bind function (ref[TiObject])];
```

Terminate an object without releasing its memory.

### TypeInfo Class

Defined inside `Core.Basic`.

Holds type information for a specific class. This information includes type's name, the module
that it belongs to, objects factory, and more.

It contains the following variables:

* typeName:  (`String`)

  A string that holds type name without its full path.

* typeNamespace: (`String`)

  A string that holds class' full path without the name of the class. If you have `OuterModule.InnerModule.MyClass`
  class then this variable will contain the value `OuterModule.InnerModule`.

* packageName: (`String`)

  A string contains the name of the package that contains the class. For example, all known classes inside the core
  have `Core` as thier package name. Wheras classes deined inside Spp library have `Spp` as their package name.

* url: (`String`)

  Package url on internet. Alusus does not deal with the url directly, instead it deal with it as an information
  to the use to distinguish the class from others in case of similar names.

* uniqueName: (`String`)

  A string constructed from adding all previous information in one unique identifier.

* baseTypeInfo: (`ref[TypeInfo]`)

  A reference for base type info, in other words, it is a reference for type's info for the class that
  this class inherits from. Reference could be 0 in case of no base class.

* objectFactory: (`ref[TiObjectFactory]`)

  A reference to the factory that creates objects from this class.

### TiObject Class

Defined inside the module `Core.Basic`.

The root for all classes with type info. Provides the ability to access type's info, in addition to
accessing the interfaces that this class implements. Also, it provides the ability for dynamic casting.

This class contains the following items:

#### getMyTypeInfo

```
  handler this.getMyTypeInfo (): ref[TypeInfo];
```

Gives a reference to this type info.

#### isDerivedFrom

```
  handler this.isDerivedFrom (ref[TypeInfo]): Bool;
```

Tell you whether this object is from the class with the given info, or from a class derived from that class.

#### getInterface

```
  handler this.getInterface (ref[TypeInfo]): ref[TiInterface];
```

Receives a reference for interface info and retruns a reference to that interface if exists, else it return 0.

### TiInterface Class

Defined inside module `Core.Basic`.

The root for all interfaces classes. It provides the ability to access the interface's type info, in addition to accessing the object
that owns the interface. 

This class contains the following items:

#### getMyInterfaceInfo

```
  handler this.getMyInterfaceInfo (): ref[TypeInfo];
```

Gives a reference to this inference info.

#### getTiObject

```
  handler this.getTiObject (): ref[TiObject];
```

Returns the object that owns this inference.

### getInterface Macro

Defined inside module `Core.Basic`.

A macro to ease getting an interface from a specific object. It is used in the following way:

```
  interface~no_deref = getInterface[obj, InterfaceType];
```

### isDerivedFrom Macro

Defined inside module `Core.Basic`.

Check whether the given object is derived from the given class. It is used in the following way:

```
  if isDerivedFrom[obj, ObjType] { ... }
```

### defDynCastedRef Macro

Defined inside module `Core.Basic`.

A macro used to shorten the operation of dynamically casting the class and storing the result in a new variable.
For example:

```
  func test (parentObj: ref[ParentType]) {
    defDynCastedRef[childObj, parentObj, ChildType];
    if childObj~ptr != 0 childObj.someFunc(...);
  }
```

## Basic Data Types with Type Info

Defined inside module `Core.Basic`.

Many classes are available that correspond to the basic types like integers and strings, but are derived from
`TiObject Class` to provide runtime type info.
These classes define equality and initialization operations in addition to the following items:

#### value

The variable containing the original value in its original class.

#### create

```
  func create (v: BasicType): SrdRef[ObjType]
```

Creates an object from this class with the given value and return a `SrdRef` to that object.

#### getTypeInfo

```
  func getTypeInfo (): ref[TypeInfo];
```

This function returns a reference to the type's info, which means to the info of objects derived from this type.
The following example shows how to define and use these classes:

```
  def i: TiInt(45);
  printInt(i.value);
  i = 7;
  
  def o: ref[TiObject](i);
  if isDerivedFrom[o, TiInt] {
    def x: ref[TiInt](castRef[o, TiInt]);
    printInt(x.value);
  }
```

What follows show a list of the basic classes with an info.
* `TiInt`
* `TiWord`
* `TiFloat`
* `TiBool`
* `TiStr`
* `TiPtr`

## Dynamic Data Access

The types used in Abstract Syntax Tree support a group of interfaces to allow traversing the tree dynamically.
These interfaces allow querying the object's properties and accessing to those values to read or modify it.

### Binding Interface

Defined inside module `Core.Basic`.

This interface allows the user to access the members of the object, and contains the following functions:

#### setMember

```
  handler this.setMember (memberName: ptr[array[Char]], value: ref[TiObject]);
  handler this.setMember (memberIndex: Int, value: ref[TiObject]);
```

Set the value of an object's member, the first to specify the member based on its name, wheras the second
is based on the order of the member amongs other members.

#### getMemberCount

```
  handler this.getMemberCount(): Word;
```

Querying number of members for this object.

#### getMember

```
  handler this.getMember (memberName: ptr[array[Char]]): ref[TiObject];
  handler this.getMember (memberIndex: Int): ref[TiObject];
```

Get the value of the member, with the given name in the first form, or the index in the second form.

#### getMemberNeededType

```
  handler this.getMemberNeededType (memberName: ptr[array[Char]]): ref[TypeInfo];
  handler this.getMemberNeededType (memberIndex: Int): ref[TypeInfo];
```

Get the type info for the specified member with given name in the first form, or the index in the second form.

#### getMemberKey

```
  handler this.getMemberKey (index: Int): String;
```

Get the name of the memnber with the given index.

#### findMemberIndex

```
  handler this.findMemberIndex (name: ptr[array[Char]]): Int;
```

Returns the index of the memnber with the given name, or -1 if not found.

### Containing Interface

Defined inside module `Core.Basic`.

This interface is used to query other objects that this object contains, which means the other nodes that
this class contains in the tree. 

The difference between this interface and `Binding` interface is that the later is used to query the properties of the object itself
which are considered a description of that node, while the first is used to to query other nodes separate from this node but associated to it.
For example, if you have an object that represents binary operator between two values, then the operator type is considered to be a property
and is queried using `Binding` interface, while the other two values are queried using `Containing` interface. 

This interface contains the following functions:

#### setElement

```
  handler this.setElement (index: Int, value: ref[TiObject]);
```

Set a new object to the element with the given index.

#### getElementCount

```
  handler this.getElementCount (): Word;
```

Get the number of elements that this object contains.

#### getElement

```
  handler this.getElement (index: Int): ref[TiObject];
```

Get the element with the given index.

#### getElementNeededType

```
  handler this.getElementNeededType (index: Int): ref[TypeInfo];
```

Get the info of the required type for the element with the given index.

### DynamicContaining Interface

Defined inside module `Core.Basic`.

It is derived from `Containing` and adds the ability to dynamically add and remove elements
to the object. This interface is used with containers that could change their size, and contains the following
functions in addition to the functions that are available in `Containing` interface:

#### addElement

```
  handler this.addElement (value: ref[TiObject]): Int;
```


Adds new element to the end of the container, and returns the index of the new element.

#### insertElement

```
  handler this.insertElement (index: Int, value: ref[TiObject]);
```


Adds new element to the container at the given index, after shifting all elements that are at that index or after it.

#### removeElement

```
  handler this.removeElement (index: Int);
```

Removes the element at the given index, and shifts all elements that are after that index.

#### getElementsNeededType

```
  handler this.getElementsNeededType (): ref[TypeInfo];
```

Get the info of the required type for the container's elements without the need to set any specific element.

### MapContaining Interface

Defined inside module `Core.Basic`.

It is derived from `Containing` and adds the ability to access elements by their names
instead if their index. This interface contains the following functions:

#### setElement

```
  handler this.setElement (
    elementName: ptr[array[Char]], value: ref[TiObject]
  ): Int;
```

Set a new value for the element with the given name and returns the index of that element.

#### getElement

```
  handler this.getElement (elementName: ptr[array[Char]]): ref[TiObject];
```

Get the value of the element with the given name.

#### getElementNeededType

```
  handler this.getElementNeededType (
    elementName: ptr[array[Char]]
  ): ref[TypeInfo];
```

Get the info for the required type of the element with the given name.

#### getElementKey

```
  handler this.getElementKey (index: Int): String;
```

Get the name of the element with the given index.

#### findElementIndex

```
  handler this.findElementIndex (name: ptr[array[Char]]): Int;
```

Get the index of the element with the given name, or -1 if not found.

### DynamicMapContaining Interface

Defined inside module `Core.Basic`.

It is derived from `MapContaining` and adds the ability to dynamically remove or add new elements.
This interface contains the following functions:

#### addElement

```
  handler this.addElement (
    name: ptr[array[Char]], value: ref[TiObject]
  ): Int;
```

Adds ew element with the given name to the container and returns the index of that new element.

#### insertElement

```
  handler this.insertElement (
    index: Int, name: ptr[array[Char]], value: ref[TiObject]
  );
```

Adds new element at the given index after shifting all elements that are at that index or after it.

#### removeElement

```
  handler this.removeElement (index: Int);
  handler this.removeElement (name: ptr[array[Char]]);
```

Removes the element with the given index (the first form), or the given name (the second form).

#### getElementsNeededType

```
  handler this.getElementsNeededType (): ref[TypeInfo];
```

Get the info of the required type for the container's elements without the need to specify a specific element.

## AST

All classes that make AST are derived from class Node
and support interfaces for dynamic data access, so it is possible at run time to traverse the AST dynamically
that allows the user to read and create ASTs from inside their program which allows them to develop new grammatical structures
to ease their work.

### Node Class

Defined inside module `Core.Data`.

It is derived from `TiObject`.
This is a root for other data classes like AST's classes. This class allows an object to access its owner.

#### owner: ref[Node]

A reference to the owner of this object.

#### getTypeInfo

```
  func getTypeInfo (): ref[TypeInfo];
```

This function returns a reference to this type info, which means the info of objects derived from this type.

### Text Types

Defined inside module `Core.Data.Ast`.

It is derived from Node.
These classes contain a single text section from the source code, without operators or expressions.
The text section as one symbol so it could be an identifier, a number, or a lexer (a sequence of characters that compiler interprets).

These classes contain the following items:

#### value: TiStr

The string read from the source code.

#### getTypeInfo

```
  func getTypeInfo (): ref[TypeInfo];
```

This function returns a reference to this type, which means the type of objects derived from this type.

#### create

```
  func create (value: ptr[array[Char]]): SrdRef[TheType];
```

Creates an object from this class with the given value, and returns `SrdRef` to this object.

A list of text types:

* Identifier
  Any identifier from the source code, like a variable name, a function name, or anyting similar to that.

* StringLiteral

  Contains a string, which means a sequence of characters enclosed in double quotes. The value stored in this object
  does not include the double quote character, and does not include the whole composite character (like \) instead it only
  include the character itself. For example, when the user write \" then the value for this object will contain only " without
  \.

  In case the user splits their string into multiple parts (by closing the double quote and reopening it) then the value
  stored in that object will be a string containing all parts without spaces between them.
  For example, if the user enter `"Hello " "World"` then the
  value will be `Hello World`

* CharLiteral

  Cotains a character. Which means the character enclosed in the source code between single quotes, but the value in this
  object will be without the quotes.

* IntegerLiteral
  Contains an integer with the symbols related to Int class. For example, "456i32".

* FloatLiteral
  Contains an integer with the symbols related to Float class. For example, "1.2f32".

### Commands And Expressions Types

Defined inside module `Core.Data.Ast`.

It is derived from Node.
The following is a list of types that represent expressions and commands. These classes do not have public functions
other than `getTypeInfo`. To access their members the user can use the dynamic member
access interfaces mentioned above.

* Alias

* Bracket

* Bridge

  This class represent `use` expression that creates a bridge from a namespace which allows it to automatically access
  another namespace items.

* Definition

* GenericCommand

  A generic class used for holding unspecified command's information. It contains a string for the keyword and a dynamic list of data.

* List

* Map

* MergeList

  A private class that tells the parser to merge its elements directly into the upper namespace instead of letting it be a sub-list inside
  the upper namespace.

* ParamPass

  Represents the operation of passing data to an element, like calling a function, a macro, or a template.

* Scope

* Token

* PrefixOperator

* PostfixOperator

* AssignmentOperator

* ComparisonOperator

* AdditionOperator

  For addition and subtraction operators.

* MultiplicationOperator

  For multiplication and division operators.

* BitwiseOperator

* LogOperator

* LinkOperator

  For the operators that link between an element and a member from it like dot operator, `->` operator, or anything similar to that.

* ConditionalOperator

* Passage

  This type is used to refer to an AST element using its pointer instead of referring
  to that element using an identifier (the name of the element in the source code).
  This makes generating code dynamically easier in some cases.
