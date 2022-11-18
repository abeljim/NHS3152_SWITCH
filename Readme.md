Tried to no change the SDK files at all

Made 2 changes that remove warning but are not required 

added extern main def so main isn't implict 
removed warning for when there is no led defined, maybe not a good idea in long term 
wasnt a good idea long term had to change the file - porbably still a bad idea

# TODO
need to add some mods files to meson still 

# Note
Planning on using
Using picolibc as the libc implementation
Install using the default settings for arme