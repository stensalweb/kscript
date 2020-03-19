# kscript (ks)

kscript is a dynamic, duck typed, easy-to-use language with a comprehensive standard library, including maths, numerical tools, GUI toolkits, networking packages, and more!

It works currently, but is missing a large part of normal functionality, such as file I/O, OS integration, and the extra packages I promise.

Current Efforts:

  * Implementing the math library (`m`)
  * Implementing the numerics library (`nx`), with NumPy-like tensor support
  * Maybe implementing some niceties like python has with tuple unpacking (i.e. `for key, val in dict:`)

## About

In the broadest sens, I think that kscript could be a general purpose language for quick development, and that programs written in kscript could be easily distributed and ran on other systems, without worrying a bunch about package versions and support. I plan to have a general purpose standard library that is cross platform, and supports pretty much all the common programming tasks.

At the same time, I also wanted something which could rival Python as a usable scripting and/or systems scripting language. However, I did away with relevant whitespace, Python's overuse of `:` (which even Python core devs are regretting with the addition of the walrus operator), odd choices for vocabulary (like, why do I `except` something always? I prefer `catch`, because sometimes you expect a so-called 'exception' to be thrown, so it's not really anything special. This is just one example, though), and again, (useless) feature bloat.

Python has a ~20-40ms startup time for me, varying on which machine I use, etc. But this is just to print out a single string! For system utilities (which may be called in a loop from a shell), this time can start adding up. My goal is a <10ms startup time on an average system, even with a module system. Currently, I am well within that.


## Building

To build the current version of kscript (library, and executable), first get a copy of the source code.

To get the latest commit, use `git clone https://github.com/chemicaldevelopment/kscript`. This will clone the repo into `kscript/`.

This will build dependencies of the standard library

Now, `cd kscript`, and run these commands:

  * `make` (builds the library & executable)
  * (optional) `sudo make install` (installs kscript to `/usr/local`)

See [BUILD.md](./BUILD.md) for more information

To change the install path, run `PREFIX=/path/to/install sudo make install`

Now, you can run the kscript binary via: `./bin/ks`

Example (output may differ slightly):
```bash
$ ./bin/ks -h
Usage: ./bin/ks [options] FILE [args...]
       ./bin/ks [options] -e 'EXPR' [args...]

Options:
  -h, --help            Prints this help/usage message
  -e [EXPR]             Run an inline expression, instead of a file
  -v[vv]                Increase verbosity (use '-vvv' for 'TRACE' level)
  -V, --version         Print out just the version information for kscript

kscript v0.0.1 Mar 16 2020 00:29:42
Cade Brown <brown.cade@gmail.com>
```

To run an expression to test, run:

```bash
$ ./bin/ks -e 'print (1 + 2**4)'
17
```


## Progress

At this point in writing, I am still working on parsing, code generation, VM implementation, and standard types, but not really started on the standard library. It will, of course, be a WIP for some time before it's really stable, but it will be usable fairly soon.

Here is the progress on builtin types:

  * `int`: 90%, represents a 64 bit signed integer (and optionally, a larger one)
  * `long`: 0%, represents an arbitrarily long integer (this will come at a later date, may be merged as part of `int`)
  * `str`: 60%, represents a string of characters. Most of what needs to be done is interning, i.e. keeping only one copy of each string alive at a time. Also, more efficiency is definitely needed in reclaiming memory, implementing free lists, etc
  * `cfunc`: 90%, just a wrapper around a callable C function
  * `code`: 50%, executable bytecode wrapper, needs serialization to disk, etc
  * `parser`: 80%, works, has a good error handling system with messages, need to add `for` loops, lambdas, slices, and maybe a few more operators
  * `ast`: 70%, mostly works, but could use pretty printing & reconstruction
  * `list`/`tuple`: 80%: may need an optimized reallocation function, and freeing memory once the list gets small enough
  * `dict`: 80%, fully working as is with generic types, but of course it needs to be profiled and optimized. This will take time, and always be more to do.
  * `type`: 50%, has most operators, `getitem`, `getattr`, etc as functions, but needs inheritance and a way to create them


## Examples

See the `examples/` folder for examples. Here are some of the basics:

### Hello World

This is the classic Hello World example

```
print ("Hello World")
```

### Functions

## Syntax

See [SYNTAX.md](./SYNTAX.md) for more information
