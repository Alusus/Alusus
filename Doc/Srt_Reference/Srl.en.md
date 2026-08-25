# Srl Library Reference 

Standard Runtime Library consists of one module named `Srl` which contains the following modules and classes:

## Array Class

A dynamic array template that automatically manages the array buffer, taking performance into account and avoiding
unnecessary memory allocation and copying operations.
The following example shows usage of this dynamic array.

```
func main {
  def a: Array[Int] = getArray();
  a.add(1); // Here the allocated memory block is expanded.
  def a2: Array[Int] = a; // No new memory block is allocated here.
  a.add(2); // Now a new copy of the buffer is created so a2 is not affected.
  printArray(a); // No new memory block is created or copied here.
  // Now the memory blocks of a and a2 are freed.
}

func getArray(): Array[Int] {
  def a: Array[Int];
  a.add(0);
  return a; // No new memory block is allocated here and memory copy happens.
}

func printArray (a: Array[Int]) {
  def i: Int;
  for i = 0, i < a.getLength(), ++i Console.print("%d\n", a(i));
}
```

`Array` class contains the following members:

#### Initialization

```
handler this~init(ref[Array[ContentType]]);
handler this~init(count: Int, args: ...ContentType);
```

The first form initializes an array from another. The new array will use the same content of the given array, and no
copy will happen until one of them changes the content. In that case, the content is copied before
the change occurs to ensure that the other array is not affected.
The second form initializes the array from the given items, as shown in the example below:

```
def a1: Array[Int]({ 5, 2, 1 }); // Array will contain 3 elements: 5, 2, and 1.
def a2: Array[Int](3, 5, 2, 1); // Array will contain 3 elements: 5, 2, and 1.
```

#### getLength

```
handler this.getLength (): ArchInt;
```

Returns the number of items in the array.

#### setLength

```
handler this.setLength (newLength: ArchInt);
```

Sets the number of items in the array. Extends the allocated memory if necessary.
If the content is shared with another array, this function copies the content to a new buffer.

#### getBufSize

```
handler this.getBufSize (): ArchInt;
```

Returns the number of items that the current allocated memory could store. When exceeding that size the object will extend the memory capacity
automatically.

#### assign

```
handler this.assign (a: ref[Array[ContentType]]);
```

Assigns new content to the array from another array. This function does not copy the contents of the other array, instead both arrays share
the same buffer until one of the them needs to change its content, in which case that array will copy the buffer and end the sharing.

#### add

```
handler this.add (e: ContentType);
handler this.add (argCount: Int, args: ...ContentType);
handler this.add (e: Array[ContentType]);
```

The first form adds an item to the array after extending the allocated memory if necessary.
If the content is shared with another array, this function copies the content to a new buffer.

The second form adds many items at once.

The third form adds an array to the current array. It adds all items of the given array to the
current array. The given array will not be affected by this call.

```
a.add({ 5, 2, 1 }); // Adds 3 elements: 5, 2, 1.
a.add(3, 5, 2, 1); // Adds 3 elements: 5, 2, 1.
```

#### insert

```
handler this.insert (index: ArchInt, element: ContentType);
```

Adds new item to the array in the specified location after extending the allocated memory if necessary.
If the content is shared with another array, this function copies the content to a new buffer.

#### remove

```
handler this.remove (index: ArchInt);
```

Removes the item at the specified index. If the content is shared with another array, this function copies the content to a new buffer.

#### slice

```
handler this.slice (start: ArchInt, count: ArchInt);
```

Copys part of the array and returns it as a new array. The copy starts from the item at index `start` and continues until `count` items
are copied or the end of the array is reached.

#### clear

```
handler this.clear ();
```

Removes all items from the array. If the content is not shared with another array the buffer will be released.

## String Class

`String` class simplifies dealing with strings. It is responsible for allocating and releasing the memory
allocated for the string, taking performance into account and avoiding unnecessary memory allocation and
copy operations.
This class provides functions to simplify different operations on strings. The following example
shows this class in use.

```
func main {
  def str: String = getString();
  str += " world"; // Memory block is expanded here.
  def str2: String = str; // No new memory is allocated here.
  str += "."; // Copy of the string buffer is created so str2 is not affected.
  printStr(str); // No new memory allocation or copy happens here.
  // Now memory blocks of str and str2 are freed.
}

func getString (): String {
  def s: String = "Hello";
  return s; // No new memory allocation or copy happens here.
}

func printStr (s: String) {
  Console.print(s); // s is automatically casted into ptr[array[Char]].
}
```

`String` class contains the following members:

#### buf

```
def buf: ptr[array[Char]];
```

A pointer to this string content.

#### parentheses operators

```
handler this(i: ArchInt): Char
```

This operator could be used to retrieve the character at the specified location.

#### comparison operators

```
handler this == ptr[array[Char]]: Bool
handler this > ptr[array[Char]]: Bool
handler this < ptr[array[Char]]: Bool
handler this >= ptr[array[Char]]: Bool
handler this <= ptr[array[Char]]: Bool
```

Compares this string to the given string.

#### getLength

```
1: handler this.getLength (): ArchInt;
2: func getLength (p: ptr[array[Char]]): ArchInt;
```

1. Returns this string's length.
2. Returns the length of the given string.

#### alloc

```
handler this.alloc (ArchInt);
```

Allocate memory in advance. This function allows the user to allocate memory in advance for use with string operations that deals with the string
buffer directly. This function is useful for dealing with libraries that deal with char pointers, while benefiting from the memory management
functionality that this class provides.

#### realloc

```
handler this.realloc (ArchInt);
```

Changes the size of allocated memory for this string. This function enables the user to change the buffer size while doing string operations
directly on the buffer.

#### assign

```
1: handler this.assign (str: ref[String]);
2: handler this.assign (buf: ptr[array[Char]]);
3: handler this.assign (buf: ptr[array[Char]], count: ArchInt);
4: func assign (target: ptr[array[Char]], fmt: ptr[array[Char]], ...any): Int;
```

1. Assigns new content to ths string from another string. This function does not copy the content of the other string, instead they both share the same buffer
until one of them needs to change it, at which point the buffer is copied and the sharing ends.
2. Assigns new content for the string from a buffer in the memory. This function copies the content from that buffer into a new buffer managed by this string.
3. This is similar to the second variant but it copies only the specified number of characters from the given buffer.
4. Assigns new value from a `format` and unspecified number of arguments to the target. User should ensure that there is enough memory in the target
to hold the whole result.
User could also use `=` operator to replace functions 1 and 2.

