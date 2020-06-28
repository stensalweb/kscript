#!/usr/bin/env ks
""" socketdemo.ks - simple example of how to use sockets in kscript (using the builtin 'soc' library)


"""

# import the 'soc' library, which handles sockets
import sock

# import the 'getarg' library, which handles parsing command line arguments
import getarg

p = getarg.Parser("socketdemo", "0.0.1", "Having fun with sockets!", ["Cade Brown <brown.cade@gmail.com>"])

# todo: add 'choices' as a validator, so ['client', 'server'] could be the roles
p.add_arg_single("port", "What port should the socket be on?", ["-p", "--port"], int, 8080)
p.add_arg_single("role", "What role should this porgram be on?", ["-r", "--role"], str, "client")
p.add_arg_single("num", "Maximum number of connections (server only)", ["-n", "--num"], int, 5)

args = p.parse()

# default to localhost
if len(args.positional) < 1, args.positional.push("localhost")

# print out basic info
print ("ROLE: " + args.role)

if args.role == 'server' {
    # host & bind server

    S = sock.Socket()
    S.bind(args.positional[0], args.port)
    S.listen(args.num)

    while true {
        conn = S.accept()[0]
        print ("GOT: " + conn)

        while true {
            msg = conn.recv(4)
            print (msg)
        }
    }

} else if args.role == 'client' {
    # connect via client protocl
    S = sock.Socket()
    S.connect(args.positional[0], args.port)

    while true {
        S.send("heyo")
    }

} else {
    throw Error("Unknown role: " + args.role)
}



