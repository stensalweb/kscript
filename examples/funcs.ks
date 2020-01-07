#!ks

# function to determine if a given integer is a prime
func is_prime(x) {
    # only integers can be prime
    if type(x) != int, ret false

    # negatives, 0, 1, are not prime
    # and check for common cases
    if x < 2, ret false
    elif x == 2, ret true
    elif x == 3, ret true
    elif x % 2 == 0, ret false

    # since we now know x is odd, we only need to check divisibility of odd numbers
    i = 3
    while i * i <= x {
        # if it is divisible, its not prime
        if x % i == 0, ret false
        i = i + 2
    }

    # else, it is prime
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

# make sure some are rejected
test(0)    # not prime
test(1)    # not prime
test(4)    # not prime
test(16)   # not prime
test(9201) # not prime

# test various numbers that are prime
test(2)    # prime
test(3)    # prime
test(17)   # prime
test(9001) # prime
test(9007) # prime


