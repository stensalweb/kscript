#!ks


func is_prime(x) {
    if x < 2 then ret false
    if x == 2 then ret true
    if x % 2 == 0 then ret false

    i = 3
    while i * i <= x {
        if x % i == 0 then ret false
        i = i + 1
    }

    ret true
}



func test(x) {
    if is_prime(x) {
        ret print(x, "is prime")
    }
    ret print(x, "is not prime")
}


test(2)
test(3)
test(4)
test(16)
test(17)
