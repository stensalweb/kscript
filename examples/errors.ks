#!ks

func f() {
    try {
        print ([][0])
    } catch e {
        # shorthand:
        # try [STMT] catch e, [RES]
        # you can leave off the 'e' if you don't care specifically about the error
        
        try [][1] catch, ret "another err"

        ret ("there was an error")
    }

    ret 3
}


print(1, 2, f())



