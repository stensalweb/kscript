#!ks

# Collatz Conjecture (https://en.wikipedia.org/wiki/Collatz_conjecture) example,
#   iterate until we haven't hit a cycle

i = 5

while i != 1 {
    print (i)

    # perform an iteration of the Collatz function
    if i % 2 == 0, i = i / 2
    else, i = 3 * i + 1
}

# example showing how to use a 'for' loop to iterate through a list
for i in [1, 2, 3] {
    print ("elem:", i)
}