#### append

```
1. handler this.append (buf: ptr[array[Char]]);
2. handler this.append (buf: ptr[array[Char]], count: ArchInt);
3. handler this.append (c: Char);
4. handler this.append (i: Int[64]);
5. handler this.append (f: Float[64]);
```

1. Appends the given buffer to the content's end of the string.
2. Appends the specified number of of characters from the given buffer.
3. Appends a character to the string's end.
4. Appends the given number to the end of the string. This function appends a string representation of the given number.
5. This is similar to the 4th variant but it appends a float number instead of an integer.
It is possible to replace these functions (except for the second function) by `+=` operator.

#### concat

```
1. handler this.concat (buf: ptr[array[Char]]);
2. handler this.concat (buf: ptr[array[Char]], count: ArchInt);
3. handler this.concat (c: Char);
4. handler this.concat (i: Int[64]);
5. handler this.concat (f: Float[64]);
6. func concat (target: ptr[array[Char]], source: ptr[array[Char]]): ptr;
7. func concat (target: ptr[array[Char]], source: ptr[array[Char]], count: ArchInt): ptr;
```

1-5. These functions are similar to `append` functions but they return the value in new string instead of editing the current string.
It is possible to replace functions 1, 3, 4, and 5 with the `+` operator.
6. Append the source string to the end of target string. User should ensure that the target buffer is enough to hold the new items.
7. This is similar to variant 6 but it only copies the specified number from the source string.

#### find

```
1. handler this.find (buf: ptr[array[Char]]): ArchInt;
2. handler this.find (c: Char): ArchInt;
3. func find (haystack: ptr[Char], needle: ptr[Char]): ptr[Char];
4. func find (haystack: ptr[Char], c: Char): ptr[Char];
```

1. Searches for a string inside this string. It returns the start location of the found string, or -1 if nothing is found.
2. Searches for a character inside this string. It returns the location of the found character, or -1 if nothing is found.
3. Searches for a string inside the given string. It returns a pointer to the beginning of the found string, or 0 if nothing is found.
4. Searches for a character inside the given string. It returns a pointer to the found character, or 0 if nothing is found.

#### findLast

```
1: handler this.findLast (buf: ptr[array[Char]]): ArchInt;
2: handler this.findLast (c: Char): ArchInt;
3: func findLast (haystack: ptr[Char], needle: ptr[Char]): ptr[Char];
4: func findLast (haystack: ptr[Char], c: Char): ptr[Char];
```

These functions are similar to `find` functions, but they start the search from the end of the string instead of the beginning.

#### compare

```
1. handler this.compare (buf: ptr[array[Char]]): Int;
2. handler this.compare (buf: ptr[array[Char]], count: ArchInt): Int;
3. func compare (str1: ptr[Char], str2: ptr[Char]): Int;
4. func compare (str1: ptr[Char], str2: ptr[Char], count: ArchInt): Int;
```

1. Compares the current string with the given string and returns 1 if the current one is bigger, -1 if it is smaller, or 0 if they are the same.
2. This is similar to the first function but it compares only the specified number of the given buffer's items.
3. This is similar to the first function bit it compares the two given arrays of chars.
4. This is similar to function 3 but it compares only a specified number of characters from the second array.

#### replace

```
1. handler this.replace (match: ptr[array[Char]], replacement: ptr[array[Char]]): String;
2. func replace (chars: ptr[array[Char]], from: Char, to: Char): ptr[array[Char]];
```

1. Replaces a part of the string with another string, and returns the result in a new string.
2. Replaces all occurances of a character with another character in the given array of chars. This function modifies the given array of chars.

#### trim

```
handler this.trim (): String;
```

Removes the spaces (space, new line, or tab characters) from both sides of the string and returns the result in a new string.

#### trimStart

```
handler this.trimStart (): String;
```

This is similar to `trim` function but it trims from the beginning only.

#### trimEnd

```
handler this.trimEnd (): String;
```

This is similar to `trim` function but it trims from the end only.

#### toUpperCase

```
handler this.toUpperCase (): String;
```

Replaces the latin small characters with their corresponding capital characters and returns the result in a new string.

#### toLowerCase

```
handler this.toLowerCase (): String;
```

Replaces the latin capital characters with their corresponding small characters and returns the result in a new string.

#### slice

```
handler this.slice (start: ArchInt, count: ArchInt): String;
```

Returns a part of the current string in a new string.

#### split

```
handler this.split (separator: ptr[array[Char]]): Array[String];
```

Splits the current string into an array of strings using the given separator. The separator is not included in the result.
If that separator is not found, it returns and array of one element which is the whole string.

#### merge

```
func merge (parts: Array[String], separator: ptr[array[Char]]): String;
```

Returns a new string which contains the given parts separated with the given separator.

#### copy

```
1. func copy (target: ptr[array[Char]], source: ptr[array[Char]]): ptr;
2. func copy (target: ptr[array[Char]], source: ptr[array[Char]], count: ArchInt): ptr;
```

1. Copies the array of chars from the source to the target. User should ensure that the memory allocated for target is enough to hold the source content.
2. This is similar to the first function except that it copies only the specified number of items from the source.

#### scan

```
func scan (source: ptr[array[Char]], fmt: ptr[array[Char]], ...any): Int;
```

Scans the given string searching for the given items in the format and returns those items in the arguments after the format.
This function is similar to `sscanf` system function.

#### isSpace

```
func isSpace (c: Char): Bool;
```

Tells whether the given character is a space, a tab, a new line, or a carriage return.

#### isEqual

```
func isEqual (str1: ptr[array[Char]], str2: ptr[array[Char]]): Bool;
```

Tells whether the two given strings are the same.

#### remove

```
func remove (chars: ptr[array[Char]], c: Char): ptr[array[Char]];
```

Removes every match of the given character from the given array of chars. Removing that character results in shifting following characters to replace it.
This function modifies the provided string.

#### format

```
func format (fmt: ptr[array[Char]], values: ...any): String;
```

