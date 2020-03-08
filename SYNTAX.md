# kscript syntax


## Control Flow

A 'statement' is the core of the syntax in kscript. A 'statement' is essentially an action for the computer to perform, such as adding 2 numbers, assigning to a result, calling a function, conditionally running code, looping code execution, function declaration.

A statement normally takes up at least one line, but multiple can be on a single line by using ';' to delimit them. For example, `x = 4; y = 5` is the same as:

```
x = 4
y = 5
```

You can use a ';' to end a statement that has nothing following it, but this is redundant and generally avoided.


Similar to C, compound statements can be created with `{ ... }`, which can themselves contain other statements.

For example, 

```

{
    x = 3
    y = 4
    if y > x {
        ...
    }
}
```

Code can be executed if a given expression is 'truthy', using the `if (condition) { code_to_run; }` construct found in many languages

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

