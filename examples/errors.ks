#!ks


try {
    x = 1 / 0
} catch e {
    if typeof(e) >= Error {
        print ("IT WAS AN ERROR")
        throw e
    } else {
        print("nothing too bad")
    }
}