Create a new string from given format after filling it with the given arguments. This function is similar to `sprintf` function, by replacing symbols starting
with `%` in the format by a value from arguments that match the type specified. The symbol `%` is followed by a character specifying the type of the given
argument. As follows:

* **%s** Array of chars.
* **%c** A single char.
* **%i** 32 bits integer
* **%l** 64 bits integer
* **%f** 32 bits float
* **%d** 64 bits float
* **%%** Prints the `%` symbol

#### parseInt

```
func parseInt (str: ptr[array[Char]]): Int[64];
```

Reads an integer from the given array of chars and returns it.

#### parseFloat

```
func parseFloat (str: ptr[array[Char]]): Float[64];
```

Reads a float from the given array of chars and returns it.

#### parseHexDigit

```
func parseHexDigit (Char): Int;
```

Parses the given HEX character.

## StringBuilder Class

The `StringBuilder` type is used to build strings while reducing the number of memory allocations resulting
from multiple string concatenations. It is highly recommended to use this class instead of the plain
`String` class when constructing strings through high numbers of concatentations.

```
class StringBuilder [formatMixin: ast_ref = _emptyMixin] {
  handler this~init();
  handler this~init(initialSize: ArchInt, growSize: ArchInt);
  handler this~init(str: String, growSize: ArchInt);
  handler this.append (buf: CharsPtr);
  handler this.append (buf: CharsPtr, bufLen: ArchInt);
  handler this.append (c: Char);
  handler this.append (i: Int[64]);
  handler this.append (f: Float[64]);
  handler this.format(fmt: ptr[array[Char]], args: ...any);
  handler this.clear();
  handler this.getLength();
  handler this += CharsPtr this.append(value);
  handler this += Char this.append(value);
  handler this += Int[64] this.append(value);
  handler this += Float[64] this.append(value);
  handler this~cast[ref[String]];
}
```

The following example shows how to use this class:

```
func main {
  def sb: StringBuilder(
    1024, // preallocate 1024 bytes
    512   // enlarge memory block by 512 whenever it's full
  );
  sb.append("Result of ");
  sb.format("7 + 7 = %i\n", 7 + 7);
  def s: String = sb;
  Console.print(s); // Prints: Result of 7 + 7 = 14
}
```

#### Initialization

```
handler this~init();
handler this~init(initialSize: ArchInt, growSize: ArchInt);
handler this~init(str: String, growSize: ArchInt);
```

The second form of the constructor preallocates the buffer with the size specified in the first arg, while
the second argument specifies the size by which to enlarge the buffer everytime it's full.

The third form is similar to the second except that it starts by sharing the buffer with the given string
until a modification is made, at which point it duplicates the buffer, if necessary, and expands it by
the size given in the third argument.

#### apppend
Appends a value to the string. This is simliar to the `String.append` method.

#### format
Similar to the `String.format`, with the given differences:
It appends the generated string to the buffer instead of creating a new string.
It understands the same set of argument types, but the set can also be extended by
end users through the use of mixins, as shown later in this section.

#### clear
Empties the buffer. This only replaces the content of the buffer with an empty string;
it doesn't free the buffer.

#### getLength
Returns the length of the string in the buffer. This is not the size of the buffer itself;
it's the size of the string contained in it, which is usually smaller than the buffer.

### Extending the Types of `format`
To expand the set of types understood by the `format` method the user can provide the
`StringBuilder` class with a mixin containing the set of functions to be called by `format`
in response to encountering the related symbol in the format string. In order for the functions
within the mixin to be called by `format` they need to receive a single argument of a type matching
the requested type, and be flaged with the `@format` modifier, giving that modifier the requested
formatting symbol as a param.

The same data type can be used in multiple formatting as long as the formatting symbol is different.
For example, the type `Int[64]` can be used in two functions, one having the symbol `%gd` to treat
the given integer as a timestamp and print it as a gregorian date, while the other having the
symbol `%hd` to also treat the given integer as a timestamp but print it as a hijri date.

The following example shows extending the StringBuilder with a formatting for convertign
timestamps to date strings:

```
def StringBuilderMixin {
  @format["gd"]
  handler this.formatTimestamp(ts: Int[64]) {
    this.append(Time.toString(ts));
  }
}
func main {
  def sb: StringBuilder[StringBuilderMixin](1024, 512);
  def timestamp: Int[64] = ...;
  sb.format("Today's date is %gd", timestamp);
  ...
}
```

## Map Class

A map template that allows specifying the types of the key and the value, and takes the responsibility for managing memory while taking
performance into consideration and avoiding unnecessary memory allocation and copying. The following example shows the class in use.

```
func main {
  def m1: Map[Int, Int] = getMap();
  m1.set(7, 50); // Memory buffer is expanded here.
  def m2: Map[Int, Int] = m1; // Buffer is not cloned here.
  m2.set(12, 7); // Buffer is cloned here so that m1 is not affected.
  printMap(m2); // No buffer cloning happens here.
  printInt(m2(7)); // Prints 50.
  printInt(m2(12)); // Prints 7.
  // Now buffers of m1 and m2 are freed.
}

func getMap (): Map[Int, Int] {
  def m: Map[Int, Int];
  m.add(0, 10);
  return m; // No buffer cloning happens here.
}

func printMap (m: Map[Int, Int]) {
  def i: Int;
  for i = 0, i < m.getLength(), ++i {
    Console.print("%d = %d\n", m.keyAt(i), m.valAt(i));
  }
}
```

Class `Map` contains the following members:

#### Initialization

```
  handler this~init (useIndex: Bool);
  handler this~init (ref[Map[KeyType, ValueType]]);
  handler this~init (ref[Map[KeyType, ValueType]], useIndex: Bool);
```

The first form initializes empty Map and choosing between using an index or not. The use of index increases the speed of searching for items in the Map
at the expense of consuming more memory and slowing down write operations.
The second form initialize the Map from another Map. The new Map will use the same content as the given Map, and no content copy will occurs until
one of the two Maps changes its content at which point the content is copied to ensure the other Map is not affected.
In this form index won't be used even if the given Map uses one.
The third form is similar to the second one, but allows the user to use an index. If the user asked for using an index then it will be created even
if the given Map does not have one.

#### keys

```
  def keys: Array[KeyType];
```

The array of the keys in the map.

#### values

```
  def values: Array[ValueType];
```

The array of the values in the map.

#### getLength

```
  handler this.getLength (): ArchInt;
```

Returns the number of items in the map.

#### keyAt

```
  handler this.keyAt (ArchInt): ref[KeyType];
```

Returns a reference to the key at the given location.

#### valAt

```
  handler this.valAt (ArchInt): ref[ValueType];
```

Returns a reference to the value at the given location.

#### set

```
  handler this.set (
    key: KeyType, value: ValueType
  ): ref[Map[KeyType, ValueType]];
```

Sets a value for the given key. If that key doesn't exist it will be added. If it already exists its value will be replaced by the new one.
This functions will return a reference to the Map itself which allows the user to chain multiple `set` operations in one statement.

#### setAt

```
  handler this.setAt (
    index: ArchInt, value: ValueType
  ): ref[Map[KeyType, ValueType]];
```

Sets a new value on the given index, which means changing the value based on its index instead of its key.
Returns a reference to the Map itself which allows the user to chain multiple `setAt` operations in a one statement.

#### insert

```
  handler this.insert (index: ArchInt, key: KeyType, value: ValueType);
```

Inserts a new key and its value at the given index.

#### remove

```
  handler this.remove (key: KeyType): Bool;
```

Removes the specified key and its value. Returns 1 if the key is removed, and 0 if the key does not exist.

#### removeAt

```
  handler this.removeAt (index: ArchInt);
```

Removes the specified key and its value at the given index.

#### clear

```
  handler this.clear ();
```

Clear all content and starts with an empty Map.

#### findPos

```
  handler this.findPos (key: KeyType): ArchInt;
```

Returns the index of the given key, or -1 if the key does not exist.

## SrdRef Class

Shared reference template that manages releasing the object automatically when the need for it ends. This reference keeps a count of the number
of shared references that point to the same object. Every time one of the reference is terminated, the count decremented by 1. When the count
reaches zero, the object is terminated and its memory is released.

```
func main {
  def x: SrdRef[MyType] = SrdRef[MyType].construct();
  def y: SrdRef[MyType] = x; // Both refs point to same object. Counter is now 2.
  x.release(); // Counter is now 1 and the object is not freed yet.
  y.release(); // Now the object is destructed and its memory is released.
}
```

`SrdRef` Class contains the following members:

#### obj
The object pointed to by this reference. In some cases, one could need to access the object itself, like when a regular pointer of reference to
an object is needed, or in case of accessing one of object's methods with the same name as one of `SrdRef` methods.

#### refCounter
The record that holds the number of references that point to the same object and the necessary information to terminate and release the object.

#### alloc

```
  handler this.alloc (): ref[ObjType];
  func alloc (): SrdRef[ObjType];
```

This function allocates memory for the object, but does not initialize the object and leaves that to the user who can use `~init`.

#### construct

```
  handler this.construct ();
  func construct (): SrdRef[ObjType];
```

This function allocates memory for the object and initialize it also. Initializing the object is done without any arguments, so to use this
function the object must enable initialization without arguments. If you need to initialize an object with arguments then you should use `allocate`
then `~init`.

#### own

```
  handler this.own (obj: ref[ObType]);
  func own (obj: ref[ObjType]): SrdRef[ObjType];
```

This function assigns the object of this reference with a given object. After calling this function the reference takes the responsibility of
releasing the object automatically. This function should be avoided if the given object is not allocated dynamically or if another code is
responsible for releasing the object, since this will lead to segmentation fault.

#### release

```
  handler this.release();
```

Release this reference, and if this reference is the last to point to the object then it will release the object before that.

#### assign

```
  handler this.assign (r: ref[SrdRef[ObjType]]);
  handler this.assign (r: ref[WkRef[ObjType]]);
  handler this.assign (c: ref[RefCounter], o: ref[ObjType]);
```

Set a new value to the reference, it changes the reference to point to the given object. The first two functions takes the value of the reference
from types `SrdRef` and `WkRef` respectively with the same object type. On the other hand, the third function is used to make the reference points to
an object with the same type of this reference, but belongs to a reference with another type. This function is used in casting while ensuring
that releasing the object when terminating the reference will leads to releasing the original object with its original type that is created with.
The next example explains the difference:

```
  def x: SrdRef[MyType];
  x.construct();
  def y: SrdRef[SubType];
  y.assign(x.refCounter, x.subObj);
  
  x.release(); // Nothing will be released as y still holds the object.
  y.release(); // Here x itself will be released, not x.subObj.
```

#### isNull

```
  handler this.isNull(): Bool;
```

Returns true if the reference is null.

## WkRef

Reference template that points to a shared object but it does not participate in owning the object, which means it does not update the reference
count of the object. If all `SrdRef` instances pointing to a specific object are terminated then that object
will be released even if some `WkRef` instances are still pointing to it. This type of references is useful to avoid circular references which lead to memory leaks.
So if you have an object that owns another object through `SrdRef` reference, and the inner object needs a reference to the outer object, then using
`SrdRef` in the inner object will lead to a circular reference, whereas using `WkRef` in the inner object won't lead to that because releasing the outer object
in this case will cause both objects to be released.
The difference between using `WkRef` and using `ref`, is that the first holds references count's information even if it does not use it, whereas the later
does not hold this information, so it is possible to get `SrdRef` from `WkRef` but we could not get `SrdRef` from `ref` because the later does not
holds the count information that `SrdRef` needs.
The way to use `WkRef` is the same as `SrdRef`. `WkRef` class has the following items:

#### obj
The object that this reference points to. In some cases you will need to access the object itself like when you need a pointer or a regular reference
to the object, or in case you want to access one of object's methods that has the same name as one of `WkRef` methods.

#### refCounter
The record that holds the number of references that point to the same object and the necessary information to terminate and release the object.

#### release

```
  handler this.release();
```

Release this reference without changing references count.

#### assign

```
  handler this.assign (r: ref[SrdRef[ObjType]]);
  handler this.assign (r: ref[WkRef[ObjType]]);
  handler this.assign (c: ref[RefCounter], o: ref[ObjType]);
```

