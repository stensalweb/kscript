---
title="types"
---

## {! title !}

This page lists the essential types, first in a list, and then each one fully listed with methods, etc:

  * obj
  * none
  * number
    * bool
    * int
    * enum
    * float
    * rational
    * complex
  * str
  * bytes
  * list
  * tuple
  * set
  * dict
  * func
  * module
  * Error
    * KeyError
    * SizeError
    * TypeError
    * TodoError
    * MathError
    * ImportError
    * SyntaxError
    * AssertError
    * InternalError
    * OutOfMemError

## obj

`obj` is the base class of all other classes; every object is an instance of `obj`, i.e. `issub(type(x), obj) == true` for all `x`


## none

`none` is a special value in kscript; it typically represents an unspecificed or default value, or otherwise missing object. It is similar to the `NULL` value in C code (which is typically a `void*`), or `nullptr` in C++ (which is of type `nullptr_t`). `type(none)` gives the class, and there is only ever one instance of this type; you can refer to it via the literal `none`.

Key features of `none`:
  * `none == none` returns true, compared to any other object, `none == x` returns `false`


## number

`number` is an abstract base class (`ABC`) that describes the basic methods of a number. There are 6 built-in subclasses, listed in this sub section. All of these types are immutable, meaning they cannot be modified.

The common operations that are implemented in every and between every combination of these types is listed in the table below:


| name           | code      |
| addition       | `a + b`   |
| subtraction    | `a - b`   |
| multiplication | `a * b`   |
| division       | `a / b`   |
| floor division | `a // b`  |
| modulo         | `a % b`   |
| exponentiation | `a ** b`  |
| identitity     | `+a`      |
| negation       | `-a`      |
| absolute value | `abs(a)`  |

Additionally, comparisons are implemented as well:

| comparison                | code      |
| equal to                  | `a == b`  |
| not equal to              | `a != b`  |
| less than                 | `a < b`   |
| greater than              | `a > b`   |
| less than or equal to     | `a <= b`  |
| greater than or equal to  | `a >= b`  |


### bool

`bool` is a subclass of `number` which represents a `true` or `false `value. There are only ever 2 instances of `bool`, and they can be referred to via the literals `true` and `false`. It can also be viewed as a single [bit](https://en.wikipedia.org/wiki/Bit), and indeed, all numerical code will consider `false` the same as `0` and `true` the same as `1` in most circumstances. For example, consider: `true + false == true` and `true * 5 == 1 * 5 == 5`

Constructing a bool via `bool(x)` will return the truthiness value of `x`, which is guaranteed to be either `true` or `false`.

### int

`int` is a subclass of `number` which represents a finite, integral value (i.e. a whole number). Note that unlike `int` in C or other programming languages, `int` has no maximum or minimum (for example, `int` for most C compilers refers to a 32 bit value; the maximum value is `INT_MAX` which is commonly `2147483647`), which means code like the following: `10 ** 100` will print `1` and then `1000` `0`s, whereas in C, there would be an overflow, and some value less than `INT_MAX` would be printed. 

### enum

`enum` is a abstract subclass of `number` which represents a finite, integral value (i.e. a number) that is associated with an [Enumeration](https://en.wikipedia.org/wiki/Enumeration); when used in any expression or mathematical function, it decays to an `int` representing the integer value of it. However, on its own, the enum value has a `name` (of type `str`) and `value` (of type `int`).

`enum`is an abstract type, so it cannot be instantiated; rather a subtype must be created like so:

```
class Side : enum {
    Left       = -1
    Middle     =  0
    Right      = +1
}
```

They may also be created dynamically with a ![dict](types/dict) object like so:

```
Side = enum.create("Side", {
    "Left":     -1, 
    "Middle":    0, 
    "Right":    +1, 
})
```


### float

### rational

### complex

Squid
kscript


## str

`str` is a type representing a [String](https://en.wikipedia.org/wiki/String_(computer_science)), which is a sequence of [Unicode](https://en.wikipedia.org/wiki/Unicode) [code points](https://en.wikipedia.org/wiki/Code_point). Unicode is able to support standard [ASCII](https://en.wikipedia.org/wiki/ASCII) plus many other characters. Like other primitives, this type is immutable, meaning characters can't be modified.

## bytes

`bytes` is a type representing a list of bytes (8 bit, unsigned integers in the range $[0, 256)$). Like `str`, it is immutable, and may not be modified. 

## list



