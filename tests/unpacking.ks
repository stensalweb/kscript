#!/usr/bin/env ks
""" tests/unpacking.ks - testings basic iterable unpacking

@author: Cade Brown <brown.cade@gmail.com>
"""


func my_print(*args) {
    print(args)
}


"""
x, y = (1, 2)
assert x == 1 && y == 2
x, y, = (1, 2,)
assert x == 1 && y == 2

x, *y = (1, 2)
assert x == 1 && y == [2,]

x, *y, = (1, 2)
assert x == 1 && y == [2,]
"""

# left folding on an operator (or any function which takes 2 arguments)
func my_lfold(op, l, *args) {
    # base case
    if !args, ret l
    # otherwise, recursively combine
    ret op(l, my_lfold(op, *args))
}

# for example, implementation of sum:
func my_sum(objs, initial=none) {
    if initial != none, ret my_lfold(__add__, initial, *objs)
    else, ret my_lfold(__add__, *objs)
}


#x = [1, 2, 3]

# ensure it computed correctly
#assert my_sum(x) == sum(x)

# test a product
#assert my_lfold(__mul__, 2, 3, 4) ==  2 * 3 * 4

func fib(i) {
    if i < 2, ret i

    ret __recurse__(i - 1) + __recurse__(i - 2)
    #ret fib(i - 1) + fib(i - 2)
}









