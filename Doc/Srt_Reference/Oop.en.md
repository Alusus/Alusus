# Oop Library Reference

The `Oop` library adds inheritance and virtual functions to classes. A virtual function is a function that a derived
class can replace with its own version. When the function is called on an object, the version of the object's actual
class runs, even if the call goes through a reference to a parent class. For example, a function that receives any
`Shape` can call `area()` on it and get the right result whether the shape is a rectangle, a square, or something else.

To use the library, import it:

```
import "Oop";
```

The **Behind the scenes** paragraphs explain what these macros actually do when applied to a class, which is
useful in knowing what they add to the classes and how it affects performance, but they are not necessary to
know how to use these macros.

## Defining Virtual Functions

To give a class virtual functions:

1. Add `Oop.defVtable[vtable, VTable];` to the body of the class. `vtable` and `VTable` are names of your choice, but
   you will need to repeat them in the derived classes, so it's simplest to always use these two.
2. Define each virtual function as you would define a regular method using `handler`, then add `within VTable` before
   it and `as_ptr` after it.

```
class Shape {
    Oop.defVtable[vtable, VTable];

    def name: CharsPtr;

    within VTable handler this.area(): Float[64] as_ptr;

    within VTable handler this.describe() as_ptr {
        Console.print("%s: %f\n", this.name, this.area());
    }
}
```

A virtual function without a body, like `area` in the previous example, is an abstract function, i.e. a function that
each derived class is expected to provide. It must not be called on an object whose class doesn't provide it.

Inside these functions `this` refers to the object, same as in regular methods, and the functions are called the same
way as regular methods, as in `s.describe()`. Properties and operators can be virtual as well, as in
`within VTable handler this.sides: Int as_ptr { ... }` and `within VTable handler this + (n: Int): Int as_ptr { ... }`.

