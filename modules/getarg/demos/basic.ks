#!/usr/bin/env ks
""" basic.ks - simple example showing how to use the 'getarg' module

@author: Cade Brown <brown.cade@gmail.com>
"""

# import the getarg library (this is part of the standard library)
import getarg

p = getarg.Parser("basic", "v0.0.1", "Basic example showing usage of the 'getarg' module", ["Cade Brown <brown.cade@gmail.com>"])

p.add_arg_single("name", "Your name", ["-n", "--name"], getarg.REQUIRED)
p.add_arg_multi ("aliases", "Other names you go by", ["-a", "--aliases"], getarg.LIST)
p.add_arg_single("money", "How much money do you have?", ["-m", "--money"], int, 0)


args = p.parse()


print ("Hello", p.name, "! Also known by the names:", ",".join(args.aliases))



if args.money {
    print ("You have $", args.money)
}





