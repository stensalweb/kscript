# kscript (ks)

kscript is a dynamic, duck typed, easy-to-use language with a comprehensive standard library, including maths, numerical tools, GUI toolkits, networking packages, and more! Learn more here: [https://chemicaldevelopment.us/kscript](https://chemicaldevelopment.us/kscript), and here: [http://chemicaldevelopment.us/kscript/#/](http://chemicaldevelopment.us/kscript/#/).

As important as the language itself are the modules... Check out `/modules/`, each folder in that folder is a standard module

kscript is a work-in-progress, but most of the important functionality is at least partially complete

Current Efforts:

  * Implementing the standard library & packages
    * Numeric library (`nx`), with NumPy-like tensor support
    * Sockets library (`sock`), for UNIX socket-based networking (mostly functional - lacking edge cases)
    * Requests library (`req`), for `GET`, `POST` requests (working)
    * GUI Application library (`cnk`), based on [Nuklear](https://github.com/Immediate-Mode-UI/Nuklear) is about 15% done, just a matter of all the functions
    * OpenGL/GLFW/etc solution packages. I want to get this right, and so I'm still deciding exactly what it will look like
  * Maybe implementing some niceties like python has with tuple unpacking (i.e. `for key, val in dict:`)
  * Better documentation & tutorials. Maybe even videos at some point
  * Example applications
  * Central packaging & build system, similar to `pip` for Python
    * This system should all be integrated, and available on all systems


## About

In the broadest sense, I think that kscript could be a general purpose language for quick development, and that programs written in kscript could be easily distributed and ran on other systems, without worrying a bunch about package versions and support. I plan to have a general purpose standard library that is cross platform, and supports pretty much all the common programming tasks.

At the same time, I also wanted something which could rival Python as a usable scripting and/or systems scripting language. However, I did away with relevant whitespace, Python's overuse of `:` (which even Python core devs are regretting with the addition of the walrus operator), odd choices for vocabulary (like, why do I `except` something always? I prefer `catch`, because sometimes you expect a so-called 'exception' to be thrown, so it's not really anything special. This is just one example, though), and again, (useless) feature bloat.

Python has a ~20-40ms startup time for me, varying on which machine I use, etc. But this is just to print out a single string! For system utilities (which may be called in a loop from a shell), this time can start adding up. My goal is a <10ms startup time on an average system, even with a module system. Currently, I am well within that (<3ms)

Further, by having a strong standard library, and well documented C APIs, extensions can easily hook into the main library in an effective and well-supported manner. For example, the NumeriX library (`nx` module in kscript) defines an array interface for C-types that could easily be extended by other writers. And, all standard functions and modules will work with the results.


## Building

To build the current version of kscript (library, and executable), first get a copy of the source code.

To get the latest commit, use `git clone https://github.com/chemicaldevelopment/kscript`. This will clone the repo into `kscript/`.

Now, `cd kscript`, and run these commands:

  * `./configure` (configures & detects system info, run `./configure -h` to print help information)
  * `make` (builds modules, libraries, and executable)
  * (optional) `sudo make install` (installs kscript to `/usr/local`)

See [BUILD.md](./BUILD.md) for more information on configuring the build process

To change the install path, run `PREFIX=/path/to/install sudo make install`

Now, you can run the kscript binary via: `./bin/ks`

Example (output may differ slightly):
```bash
$ ./bin/ks -h
Usage: ./bin/ks [options] FILE [args...]
       ./bin/ks [options] -e 'EXPR' [args...]
       ./bin/ks [options] - [args...]

Options:
  -h,--help             Prints this help/usage message
  -e,--expr [EXPR]      Run an inline expression, instead of a file
  -                     Start an interactive REPL shell
  -v[vv]                Increase verbosity (use '-vvv' for 'TRACE' level)
  -V,--version          Print out just the version information for kscript

kscript v0.0.1 release Mar 22 2020 14:12:31
Cade Brown <brown.cade@gmail.com>
```

To run an expression to test, run:

```bash
$ ./bin/ks -e '1 + 2**4'
17
```


## Progress

Most of the standard types have been created (`int`, `float`, `str`, `dict`, `Error`, etc), and a good deal of the builtins have been implemented `getattr`, `iter`, `print`, etc.

A couple of big changes that might be made:

  * keyword arguments, for example `func(x=4, y=5)`. In kscript, `=` expressions are not just statements, so that would assign to a local variable x and y. I also don't want to add a special case, so I think the best solution for keyword arguments is `func(@x=4, @y=5)`. In my opinion, this is far more readable and explicit, so I think this is what will happen
  * unpacking iterables, for example `for x, y in coll`. This would be very useful, and just cause a few changes. Also, perhaps allow starred unpacking?

And things left to do:

  * Standard FILE I/O, some sort of os/sys module for file systems, file creation, etc
  * Somehow figure out slicing with indicies, similar to python (i.e. `arr[1:]`)
  * Custom user type definitions
  * Importing other kscript files
  * kscript-based modules
  * package manager

## Examples

See the `examples/` folder for examples. Here are some of the basics:

You can also see demos of libraries in `modules/<module>/demos` (e.g. `modules/cnk/demos`)

### Hello World

This is the classic Hello World example

```
print ("Hello World")
```

### Functions

## Syntax

See [SYNTAX.md](./SYNTAX.md) for more information