**Behind the scenes:** The functions of the class are reached through a table of function pointers, called a vtable.
`defVtable` defines a class named `VTable` for this table, a single shared instance of it named `svtable` (`s`
followed by the first name given to the macro), and a member named `vtable` that refers to that shared instance. Each
`within VTable handler ... as_ptr` adds a function pointer to `VTable`, along with the function it points to. The
`vtable` member is injected, so the pointers can be called directly on the object, and it's marked with
[no_bind](../lang_reference.en.md#binding-members-to-the-enclosing-object-no_bind), so the functions receive the object
as `this` rather than the vtable. Since the functions are defined inside `VTable`, `this` would normally be of type
`VTable`, so the library changes it to the type of the class that owns the vtable. See
[method pointers](../lang_reference.en.md#method-pointers) for more on `as_ptr`.

## Inheriting a Class

To inherit a class that has virtual functions, add `Oop.inheritClass` to the body of the derived class, giving it:

* A name for the member that holds the parent's part of the object, like `shape`.
* The parent class, like `Shape`.
* The same two names that were given to `defVtable` in the parent class.

To replace a virtual function of the parent, define it the same way it is defined in the parent, but add the member
name before the function's name, as in `this.shape.area()`, and use `set_ptr` instead of `as_ptr`. The derived class can
also add new virtual functions of its own using `as_ptr`, the same way as in the parent:

```
class Rect {
    Oop.inheritClass[shape, Shape, vtable, VTable];

    def w: Float[64];
    def h: Float[64];

    handler this~init() {
        this.name = "rect";
    }

    within VTable handler this.shape.area(): Float[64] set_ptr {
        return this.w * this.h;
    }

    within VTable handler this.diagonal2(): Float[64] as_ptr {
        return this.w * this.w + this.h * this.h;
    }
}
```

Inside these functions `this` refers to the `Rect` object. Members of the parent, like `name`, can be used directly on
a `Rect`, and a `Rect` can be passed wherever a `ref[Shape]` is expected. Calling a virtual function through that
reference runs `Rect`'s version:

```
func printShape(s: ref[Shape]) {
    s.describe();
}

def r: Rect;
r.w = 2;
r.h = 3;
printShape(r); // Prints: rect: 6.000000
```

To inherit a class without replacing any of its virtual functions or adding new ones, use the short form
`Oop.inheritClass[shape, Shape]`.

**Behind the scenes:** `inheritClass` adds an injected member named `shape` of type `Shape`, which is why `Shape`'s
members are available on `Rect` and why a `Rect` is accepted as a `Shape`. It also defines a new vtable class for
`Rect`, also named `VTable`, which contains `Shape`'s vtable in a member named `shape`, so `this.shape.area` in an
override refers to the `area` pointer inherited from `Shape`. A shared instance of this vtable is filled with `Rect`'s
versions, and `Rect`'s constructor points `shape.vtable` to it. That is how a call through a `ref[Shape]` reaches
`Rect`'s versions: the call goes through `shape.vtable`, which now points to `Rect`'s vtable. `Rect`'s own functions,
whether new like `diagonal2` or replacements like its `_area`, are reached through an injected property that gives
`Rect`'s vtable.

## Calling the Parent's Version

A function that replaces a function of the parent can still call the parent's version, by adding an underscore before
the function's name:

```
class Rect {
    Oop.inheritClass[shape, Shape, vtable, VTable];
    ...
    within VTable handler this.shape.describe() set_ptr {
        Console.print("Rect: ");
        this.shape._describe(); // Calls Shape's version.
    }
}
```

**Behind the scenes:** A virtual function with a body, defined with `as_ptr` or `set_ptr`, is defined as a regular
method whose name is the function's name preceded by an underscore, and the function pointer is set to point to that
method. Calling `this.shape._describe()` calls `Shape`'s method directly instead of going through the function pointer,
so it always runs `Shape`'s version.

## Inheriting a Derived Class

A class that inherits a class that is itself derived, like a `Square` that inherits `Rect`, uses the long form of
`Oop.inheritClass`, which takes six names:

* A name for the member that holds the parent's part, like `rect`.
* The parent class, like `Rect`.
* The parent's vtable member, like `rect.vtable`. This is always the new member's name followed by `.` and the first
  name given to `defVtable` in the top-most class.
* The parent's vtable class, like `Rect.VTable`.
* A name for the vtable of the new class, like `svtable`.
* A name for the vtable class of the new class, like `VTable`.

To replace a function, start from `this` and go through the member names down to the class that first defined the
function. For example, `area` was first defined in `Shape`, so `Square` replaces it with `this.rect.shape.area()`,
while `diagonal2` was first defined in `Rect`, so `Square` replaces it with `this.rect.diagonal2()`:

```
class Square {
    Oop.inheritClass[rect, Rect, rect.vtable, Rect.VTable, svtable, VTable];

    within VTable handler this.rect.shape.area(): Float[64] set_ptr {
        return this.w * this.w;
    }

    within VTable handler this.rect.diagonal2(): Float[64] set_ptr {
        return this.w * this.w * 2;
    }
}
```

Calling the parent's version works as in the previous section: `this.rect._area()` calls `Rect`'s version of `area`,
and `this.rect._diagonal2()` calls `Rect`'s version of `diagonal2`. If the parent didn't replace a function, the call
goes to the version of the closest ancestor that has one. To skip the parent's version and call a further ancestor's,
continue the path, as in `this.rect.shape._describe()`, which calls `Shape`'s version even though `Rect` replaced it.

**Behind the scenes:** The vtable class of each class contains the vtable of its parent in a member named after the
member that holds the parent, so the path to a function in the vtables matches the path through the objects, as in
`this.rect.shape.area`. Each class gets an injected property that gives its own vtable, and this property is placed at
the top of the class, before the parent member, so the search for `_area` in `this.rect` looks in `Rect`'s vtable,
which has `Rect`'s `_area`, before going through the `shape` member to `Shape`'s vtable. The member that holds the
parent's vtable isn't injected, so this search doesn't go into the parent's vtable, which keeps names like `shape`
referring to the objects rather than the vtables.

## Inheriting Multiple Classes

A class can inherit more than one class that has virtual functions. Use the long form of `Oop.inheritClass` for each
parent, and give each parent's vtable its own names:

```
class Multi {
    Oop.inheritClass[base1, Base1, base1.vtable, Base1.VTable, svtable1, VTable1];
    Oop.inheritClass[base2, Base2, base2.vtable, Base2.VTable, svtable2, VTable2];

    within VTable1 handler this.base1.get1(): Int set_ptr {
        return this.i * 10;
    }

    within VTable2 handler this.base2.get2(): Int set_ptr {
        alignThis[base2, Multi];
        return this.i + this.j * 10;
    }
}
```

There are two rules to keep in mind:

* Functions that replace functions of any parent other than the first must start with `alignThis[member, Class]`,
  where `member` is the name of the member holding that parent and `Class` is the class being defined, as in
  `alignThis[base2, Multi]` in the previous example.
* Only the first parent is accepted automatically where a reference to a parent is expected. For other parents, pass
  the member that holds the parent, as in `printBase2(m.base2)`.

To inherit a class that has multiple parents with virtual functions, use the short form `Oop.inheritClass` to inherit
the class, then add `Oop.inheritVTable` for each of its vtables. `Oop.inheritVTable` takes the same six names as the
long form of `Oop.inheritClass`:

```
class MultiChild {
    Oop.inheritClass[multi, Multi];
    Oop.inheritVTable[multi, Multi, base1.vtable, Multi.VTable1, svtable1, VTable1];
    Oop.inheritVTable[multi, Multi, base2.vtable, Multi.VTable2, svtable2, VTable2];

    within VTable1 handler this.multi.base1.get1(): Int set_ptr {
        return this.i * 100;
    }

    within VTable2 handler this.multi.base2.get2(): Int set_ptr {
        alignThis[base2, MultiChild];
        return this.i + this.j * 100;
    }
}
```

**Behind the scenes:** Each parent part is stored at its own position inside the object, and each has its own vtable
member, which is why each parent needs its own vtable class and shared instance. When a function is called through a
reference to the second parent, `this` points to the `base2` member, which is not at the start of the object.
`alignThis` moves `this` back by the position of that member, so it points to the start of the object again. Only the
first parent is accepted automatically as a reference to the parent because only the first injected member is used
when converting an object to the type of one of its members.

## Macros

### defVtable

```
macro defVtable [vtable, VTable]
```

Adds virtual functions support to a class. `vtable` and `VTable` are the names used to refer to the vtable in the class
and in derived classes. Virtual functions are then defined with `within VTable handler ... as_ptr`.

**Behind the scenes:** Defines the vtable class `VTable`, a shared instance of it named `s` followed by `vtable` (as in
`svtable`), and an injected `no_bind` member named `vtable` that refers to the shared instance.

### inheritClass

```
macro inheritClass [base, Base]
macro inheritClass [base, Base, vtable, VTable]
macro inheritClass [base, Base, baseVtable, BaseVTable, mySharedVtable, MyVTable]
```

Inherits the class `Base` and stores its part of the object in a member named `base`.

* The short form inherits the class without replacing or adding virtual functions.
* The second form also lets the derived class replace and add virtual functions. `vtable` and `VTable` are the names
  given to `defVtable` in `Base`. Functions are defined with `within VTable handler ...` in the derived class.
* The long form is needed when inheriting a derived class or multiple classes. `baseVtable` and `BaseVTable` are the
  parent's vtable member and class, and `mySharedVtable` and `MyVTable` are names for the vtable of the new class.
  Functions are defined with `within MyVTable handler ...` in the derived class.

**Behind the scenes:** Defines an injected member named `base` of type `Base`. The second form is equivalent to
`inheritVTable[base, Base, base.vtable, Base.VTable, svtable, VTable]`, where `svtable` is `s` followed by `vtable`, and
the long form is equivalent to the short form followed by `inheritVTable` with the same args.

### inheritVTable

```
macro inheritVTable [base, Base, baseVtable, BaseVTable, mySharedVtable, MyVTable]
```

Lets a class replace and add virtual functions of one of the vtables of a parent that was already inherited using the
short form of `inheritClass`. The args are the same as the long form of `inheritClass`. It is used when inheriting a
class that has multiple parents with virtual functions.

**Behind the scenes:** Defines the vtable class `MyVTable`, which contains `BaseVTable` in a member named `base`, and a
shared instance of it named `mySharedVtable`. The constructor of the class points `baseVtable` to `mySharedVtable`. It
also adds, at the top of the class wherever the macro is used, an injected `no_bind` property named `mySharedVtable`
followed by `Ref` that gives a reference to the vtable as `MyVTable`, which makes the class's own virtual functions
available on the object.

## Errors

* `SRT1004`: A function replaces a function through a member that doesn't exist in the class, as in
  `this.nonExisting.f()`. Make sure the member name matches the first name given to `inheritClass`.
* `SRT1005`: `defVtable`, `inheritVTable`, or a form of `inheritClass` that takes vtable names was used outside the body
  of a class. These macros can only be used inside the body of the class they apply to.
