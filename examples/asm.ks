#!ks

func adder(A) {
    func cl(x) {
        ret x + A
    }
    ret cl
}


asm {
    load "print"
    const "Hello World"
    call 2
    popu
}

print (adder(3)(1))
