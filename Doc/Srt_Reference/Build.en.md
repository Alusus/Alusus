# Build Library Reference 

`Build` library allows the user to build an executable file for their program. The library contains the following items:

## Exe Class

Allows the user to build an executable file for their program that works on the current operating system.
This class needs `gcc` tools to be available on the operating system.
Building an executable file is done following these steps:

* Create an object from `Exe` class and pass to it a pointer to the source code element that we want to convert
it to an executable file, in addition to the executable file's name that we want to build.
It is possible to get the element's pointer using `~ast` command.

* Adding any dependencies using `addDependency` function and pass to it a pointer to the module that the program
depends on. Notice that adding dependency is required only when the module needs external libraries.
The module set what external libraries it needs using `@deps` modifier, but if the module does not need
external libraries then no need to add it as a dependency manually because the compiler does that automatically.

  It is also possible to give the function dependency file's name instead of giving it a pointer to a module,
  and in case of using dependencies files' names then it is possible to use `addDependencies` function to pass
  mutilple dependencies to the function at once.

* Adding any other options that linker needs using `addFlag` or `addFlags` functions.

* Creating an executable file by calling `generate` function.

The following example shows how to do these steps:

```
  def exe: Build.Exe(WidgetGuide.start~ast, "hello_world");
  exe.addDependency(Gtk~ast); // or exe.addDependency(String("libgtk...."));
  exe.addDependencies(Array[String]({ String("libcurl.so"), ... }));
  exe.addFlag("-Wl,-rpath,@executable_path");
  if exe.generate() {
    Srl.Console.print("Build complete.\n");
  } else {
    Srl.Console.print("Build failed.\n");
  };
```

The next code shows how libraries developers could add external dependencies information to their modules:

```
  @deps["libmyextlib.so"] module MyLib {
    ...
  };
```

Without adding `@deps` modifier, the `addDependency` function does nothing.
You can control the result of the build by changing the value of the
`targetTriple` member of the `Exe` class, which allows targetting an OS
different from the current OS. You can also specify a different linker
from the default one by setting the `linkerFilename` of the `Exe` class.
Through these two members you can target an OS different from the current
OS. For example, you can build for Windows from a Linux machine by doing
the following:

```
  def exe: Build.Exe(WidgetGuide.start~ast, "hello_world");
  exe.targetTriple = "x86_64-pc-win32";
  exe.linkerFilename = "x86_64-w64-mingw32-g++";
  exe.generate();
  
```

## Wasm Class

This class is similar to `Exe` class and used in the same way but it generates web assemply code instead of a code
that targets operating system and curren device architecture.

```
  def wasm: Build.Wasm(HelloWorld.start~ast, "hello_world");
  wasm.addDependency(String("stdlib.wasm"));
  wasm.addFlags({ String("--export=malloc"), String("--export=realloc") });
  if wasm.generate() {
    Srl.Console.print("Build complete.\n");
  } else {
    Srl.Console.print("Build failed.\n");
  };
```

## genExeceutable Function

A helper function to create an executable file in one step. This function is useful in case building process does not
need any dependencies or additional options. This function is just a interface for `Exe` class since internally it is using
the `Exe` class to do the work.

```
  Build.genExecutable(start~ast, "hello_world");
```

## genWasm Function

A helper function to create a web assemply file in one step. This function is useful in case building process does not
need any dependencies or additional options. This function is just a interface for `Wasm` class since internally it is using
the `Wasm` class to do the work.

```
  Build.genWasm(start~ast, "hello_world");
```
