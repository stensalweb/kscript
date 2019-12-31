# kscript (ks)

kscript is a dynamic, duck typed, efficient language with a large standard library.

This is all currently a big WIP, lots of rewrites, etc, but it is becoming promising. It has a very small footprint as far as memory, CPU, and disk. It aims to be sleek above all else, being extremely extensible, and the syntax is made to solve many issues I personally have with many languages.


## About

As broad as I could go, I think that kscript could work as a better replacement for front-end web development (aka replace JavaScript), as it solves many issues JavaScript faces:
  * Prototypal inheritance, whereas kscript has normal inheritance like every sane language
  * Very complicated parsing, which is a bottleneck for large objects, and especially libraries, whereas kscript has a hand-rolled hybrid recursive descent & shunting yard parser
  * Awful type system, specifically for comparisons (`===`, anyone?) and operator overloading (this would be a great feature, especially for online calculators/math apps), whereas kscript has better operator overloads (and a consideration to use the `A ~ B` operator as an approximate equals)
  * Web Standards have become so bloated, but with many things being obseleted, they now are almost useless, whereas kscript will have libraries to manipulate text, audio, images, dictionary-like data (similar to JSON)
  * ... and the list goes on

At the same time, I also wanted something which could rival Python as a usable scripting and/or systems scripting language. However, I did away with relevant whitespace, Python's overuse of `:` (which even Python core devs are regretting with the addition of the walrus operator), odd choices for vocabulary (like, why do I `except` something always? I prefer `catch`, because sometimes you expect a so-called 'exception' to be thrown, so it's not really anything special. This is just one example, though), and again, feature bloat.

Python has a ~20-40ms startup time for me, varying on which machine I use, etc. But this is just to print out a single string! For system utilities (which may be called in a loop from a shell), this time can start adding up.

## Progress

At this point in writing, I am still working on parsing, code generation, VM implementation, and standard types, but not really started on the standard library. It will, of course, be a WIP for some time before it's really stable, but it will be usable fairly soon.

Here is the progress on builtin types:

  * `int`: 90%, represents a 64 bit signed integer
  * `long`: 0%, represents an arbitrarily long integer (this will come at a later date)
  * `str`: 75%, represents a string of characters. Most of what needs to be done is interning, i.e. keeping only one copy of each string alive at a time
  * `cfunc`: 100%, just a wrapper around a callable C function
  * `code`: 25%, executable bytecode wrapper, needs serialization to disk & all the bytecode instructions
  * `parser`: 40%, still needs things like subscripting, function definitions, dictionaries, but the infrastructure is all there
  * `ast`: 40%, still needs many features, like branches, all the operators, loops, functions
  * `list`/`tuple`: 70%: may need an optimized reallocation function, and freeing memory once the list gets small enough
  * `dict`: 70%, fully working as is with generic types, but of course it needs to be profiled and optimized
  * `type`: 20%, needs operator functions, creation functions, freeing functions, etc to be defined more clearly

Here is the progress on builtin functions:
  * `add`/operators: 10%, barely started them

## Examples

See the `examples/` folder for examples. Here are some of the basics:

```
print ("Hello World")
```


## Basic Syntax

The syntax is fairly simple:

  * `#...` denotes a single line comment (like python)
  * `#* ... *#` denotes a start/end comment
  * `a + b` yields the result of the sum of `a` and `b` (works the same for operators like `-`,`*`,`/`,`%`,...)
  * normal order of operations applies here: `a + (b * c)` evaluates `b * c` first, then adds `a` and the result
  * `f(a, b, c)` is a function call with 3 arguments onto the function `f`
  * `f[a]` is a get call, which will work the way it does for most other language for respective types (lists will treat `a` as the index into the values, dictionaries will treat `a` as the key, etc)



