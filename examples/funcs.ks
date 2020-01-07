#!ks

# function to determine if a given integer is a prime
func is_prime(x) {
    # only integers can be prime
    if type(x) != int, ret false

    # negatives, 0, 1, are not prime
    if x < 2, ret false
    elif x == 2, ret true
    elif x == 3, ret true
    elif x % 2 == 0, ret false

    i = 1
    while i * i <= x {
        if x % (i = i + 2) == 0, ret false
    }

    ret true
}

# tests a given numbers primality, and prints the results
func test(x) {
    if is_prime(x) {
        print(x, "is prime")
    } else {
        print(x, "is not prime")
    }
}


test(2)
test(3)
test(4)
test(16)
test(17)

test(9001)

