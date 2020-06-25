# kscript

## What is kscript?

kscript is a dynamic, duck typed, easy-to-use language with a comprehensive standard library, including maths, numerical tools, GUI toolkits, networking packages, and more!

The plan is for kscript to be able to be used for writing system utilities, websites, GUI applications, graphing applications, and anything else you can imagine.

Learn more here: [https://chemicaldevelopment.us/kscript](https://chemicaldevelopment.us/kscript)


## What makes it different?

kscript is guided by a simple philosophy:

  * kscript's syntax should result in code that is easy to read, write, and modify
  * Functions, classes, and modules should be designed and documented so that programmers only need to check docs once; Their usage should be easy to remember, and hard to mess up
  * Installing, using, and modifying programs should be independent of any specific OS; All packages should support all platforms transparently
  * Anything that most programmers need should be in the standard library. This is probably the broadest category, and includes things such as:
    * A single standard for large, dense data storage - implemented in the `nx` module
    * Tensor (i.e. Vector, Matrix) math - implemented in the `nx` module
    * Multi-media functionality (reading/writing/encoding/decoding audio & video) - implemented in the `mm` module (TODO)

When kscript (or modules) don't live up to these expectations, it has failed. Please send a message to [brown.cade@gmail.com](mailto:brown.cade@gmail.com) if you encounter any hardships

With that being said, kscript is the dialectical synthesis of programming languages like Python (thesis) and Golang (antithesis). We have seen what is good with Python (duck typing, packaging systems), and what problems Golang has addressed (application infrastructure, even more simplicity). And now, kscript is a logical evolution of current programming language trends.

It also seeks to be light-weight where possible, so that it can be easily embeddable in other applications requiring user input (`libkscript`)

## How can I get started?

You can check the [docs](https://chemicaldevelopment.us/kscript) (where you are now). Check out the section on [installing kscript](/install) and find your platform.

Once you have kscript installed, feel free to check out the [introduction tutorials](/tutorials)



