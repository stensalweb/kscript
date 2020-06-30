#!/usr/bin/env ks
""" simple_speed.ks - a basic, simple speed test comparing kscript & nx routines



@author: Cade Brown <brown.cade@gmail.com>

"""

# import NumeriX library
import nx

# import the 'getarg' library, which handles parsing command line arguments
import getarg

p = getarg.Parser("simple_speed", "0.0.1", "Testing out the speed between kscript & the nx library", ["Cade Brown <brown.cade@gmail.com>"])

p.add_arg_single("num", "How many data points?", ["-n", "--num"], int, 1000000)
p.add_arg_single("trials", "How many trials?", ["-t", "--trials"], int, 10)
p.add_arg_single("dtype", "What data type should be used for the comparison?", ["-d", "--dtype"], nx.dtype, nx.dtype.FP64)

args = p.parse()


print (" ** Starting trial with %r %r".format(args.num, args.dtype))


# kscript data
kA = list(range(args.num))
kB = list(range(3, 3 + 100 * args.num, 100))
kC = list(range(args.num - 1, -1, -1))



print ("    Convert to nx.array")

st = time()

for i in range(args.trials) {
    nA = nx.array(kA, args.dtype)
}

et = time() - st

print ("      - took %r ms/iter".format(et / args.trials))


nB = nx.array(kB, args.dtype)
nC = nx.array(kC, args.dtype)


print ("    C = A + B")

st = time()

for i in range(args.trials) {
    nx.add(nA, nB, nC)
}

et = time() - st

print ("      - took %r ms/iter".format(et / args.trials))



print ("    C = A + B (new)")

st = time()

for i in range(args.trials) {
    nC = nx.add(nA, nB)
}

et = time() - st

print ("      - took %r ms/iter".format(et / args.trials))















