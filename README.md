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