Sets a new value to the reference, it changes the reference to point to the given object. The first two functions takes the value of the reference
from types `SrdRef` and `WkRef` respectively with the same object type. On the other hand, the third function is used to make the reference points to
an object with the same type of this reference, but belongs to a reference with another type. This function is used in casting while ensuring
that releasing the object when terminating the reference will leads to releasing the original object with its original type that is created with.
The next example explains the difference:

```
  def x: SrdRef[MyType];
  x.construct();
  def y: WkRef[SubType];
  y.assign(x.refCounter, x.subObj);
  def z: SrdRef[SubType];
  z = y;
  
  x.release(); // Nothing is released here.
  z.release(); // x will be released here, not x.subObj.
```

#### isNull

```
  handler this.isNull(): Bool;
```

Returns true if the reference is null.

## UnqRef Class

The simplest type of smart references. This reference is not shared and does not use a reference count. When terminating this reference it will
release the object that it owns directly. For example:

```
  def x: UnqRef[MyType];
  x.construct();
  
  def y: UnqRef[MyType];
  y = x; // Error.
  
  x.release(); // Object will be released here.
```

This class contains the following items:

#### obj
The object that this reference points to. In some cases you will need to access the object itself like when you need a pointer or a regular reference
to the object, or in case you want to access one of object's methods that has the same name as one of `UnqRef` methods.

#### construct

```
handler this.construct();
```

This function allocate memory for the object and initialize it also. Initialization of the object is done without arguments, so to use this function
the object must be able to initialize without arguments.

#### release

```
handler this.release();
```

Release this reference and release the object.

#### isNull

```
  handler this.isNull(): Bool;
```

Returns true if the reference is null.

## Error Class

A root for classes that hold runtime error information. It contains two abstract methods:

#### getCode
Returns a code that distinguish between this error and the others.

```
  handler this.getCode(): String as_ptr;
```

#### getMessage
Returns a string representation of the error the could be displayed to the user.

```
  handler this.getMessage(): String as_ptr;
```

## GenericError Class

Inherited from `Error` class. It allows the user to specify a custom error code and message manually for each instance.
It contains two variables:
* **code**: String
* **message**: String

## Possible Class

Template class to hold information of unspecified type (user specify it) with the possibility of holding an error value
in cases of failure. Users who receive a variable of this class should check the error status before using the information held by this class.
It contains the following items:

#### value
The information held by this object. `DataType` class refer to template argument.

```
  @injection def value: DataType;
```

#### error
A reference to the error if exists. If this error is set then operation was failed and the information in `value` is not valid.

```
  def error: SrdRef[Error];
```

#### casting operations
This class contains the following casting operations:

```
  handler this~cast[ref[DataType]] return this.value;
  handler this~cast[Bool] return this.error.isNull();
```

Casting operation as `Bool` returns 1 if the operation succeed, which means the information is valid.

#### success
A function to ease the creation of an object holding valid value.

```
  func success (v: DataType): Possible[DataType];
```

#### failure
A function to ease the creation of error object.

```
  func failure (err: SrdRef[Error]): Possible[DataType];
```

## Nullable Class

Template class to hold information of unspecified type (user specifies it) in addition to `null` value as one of the possible values.

#### value
The information held by this object. `DataType` class refer to template argument.

```
  @injection def value: DataType;
```

#### isNull
Boolean value to specify whether the value is `null` or not.

```
  def isNull: Bool;
```

#### casting operations
This class contains casting operation that automatically casts to original information type.

```
  handler this~cast[ref[DataType]] return this.value;
```

## Memory Module

Memory module contains the following functions:

#### alloc

```
@expname[malloc] func alloc (size: ArchInt): ptr;
```

Allocates a memory block on the heap and returns its pointer. This is the same as `malloc` function from POSIX.

#### realloc

```
@expname[realloc] func realloc (p: ptr, newSize: ArchInt): ptr;
```

Changes the size of an allocated memory block. This may result in copying the memory block to a new location.
This is the same as `realloc` function from POSIX.

#### allocAligned

```
@expname[aligned_alloc]
func allocAligned (alignment: ArchInt, size: ArchInt): ptr;
```

This is the same as `aligned_alloc` function from POSIX.

#### free

```
@expname[free] func free (p: ptr);
```

Frees an allocated memory blockl. This is the same as `free` function from POSIX.

#### copy

```
@expname[memcpy] func copy (target: ptr, src: ptr, size: ArchInt): ptr;
```

Copies a memory block to a new location. This is the same as `memcpy` function from POSIX.

#### compare

```
@expname[memcmp] func compare (s1: ptr, s2: ptr, size: ArchInt): Int;
```

Compares two memory blocks. Returns a value < 0 if the first non-matching byte in s1 is smaller than its
counterpart in s2. Returns a value > 0 if it's greater. Returns 0 if the two blocks are identical.
This is the same as `memcmp` function from POSIX.

#### set

```
@expname[memset] func set (s: ptr, c: Int, n: ArchInt): ptr;
```

Sets all bytes in the specified memory buffer to the given value.
This is the same as `memset` function from POSIX.

### Custom Memory Management
The definitions `alloc`, `realloc`, `allocAligned`, and `free` are in fact pointers to system
functions. The user can override these pointers to provide custom allocators using the
function `overrideAllocator`, and can then reset to the default allocator using the function
`resetAllocator`.

```
function overrideAllocator(
    customAlloc: ptr[function (size: ArchInt) => ptr[Void]],
    customRealloc: ptr[function (p: ptr[Void], newSize: ArchInt) => ptr[Void]],
    customAllocAligned: ptr[function (alignment: ArchInt, size: ArchInt) => ptr[Void]],
    customFree: ptr[function (pointer: ptr[Void])]
);

function resetAllocator();
```

Custom allocators will need to directly reach the system allocation functions, which are
listed here:

`sysAlloc`: Corresponds to `alloc`.

`sysRealloc`: Corresponds to `realloc`.

`sysAllocAligned`: Corresponds to `allocAligned`.

`sysFree`: Corresponds to `free`.

## Math Module

`Math` module contains the following math functions:

#### abs

```
@expname[abs] func abs (i: Int[32]): Int[32];
@expname[llabs] func abs (i: Int[64]): Int[64];
@expname[fabsf] func abs (f: Float[32]): Float[32];
@expname[fabs] func abs (f: Float[64]): Float[64];
```

Get the absolute value of a number.

#### mod

