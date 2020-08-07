#!/usr/bin/env ks
""" main.ks - main kscript file to control the building of the documentation

@author: Cade Brown <brown.cade@gmail.com>
"""

import getarg

p = getarg.Parser("main", "0.0.1", "Tool to build the kscript documentation", ["Cade Brown <brown.cade@gmail.com>"])

args = p.parse()

x = ios("docs-src/index.md", "r").reads()



if x.startswith('---') {
    print ("HAS META: ", x.split('\n'))
    
}



