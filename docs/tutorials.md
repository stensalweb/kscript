# Tutorials

## 1. Hello World

This tutorial is the classic ["Hello World"](https://en.wikipedia.org/wiki/%22Hello,_World!%22_program) program, in kscript.

We assume you have already [installed](/install) kscript on your platform, and can run kscript with `ks`. If you have another binary name or path, replace it (i.e. replace `ks` with `/path/to/ks`)

Choose a folder where you can put a file, and create a file ending with `.ks`. For this tutorial, I will name mine `hello.ks`. 

My folder now looks like this:

```bash
.
└── hello.ks
```

Open the file in any editor, and enter the following text in the `hello.ks` file:

```kscript
print ('Hello World')

```

This runs the built-in `print` function, invoked with a single string literal argument `'Hello World'`. By default, `print` will output its arguments and then a newline.

The program then exits because it has ran all the statements in the program.



To run it, open your shell and run:

```bash
$ ks ./hello.ks
Hello World
```

It should print out "Hello World" like it did above

### Going Further

On MacOS and Linux-based platforms, you can also set it up to run the file directly. For example, you can run `./hello.ks` instead of `ks ./hello.ks`

To do this you need to first add a [shebang](https://en.wikipedia.org/wiki/Shebang_(Unix)) to the top of the file, so that `hello.ks` reads:

```kscript
#!/usr/bin/env ks
print ('Hello World')

```

This requires that you have `kscript` installed on your `$PATH` (i.e. you could run `ks` by itself). If not, you can use a shebang like `#!/path/to/ks`, but this is not as portable, so it's recommended to use `/usr/bin/env` in your shebang

Now, you need to mark the `hello.ks` file as executable using [chmod](https://en.wikipedia.org/wiki/Chmod). So, run the following command:


```bash
$ chmod +x hello.ks
```

And now, test it out:
```bash
$ ./hello.ks
Hello World
```


## 2. Project Euler #1

This tutorial is an implementation of the [Project Euler Problem #1](https://projecteuler.net/problem=1) in kscript.

The basic goal is to find the sum of all the multiples of 3 or 5 below 1000. You must also be wary not to double-count multiples of 3 and 5 twice.

I'm going to write multiple methods to show how you can use kscript; Only one of the functions is required. These will all be in a file named `pe_1.ks`

My folder now looks like this:

```bash
.
└── pe_1.ks
```


```kscript
# pe_1.ks - project euler #1 solution

# method 1: brute force, going through every number and checking
func method_1(N) {
    sm = 0
    for i in range(N) {
        # if the number is divisible by either 3 or 5, add it to the sum
        if i % 3 == 0 || i % 5 == 0, sm = sm + i
    }

    ret sm
}

# method 2: smarter method, using ranges and the builtin `sum` function
# 'sum' takes a sequence/iterable and returns the sum of all elements
# we have to subtract off multiples of 15, because those are double counted
func method_2(N) {
    ret sum(range(0, N, 3)) + sum(range(0, N, 5)) - sum(range(0, N, 15))
}


# method 3: smartest method, using algebra and no sequences
func method_3(N) {
    # helper function to sum the multiples of `x` between 0 and N
    func summult(x) {
        # number of multiples of x
        nMx = (N-1) / x
        # return `x` times the sum of 1...nMx
        # We use triangular numbers to quickly compute the sum of that
        ret x * nMx * (nMx + 1) / 2
    }
    ret summult(3) + summult(5) - summult(15)
}

# print all 3 results
r1 = method_1(1000)
r2 = method_2(1000)
r3 = method_3(1000)
print (r1)
print (r2)
print (r3)

# assert they are all equal
assert r1 == r2 && r2 == r3

```

Running this yields:

```bash
$ ks ./pe_1.ks
233168
233168
233168
```

All the answers match, and indeed this is the correct answer.









