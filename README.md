lasergbrl2pen :

This program is created to change lasergbrl g-code into g-code for marlin based cnc.

The laserintesity is converted into Z-depth.

A choice to make Up-Down-Up motion for each coordinate is added (G80/G81 simulation)

Compilation on linux :

gcc -o  lasergbrl2pen lasergbrl2pen.c

Also compiling for windows 32 and 64 bit on linux is possible with mingw-w64 (sudo apt-get install mingw-w64):

i686-w64-mingw32-gcc -o lasergbrl2pen_32.exe lasergbrl2pen.c

x86_64-w64-mingw32-gcc -o lasergbrl2pen_64.exe lasergbrl2pen.c

Nothing has to be installed to run this on windows.

Can be run in cmd with a commandline or open your file with lasergbrl2pen_64.exe.

A windows 64 bit binary is added !


If my program becomes better i will add more information about the project.
(this is it for now)
