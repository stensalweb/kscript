#!/usr/bin/env ks
""" socketdemo.py - simple example of how to use sockets in kscript (using the builtin 'soc' library)


TODO:
  * add an argument parsing library, so that arguments and '--help' options can be given
  * allow for more control over addresses & ports

"""

# import the 'soc' library, which handles sockets
import sock

# determine whether it is a server or client, give an error otherwise
is_server = len(__argv__) > 1 && (__argv__[1] == 'server' || [print("Invalid argument"), exit(1)])

if is_server {
    # host & bind server
    print (" -*- SERVER -*- ")

    S = sock.Socket()
    S.bind("localhost", 8080)
    S.listen(3)

    conn = S.accept()[0]

    print (conn)

    while true {
        msg = conn.recv(4)
        print (msg)
    }

} else {
    # connect via client protocl
    print (" -*- CLIENT  -*- ")

    S = sock.Socket()
    S.connect("localhost", 8080)

    while true {
        S.send("heyo")
    }

}



