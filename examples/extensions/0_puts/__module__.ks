#!/usr/bin/env ks
"""


"""

__authors__ = [
  "Cade Brown <brown.cade@gmail.com>"
]

__version__ = (0, 0, 1)


# internal implementation
_impl = none

# search through implementation names and attempt to import them
for _impl_name in [
    #"_puts_c",
    #"_puts_ks",
] {
    try {
        # attempt to set the implementation
        _impl = __import__(_impl_name)
        #break
    } catch e {
        # otherwise, we have an error
        if !issub(type(e), ImportError), throw e
    }
}


# throw an error if one was not found
if !_impl, throw ImportError("Could not find a valid implementation to provide `puts`")


# main function, which is just a wrapper around the implementation which was found
func puts(obj) {
    """Output `obj` (converted to `str`) to the standard output

    If `obj` is not convertible to a `str`, an error will be thrown

    Return `none`, always

    """

    # convert to a string, if it is not already
    if type(obj) != str, obj = str(obj)

    # call the implementation
    _impl.puts(obj)
}