```
@expname[fmodf] func mod (x: Float[32], y: Float[32]): Float[32];
@expname[fmod] func mod (x: Float[64], y: Float[64]): Float[64];
```

Get division remainder of two numbers.

#### exp

```
@expname[expf] func exp (x: Float[32]): Float[32];
@expname[exp] func exp (x: Float[64]): Float[64];
```

This is the same as `expf` and `exp` functions from POSIX.

#### exp2

```
@expname[exp2f] func exp2 (x: Float[32]): Float[32];
@expname[exp2] func exp2 (x: Float[64]): Float[64];
```

This is the same as `exp2f` and `exp2` functions from POSIX.

#### log

```
@expname[logf] func log (x: Float[32]): Float[32];
@expname[log] func log (x: Float[64]): Float[64];
```

This is the same as `logf` and `log` functions from POSIX.

#### log2

```
@expname[log2f] func log2 (x: Float[32]): Float[32];
@expname[log2] func log2 (x: Float[64]): Float[64];
```

This is the same as `log2f` and `log2` functions from POSIX.

#### log10

```
@expname[log10f] func log10 (x: Float[32]): Float[32];
@expname[log10] func log10 (x: Float[64]): Float[64];
```

This is the same as `log10f` and `log10` functions from POSIX.

#### sqrt

```
@expname[sqrtf] func sqrt (x: Float[32]): Float[32];
@expname[sqrt] func sqrt (x: Float[64]): Float[64];
```

This is the same as `sqrtf` and `sqrt` functions from POSIX.

#### cbrt

```
@expname[cbrtf] func cbrt (x: Float[32]): Float[32];
@expname[cbrt] func cbrt (x: Float[64]): Float[64];
```

This is the same as `cbrtf` and `cbrt` functions from POSIX.

#### pow

```
@expname[powf] func pow (b: Float[32], e: Float[32]): Float[32];
@expname[pow] func pow (b: Float[64], e: Float[64]): Float[64];
```

This is the same as `powf` and `pow` functions from POSIX.

#### sin

```
@expname[sinf] func sin (x: Float[32]): Float[32];
@expname[sin] func sin (x: Float[64]): Float[64];
```

This is the same as `sinf` and `sin` functions from POSIX.

#### asin

```
@expname[asinf] func asin (x: Float[32]): Float[32];
@expname[asin] func asin (x: Float[64]): Float[64];
```

This is the same as `asinf` and `asin` functions from POSIX.

#### sinh

```
@expname[sinhf] func sinh (x: Float[32]): Float[32];
@expname[sinh] func sinh (x: Float[64]): Float[64];
```

This is the same as `sinhf` and `sinh` functions from POSIX.

#### asinh

```
@expname[asinhf] func asinh (x: Float[32]): Float[32];
@expname[asinh] func asinh (x: Float[64]): Float[64];
```

This is the same as `asinhf` and `asinh` functions from POSIX.

#### cos

```
@expname[cosf] func cos (x: Float[32]): Float[32];
@expname[cos] func cos (x: Float[64]): Float[64];
```

This is the same as `cosf` and `cos` functions from POSIX.

#### acos

```
@expname[acosf] func acos (x: Float[32]): Float[32];
@expname[acos] func acos (x: Float[64]): Float[64];
```

This is the same as `acosf` and `acos` functions from POSIX.

#### cosh

```
@expname[coshf] func cosh (x: Float[32]): Float[32];
@expname[cosh] func cosh (x: Float[64]): Float[64];
```

This is the same as `coshf` and `cosh` functions from POSIX.

#### acosh

```
@expname[acoshf] func acosh (x: Float[32]): Float[32];
@expname[acosh] func acosh (x: Float[64]): Float[64];
```

This is the same as `acoshf` and `acosh` functions from POSIX.

#### tan

```
@expname[tanf] func tan (x: Float[32]): Float[32];
@expname[tan] func tan (x: Float[64]): Float[64];
```

This is the same as `tanf` and `tan` functions from POSIX.

#### atan

```
@expname[atanf] func atan (x: Float[32]): Float[32];
@expname[atan] func atan (x: Float[64]): Float[64];
```

This is the same as `atanf` and `atan` functions from POSIX.

#### atan2

```
@expname[atan2f] func atan2 (y: Float[32], x: Float[32]): Float[32];
@expname[atan2] func atan2 (y: Float[64], x: Float[64]): Float[64];
```

This is the same as `atan2f` and `atan2` functions from POSIX.

#### tanh

```
@expname[tanhf] func tanh (x: Float[32]): Float[32];
@expname[tanh] func tanh (x: Float[64]): Float[64];
```

This is the same as `tanhf` and `tanh` functions from POSIX.

#### atanh

```
@expname[atanhf] func atanh (x: Float[32]): Float[32];
@expname[atanh] func atanh (x: Float[64]): Float[64];
```

This is the same as `atanhf` and `atanh` functions from POSIX.

#### ceil

```
@expname[ceilf] func ceil (x: Float[32]): Float[32];
@expname[ceil] func ceil (x: Float[64]): Float[64];
```

This is the same as `ceilf` and `ceil` functions from POSIX.

#### floor

```
@expname[floorf] func floor (x: Float[32]): Float[32];
@expname[floor] func floor (x: Float[64]): Float[64];
```

This is the same as `floorf` and `floor` functions from POSIX.

#### round

```
@expname[roundf] func round (x: Float[32]): Float[32];
@expname[round] func round (x: Float[64]): Float[64];
```

This is the same as `roundf` and `round` functions from POSIX.

#### random

```
@expname[rand] func random (): Int;
```

This is the same as `rand` function from POSIX.

#### seedRandom

```
@expname[srand] func seedRandom (s: Word[32]);
```

This is the same as `srand` function from POSIX.

## Net Module

`Net` module contains functions for making network requests. It has the following members:

#### getBuildDependencies

```
func getBuildDependencies(): Array[String];
```

Returns the required dependencies names needed in case of building executable version of projects that use this module.
In the case of `Net` module the function returns one library which is Curl.
`Net` module contains the full path for the file, not only its name because Alusus depends on a version of Curl bundled with Alusus.

#### get

```
1: func get (
     url: ptr[array[Char]], result: ptr[ptr], resultCount: ptr[Int]
   ): Bool;
2: func get (
     url: ptr[array[Char]], filename: ptr[array[Char]]
   ): Bool;
```

