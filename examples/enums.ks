#!/usr/bin/env ks

Side = Enum.create("Side", [
    "None",
    "Left",
    "Right"
])

print (Side.get("Left"), Side.Left)


assert Side.get("Left") == Side.Left

print (Side._enum_idxs)


