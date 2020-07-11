# kscript syntax

## Values

### Lists & Tuples

Lists and tuples are created similarly; lists use `[]` and tuples use `()`

For example, a list of integers can be constructed like: `[1, 5, 2]`, and a tuple like `(1, 5, 2)`. You can always include an extra comma at the end, such that:

```python
x = [1, 5, 2]
x = [1, 5, 2,]
y = (1, 5, 2)
y = (1, 5, 2,)
```

Are equivalent. They can also span multiple lines, i.e.:

```python
x = [
    1,
    2,
    3,
]
```

Empty lists are created like `[]`, and empty tuples are created like `(,)`, the comma is required for tuples


### Dictionaries


Dictionaries are created with the `{ KEY: VALUE, }` idiom, like so:

```python
x = {
    "MyName": "Cade",
}

```

Similar to lists & tuples, they can span multiple lines, include an optional extra comma after the last element, and the empty dictionary is `{}`


## Control Flow

A 'statement' is the core of the syntax in kscript. A 'statement' is essentially an action for the computer to perform, such as adding 2 numbers, assigning to a result, calling a function, conditionally running code, looping code execution, function declaration.

A statement normally takes up at least one line, but multiple can be on a single line by using ';' to delimit them. For example, `x = 4; y = 5` is the same as:

```
x = 4
y = 5
```

You can use a ';' to end a statement that has nothing following it, but this is redundant and generally avoided.


## Conditionals/Loops

### `if`

Code can be executed if a given expression is 'truthy', using the `if (condition) { code_to_run; }` construct found in many languages. Essentially, the interpreter evaluates `condition`, and if that condition was `truthy` (i.e. the value `true`, a non-zero integer, or something which has a positive `len(obj)`), then `code_to_run` is executed. Otherwise, it jumps to after the `if` block and continues executing normally

It also has support for 'else' causes, which will run instead of the other code, in the case that the condition is not truthy.

Here are all the cases:

```python

if (A) {
    B
} elif (C) {
    D
} else if (E) {
    F
} else {
    G
}

```

Using `elif` has the same effect as `else if`



### `while`

The `while` loop is similar to most languages' constructs. However, there is no equivalent of `do { ... } while (COND)`

Also, there is a `while; else` loop. It works differently than Python's (see [https://realpython.com/lessons/while-loop-else-clause/](https://realpython.com/lessons/while-loop-else-clause/)).

The `else` part of a `while` loop is executed only if the condition is false on the FIRST run.


The following example illustrates it's use for ensuring that the while loop ran at least once:

```python

data = [...]

while len(data) > 0 {
    # process data...
    data.pop()
} else {
    print ("Had no data!")
}

```

Or, equivalently:

```python

data = [...]

if len(data <= 0) {
    print ("Had no data")
}

while len(data) > 0 {
    # process data...
    data.pop()
}


```

I introduced this feature to help with more readable data processing. For example, some times you would require that `data` has some elements in it. If not, the `else` clause will execute, and you can clearly handle that case.

This way the check conditional (whether `data` had any items) is not repeated