1. Get the resource specified by the url and return it.
2. Get the resource specified by the url and store it in the specified file.

#### uriEncode

```
func uriEncode(CharsPtr): String;
```

URL encodes the given string.

#### uriDecode

```
func uriDecode(CharsPtr): String;
```

URL decodes the given string.

## Console Module

`Console` module contains functions for dealing with the terminal. It has the following functions:

#### getChar

```
@expname[getchar] func getChar (): Int;
```

This is similar to `getChar` function from POSIX.

#### putChar

```
@expname[putchar] func putChar (c: Int): Int;
```

This is similar to `putchar` function from POSIX.

#### print

```
1. @expname[printf] func print (fmt: ptr[array[Char]], ...any): Int;
2. func print (i: Int[64]);
3. func print (f: Float[64]);
4. func print (f: Float[64], n: Int);
```

1. This is the same as `printf` function from POSIX.
2. Prints an integer.
3. Prints a floating point number.
4. Prints a floating point number with the specified number of digits to the right of the decimal point.

#### scan

```
@expname[scanf] func scan (fmt: ptr[array[Char]], ...any): Int;
```

This is the same as `scanf` function from POSIX.

#### getInt

```
func getInt (): Int;
```

Asks the user to enter an integer.

#### getFloat

```
print getFloat (): Float;
```

Asks the user to enter a floating point number.

#### getString

```
func getString (str: ptr[array[Char]], size: Word);
```

Asks the user to enter a string.

### Style

`Style` module contains functions to control the style of writing in the console.
Each one of these functions returns a pointer to an array of chars which could be printed to the console to get the required style.
* **reset**
* **bright**
* **dim**
* **italic**
* **underscore**
* **blink**
* **reverse**
* **hidden**
* **fgBlack**
* **fgRed**
* **fgGreen**
* **fgYellow**
* **fgBlue**
* **fgMagenta**
* **fgCyan**
* **fgWhite**
* **bgBlack**
* **bgRed**
* **bgGreen**
* **bgYellow**
* **bgBlue**
* **bgMagenta**
* **bgCyan**
* **bgWhite**

## System Module

`System` module contains the following definitions:

#### sleep

```
@expname[usleep] func sleep (w: Word): Int;
```

This is the same as `usleep` function from POSIX.

#### setEnv

```
@expname[setenv]
func setEnv (
    name: ptr[array[Char]], value: ptr[array[Char]], overwrite: Int
): Int;
```

This is the same as `setenv` function from POSIX.

#### getEnv

```
@expname[getenv]
func getEnv (name: ptr[array[Char]]): ptr[array[Char]];
```

This is the same as `getenv` function from POSIX.

#### exec

```
@expname[system] func exec (cmd: ptr[array[Char]]): Int;
```

This is the same as `system` function from POSIX.

#### exit

```
@expname[exit] func exit (status: Int);
```

This is the same as `exit` function from POSIX.

## Fs Module

`Fs` module contains functionality for dealing with the file system.

#### DirEnt

```
class DirEnt {
  def dType: Int[8];
  def dName: array[Char, FILENAME_LENGTH];
};
```

Information record about an item from folder items.

#### FileNames؛

```
class FileNames {
  def count: Int;
  def names: array[array[Char, FILENAME_LENGTH]];
}
```

A list of files' names.

#### Seek

```
def Seek: {
  def SET: 0;
  def CUR: 1;
  def END: 2;
};
```

Constants to deal with `seek` function.

#### exists

```
func exists (filename: ptr[array[Char]]): Bool;
```

Check if a specific file or folder exists.

#### isDir

```
func isDir (path: ptr[array[Char]]): Bool;
```

Check if the given path is for a directory. It returns true if the given path is a directory, and returns
false if the path is missing or a file.

#### rename

```
@expname[rename]
func rename (oldName: ptr[array[Char]], newName: ptr[array[Char]]): Int;
```

Renames a file or a folder.

#### remove

```
@expname[remove] func remove (filename: ptr[array[Char]]): Bool;
```

Removes a file or a folder.

#### openFile

```
@expname[fopen]
func openFile (filename: ptr[array[Char]], mode: ptr[array[Char]]): ptr[File];
```

Opens a file. This is the same as `fopen` function from POSIX.

#### closeFile

```
@expname[fclose] func closeFile(f: ptr[File]): Int;
```

Closes an open file. This is the same as `fclose` function from POSIX.

#### print

```
@expname[fprintf]
func print (f: ptr[File], fmt: ptr[array[Char]], ...any): Int;
```

Print a string to the file. This is the same as `fprintf` function from POSIX.

#### scan

```
@expname[scanf]
func scan (f: ptr[File], fmt: ptr[array[Char]], ...any): Int;
```

Reads inputs from the file. This is the same as `scanf` function from POSIX.

#### write

```
@expname[fwrite]
func write (
    content: ptr, size: ArchInt, count: ArchInt, f: ptr[File]
): ArchInt;
```

Writes a memory block to a file. This is the same as `fwrite` function from POSIX.

#### read

```
@expname[fread]
func read (
    content: ptr, size: ArchInt, count: ArchInt, f: ptr[File]
): ArchInt;
```

Reads a block of data from the file. This is the same as `fread` function from POSIX.

#### flush

```
@expname[fflush] func flush (f: ptr[File]): Int;
```

Flushes the write buffer. This is the same as `fflush` function from POSIX.

#### tell

```
@expname[ftell] func tell (f: ptr[File]): ArchInt;
```

Get the current seek position of the file. This is the same as `ftell` function from POSIX.

#### seek

```
@expname[fseek]
func seek (f: ptr[File], offset: ArchInt, seek: Int): Int;
```

Moves the file read/write pointer. This is the same as `fseek` function from POSIX.

#### createFile

```
func createFile (
    filename: ptr[array[Char]], content: ptr, contentSize: ArchInt
): Bool;
```

Creates a file and store the given content in it.

#### readFile

```
func readFile (
    filename: ptr[array[Char]], result: ptr[ptr], size: ptr[ArchInt]
): Bool;
```

Reads the full contents of the file and returns it in a new block of memory. The caller is responsible
for releasing the allocated memory buffer.

