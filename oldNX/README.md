# `modules/nx` - numerics, tensors module

This is a numerics library for tensors, arrays, matrices, etc

## Building

`nx` doesn't require any special dependencies, so it should always be enabled, and built as part of the normal build process (set `KSM_STD='nx ...' make)


## Exported Types

`nx.array` -> a tensor which can hold C-style values in an array

`nx.view` -> a view of a tensor, which allows you to edit values, or view a subset of an `nx.array`


## Exported Functions

TODO: