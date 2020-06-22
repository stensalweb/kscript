
# Linux


Check [here](https://github.com/ChemicalDevelopment/kscript/releases) for releases. Every 



# MacOS



# From Source

## Requirements

The requirements for building from source are:

  * Python3.7+
  * gcc
  * GMP (optional)
  * GNU Readline (optional)

And, for all the modules, you will need:

  * libcurl (`req` library)

You can build these individually, or install them from your system package manager, for example:

Debian/Ubuntu: `sudo apt install python3 libgmp-dev libreadline-dev libcurl4-openssl-dev`

MacOS (homebrew): `brew install python gmp readline curl`

Windows: Set up Cygwin packages for all of them (I can't remember the names, but they should all be included as Cygwin packages that you can select for installation)


## Building


You can always see and download the source code [here](https://github.com/ChemicalDevelopment/kscript/releases). It's recommended to download one of the releases, and unzip that.

However, you can also clone the repository (`git clone https://github.com/ChemicalDevelopment/kscript.git`) and use the most up-to-date source code. There may be bugs in the development version, use at your own risk!

Once in that folder, run:

`$ ./configure`

If you want to customize the installation, you can run `./configure --help` for more options.

For example, to build with absolutely no outside libraries, run `./configure --without-curl --without-gmp --without-readline` first. The help menu gives you more options as well


Once the configuration script has been ran, then run:

`$ make -j8` (where the number after `-j` is the number of cores you want to use - using all your cores is recommended unless you need the CPU for something else while you're building)


Now, you should be able to run `./bin/ks`, and it should give you a prompt like:

```
 %> 
```

You can enter programs line by line and allow them to be executed, like:

```
 %> 2 + 3
5
 %> "Cade" + "Brown"
'CadeBrown'
 %> len([1, 2, 3])
3
```

You can run `./bin/ks FILENAME.ks` to execute a file

Or, you can run `./bin/ks -h` to print out help/usage information


## Installation

Once you have confirmed kscript is working, you can run `sudo make install`, which will install it to `/usr/local` (to select a different location, use `./configure --prefix=NEWLOCATION`)

And now, it should be installed!


