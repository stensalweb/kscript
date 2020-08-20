# kscript extensions

These are a list of how-to's, best practices, and examples of how to make extensions. Some of this, you can learn from example from the standard library, but sometimes those are convoluted for completeness, performance, or bootstrapping need. Therefore, this document shows how you (the end user) should go about writing extensions for kscript; including setting up a project, building, documenting, and implementing the functionality of a given extension.

Throughout this document, we will be writing extensions in `C99` (the same standard used by the reference implementation of the kscript compiler & interpreter). In the future, implementations may differ (some using different design decisions, or implemented in ways which are not at all compatible with the kscript C-API). In any case; we will try and design extensions which can be implemented for multiple systems accross different architectures and implementations. But, as with anything this prescriptive, we can't account for everything.

See `examples/extensions` folder for the source code discussed here.


## 0. `puts`

The first extension will be a simple `puts` module, with a single function, `puts`. The source code is in `examples/extensions/0_puts`.

Firstly, we would like to define exactly what `puts` will take: a single argument, which is always turned into a string, and then printed to the `stdout` stream (or whatever `puts` would go to in `C`). For completeness, we will implement two versions and use the [Bridge Pattern](https://en.wikipedia.org/wiki/Bridge_pattern) so that it will be easy to implement in the future. 

First, we will create a few files:

```bash
0_puts/
  _puts_ks/
    __module__.ks
  _puts_c/
    Makefile
    module.c  
  __module__.ks
```


In kscript, the `__module__.ks` file is a special name; it is ran when a kscript module is imported. Therefore, we should set up initialization & module definitions in that file. For example:

```python
# __module__.ks - main module file for the `puts` module

__authors__ = [
  "Cade Brown <brown.cade@gmail.com>"
]




```




