Tried to no change the SDK files at all

Made 2 changes that remove warning but are not required 

added extern main def so main isn't implict 
removed warning for when there is no led defined, maybe not a good idea in long term 
wasnt a good idea long term had to change the file - porbably still a bad idea

# TODO
Need to add some mods files to meson still 

# Note
Using picolibc as the libc implementation

Install using the default settings for ARM

# Windows Setup
Install arm gcc
install python 
install meson using pip
install pyjlink using pip 

# Use
Currently on works on Linux - adding windows support is simple 

Install meson and ninja 

Install arm gcc 

Install pyjLink

Build using the ./build.sh 

Cd into build/debug 

Compile: ninja

Clean: ninja clean

Flash using Jlink: ninja flash

Reconfigure: ninja reconfigure 
