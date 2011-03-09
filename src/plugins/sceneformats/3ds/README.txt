This implementation of 3D studio max file import relies on the library
lib3ds available from http://lib3ds.org/.  A copy of this library is
included with Qt/3D, or you can download and build your own version
as described below.

To enable building and use of the plugin, pass CONFIG+=system_3ds to qmake

Currently version 1.3 is required.  Do not use version 2.0 or later as it is incompatible.

For Linux systems typically 1.3 can be obtained by installing a binary package, eg lib3ds-dev

When compiling from source on Unix a simple ./configure, make, make install seems to work fine
on both linux and mac osx.

On windows with Visual Studio the lib3ds library can be compiled as follows.

1)  Download the zipped source of 1.3 and expand it, cd into the directory created.

2)  Edit the file msvc8\lib3ds.rc
   -#include "winres.h"
   +#include "WinResrc.h"

3) Locate the vcbuild.exe on the windows system:

    // this is needed for VS9
    C:\path\to\vcbuild.exe /upgrade msvc8\lib3ds.vcproj
    C:\path\to\vcbuild.exe msvc8\lib3ds.vcproj

4) Release and debug versions of the library files will be created.  Install by:

    xcopy msvc8\release\* C:\path\to\bin
    xcopy msvc8\debug\* C:\path\to\bin

To avoid having to install the lib3ds.org library/dll/headers in a path where the compiler
will locate them, pass the following arguments to qmake:
     QMAKE_LIBDIR_3DS=C:\build\lib3ds\msvc8\release
     QMAKE_INCDIR_3DS=C:\build\lib3ds

If the library is called something different to lib3ds-1_3 on windows or lib3ds.1.3.so on
Unix, for example to use the debug library on windows pass this argument to qmake:
     QMAKE_LIBS_3DS=lib3ds-1_3d

There is a handy 3dsplay utility in the lib3ds distribution - its not needed for Qt3D but is
convenient for debugging models.

To build the 3dsplay utility - a command line 3ds viewer that uses GLUT and OpenGL - after
building and installing lib3ds as above, from the build directory use:
Linux: gcc -o 3dsplay -lGL -lglut -l3ds examples/3dsplay.c
MacOSX: gcc -FOpenGL -FGLUT -framework OpenGL -framework GLUT -l3ds -o 3dsplay examples/3dsplay.c

To get image loading support to enable textures, this works on MacSOX:
First install SDL: http://www.libsdl.org/download-1.2.php
And SDL_image: http://www.libsdl.org/projects/SDL_image/
 gcc -c -DUSE_SDL -o examples/3dsplay.o  -I/Library/Frameworks/SDL.framework/Headers -I/Library/Frameworks/SDL_image.framework/Headers examples/3dsplay.c
 gcc -framework SDL_image -framework SDL -framework OpenGL -framework GLUT -l3ds -o 3dsplay examples/3dsplay.o