#### makeDir

```
func makeDir (folderName: ptr[array[Char]], mode: Int): Bool;
```

Create a new folder.

#### openDir

```
@expname[opendir] func (folderName: ptr[array[Char]]): ptr[Dir];
```

Opens a folder for reading. This is the same as `opendir` function from POSIX.

#### closeDir

```
@expname[closedir] func (folder: ptr[Dir]): Int;
```

Closes an open folder. This is the same as `closedir` function from POSIX.

#### rewindDir

```
@expname[rewinddir] func rewindDir (dir: ptr[Dir]);
```

Reset the position of a directory stream to the beginning of a directory. This is the same as `rewinddir` function from POSIX.

#### readDir

```
1. @expname[readdir] func readDir (dir: ptr[Dir]): ptr[DirEnt];
2. func readDir (name: ptr[array[Char]]): ptr[FileNames];
```

1. This is the same as `readdir` function from POSIX.
2. Return a list of files' names in a speicic folder.

## Regex Module

`Regex` module contains functions for dealing with regular expressions.

#### match

```
func match (
    pattern: ptr[array[Char]], str: ptr[array[Char]], flags: Int
): Array[String];
```

Apply the given pattern to the given string and return the result as an array if strings. In case of match the array contains the whole match for the pattern
in the first item in the array, whereas the following items contain the partial match determined by the pattern inside parentheses.
In case of no match, the result is an empty array.

#### Matcher

```
class Matcher {
    handler this~init(pattern: ptr[array[Char]]);
    handler this~init(pattern: ptr[array[Char]], flags: Int);
    handler this.initialize(pattern: ptr[array[Char]]);
    handler this.initialize(pattern: ptr[array[Char]], flags: Int);
    handler this.release();
    handler this.match (str: ptr[array[Char]]): Array[String];
}
```

A class that allows the user to intialize regular expression then use it in multiple searching operations. The method `match` applies the pattern to the given string
and returns an array of strings. In case there exists a match the array contains the whole match of the pattern in the first item in the array
whereas the following items contain the partial match determined by the pattern inside parentheses.
In case of no match, the result is an empty array.

## Time Module

`Time` module contains the following definitions:

#### Time

```
class DetailedTime {
  def second: Int;
  def minute: Int;
  def hour: Int;
  def day: Int;
  def month: Int;
  def year: Int;
  def weekDay: Int;
  def yearDay: Int;
  def daylightSaving: Int;
  def timezoneOffset: Int[64];
  def timezone: ptr[array[Char]];
};
```

A record that holds the date and time information.

#### getDetailedTime

```
1. @expname[localtime] func getDetailedTime (
     ts: ptr[Word[64]]
   ): ptr[DetailedTime];
2. @expname[localtime_r] func getDetailedTime (
     ts: ptr[Word[64]], ptr[DetailedTime]
   ): ptr[DetailedTime];
```

1. This is the same as `localtime` function for POSIX.
2. This is the same as `localtime_r` function for POSIX.

#### getTimestamp

```
@expname[time] func getTimestamp (result: ptr[Word[64]]): ptr[Word[64]];
```

Returns the time as the number of seconds since the Epoch. This is the same as `time` function for POSIX.

#### getClock

```
@expname[clock] func getClock (): Int[64];
```

This is the same as `clock` function for POSIX.

#### toString

```
@expname[clock] func toString (ts: ptr[Word[64]]): ptr[array[Char]];
```

This is the same as `clock` function for POSIX.

## Other definistions

#### castRef

  A macro that is used to represent a reference as a reference of another type. This macro accepts two arguments, the first is the variable we want to represent,
  and the second is the type we want to represent to its reference.

#### nullRef
  A macro that returns a reference with a pointer value of 0. It takes one argument which is the type of the reference's target.

#### alignThis
  A macro that can be used to unshift the value of `this` to make it point to the beginning of the object in case of multiple inheritance
  where an overriden function belongs to a second parent. In this case the parent would not be aligned with the beginning of the
  inheriting object so it needs to be unshifted to point to the beginning of the object before it can be used to access members of
  that object.
  The macro receives one argument, which is the name of the member object referring to the parent in which the override function is
  defined.

#### getThisSourceFullPath
  A macro that gives a string that contains the full path to the current source code file. It does not take any arguments.

#### getThisSourceDirectory
  A macro that returns a string containing the full path to the directory containing the current source code file. This macro does not take any arguments.

#### castSrdRef
  A macro that casts a `SrdRef` into a `SrdRef` of another type, in a similar fashion to `castRef` but for shared references. The first argument is the
  `SrdRef` variable to cast, and the second is the target object type. The resulting `SrdRef` shares the same reference counter as the original, so
  releasing either one behaves correctly regardless of which one is released first, and the original object is only released once the last of them is
  released.

  ```
  def x: SrdRef[MyType] = SrdRef[MyType].construct();
  def y: SrdRef[SubType] = castSrdRef[x, SubType];
  ```

#### allocObj
  A macro that allocates memory for an object of the given type without initializing it, and returns a reference to that object. It takes one
  argument, which is the object's type. This is useful when the object needs to be initialized with arguments, in which case `~init` should be called
  explicitly afterwards.

  ```
  def o: ref[MyType](allocObj[MyType]);
  o~init(someArg);
  ```

#### newObj
  A macro that allocates memory for an object of the given type and initializes it without arguments, and returns a reference to that object. It takes
  one argument, which is the object's type. Objects allocated with this macro should later be freed with `freeObj`.

  ```
  def o: ref[MyType](newObj[MyType]);
  ```

#### freeObj
  A macro that terminates an object and frees the memory allocated for it. It has two forms:

  ```
  freeObj[o];
  freeObj[o, T];
  ```

  The first form takes the reference to the object to free. The second form additionally takes a type argument and casts the object to that type,
  using `castRef`, before terminating it; this is useful when `o` is a reference of a parent type to an object of a child type and you need to
  cast it to the child type so that the correct `~terminate` is invoked.

#### freeObjPtr
  A macro similar to `freeObj`, but it takes a plain pointer rather than a reference. It receives two arguments: the pointer to the object, and the
  object's type, which is used to cast the pointer before terminating the object and freeing its memory.

  ```
  freeObjPtr[p, MyType];
  ```
