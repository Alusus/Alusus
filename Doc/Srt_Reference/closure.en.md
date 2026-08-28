# Closure Library Reference 

This library adds support for closures.
Closures are functions that carry needed environment data with them, i.e. carries a payload which includes values of outer
variables accessed by the function.

Inner functions can access global variables outside of it, but it can not access local variables inside an outer function that contains the inner function,
because the outer function could be terminated and its variables removed from memory before the inner function is called.
For example:

```
def pf: ptr[func];
func prepareFunc {
    def i: Int = 10;
    pf = func {
        Console.print("%d\n", i); // Error: by the time this line is executed
            // i would have been removed from memory
    };
}

prepareFunc();
pf();
```

To enable an inner function to access the outer function's variable, the function must carry a copy from the data
that it uses from the external function, which is what closures provide. Closures are provided by `closure` library.
The compiler automatically prepares a payload that encapsulates accessed outer variables and attaches them to the clsoure.
The definitions of the closure are similar to inner function definition except that they
use `closure` keyword instead of `function` keyword. Example of closure:

```
import "closure";

def pc: closure ();
func prepareClosure {
    def i: Int = 10;
    pf = closure () {
        Console.print("%d\n", i); // Correct: access to i will be replaced
            // by the compiler to an access to a copy of i
    };
}

prepareClosure();
pc();
```

We can notice from the previous example that writing a closure without a body makes it a closure definition, and it is important to know that
closure record contains a pointer to the function in addition to a shared reference to information record. Responsibility remains on the programmer
to ensure that accessing shared references will not lead to circular references, and hence a memory leak.
For example, if you access a shared reference from a closure and that reference is pointing to the record that contains the closure, then
this will result in memory leak (the record holds the closure which in turn holds the record so both will stay in the memory).
In that case, all we need to do is to make the closure hold a non-shared reference to avoid memory leaks. For example:

```
import "closure";

class Record {
    def c: closure ();
    def i: Int;
}

def r1: SrdRef[Record];
r1.construct();
r1.i = 10;
r1.c = closure() {
    Console.print("%d\n", r1.i);
};
r1.release(); // r1 will not be released and neither will the closure it owns.

def r2: SrdRef[Record];
r2.construct();
r2.i = 10;
def r22: WkRef[Record] = r2;
r2.c = closure() {
    Console.print("%d\n", r22.i);
};
r2.release(); // r2 will be released along with the closure it owns.
```

Closure can receive values and return values as regular functions.

```
import "closure";

def pc: closure (Float): Int;
func prepareClosure {
    def i: Int = 10;
    pf = closure (j: Float): Int {
        return i * j;
    };
}

prepareClosure();
Console.print("%d\n", pc(3.5)); // Prints 35
```

## Data Capture Modes

It is possible to specify the way data is captured from the closure's surrounding context, and it is possible to specify that to each
variable independently. For example, if you want to read a variable from the surrounding context but you don't want to modify its value inside
the closure, then it is enough to capture a copy of that variable, whereas if you want to change the value of a variable inside the closure
then you need to capture a reference to the variable instead. To control the way to capture data, we should follow the following form in
the closure's definition:

```
closure (/*capture_mode_defs*/)&(/*args_defs*/): /*ret_type*/ {
  // closure body
}
```

In the next example we set the way to capture `n` as `by_ref` and that tells the closure to store a reference to that variable instead
of a copy, so changing the variable's value inside the closure will lead to changing the original variable outside the closure.

```
func testCaptureByRef (n: Int) {
    def c: closure (i: Int): Int;
    c = closure (n: by_ref)&(i: Int): Int {
        return i * n++;
    };
    print("n before calling c: %d\n", n);
    print("closure result: %d\n", c(3));
    print("n after calling c: %d\n", n);
}
testCaptureByRef(5);
/*
output:
n before calling c: 5
closure result: 15
n after calling c: 6
*/
```

In that example if we change the way if capturing `n` from `by_ref` to `by_val` then the output after calling the closure is 5 instead of 6.
The following modes of data capturing are available:

* **by_ref:** closure captures a reference to the variable.
if the variable is a reference then the closure will captures that reference, not a reference to the reference.

* **by_val:** closure captures the variable's value.
If that variable is a reference, then the closure will capture the value that this reference points to, not the reference itself.

* **identical:** closure captures the variable literally.
If that variable is a reference then the closure will capture the reference, and if it is a value then it will capture the value.

* **auto:** If you don't set the capturing way manually then auto way will be used.
In that way the compiler determines the way to capture based on the variable's type.
If it is a user-defined type and not `SrdRef` then it will use `identical`, otherwise it will
use `by_val` instead.

It is possible to check whether the closure's pointer is null or not by using `isNull` function.

```
  def c: closure (i: Int): Int;
  c.isNull() // returns true
  c = closure (i: Int): Int { return 0 };
  c.isNull() // returns false
```
