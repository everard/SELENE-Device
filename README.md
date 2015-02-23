SELENE Device is a simple 3D graphics rendering engine. It is cross-platform, but has only Windows and Android renderers.

CONFIGURATION & COMPILATION
===========================

1. Create parent directory, which will contain SELENE Device:

        mkdir SELENE-Device
        cd SELENE-Device
        git clone git://github.com/everard/SELENE-Device.git Repository

2. Unzip Utilities/Build.zip archive into SELENE-Device directory.

To be able to compile and run everything out-of-box your directory tree should look similar to this one:

    + SELENE-Device              <- ROOT DIRECTORY
    |\
    | + Repository\              <- SOURCE CODE (CLONED REPOSITORY)
    | |\
    | | | .git\
    | | | Source\
    | | | ~ other files ~
    | |
    | + Build\
    |  \
    |   | Assets\                <- ASSETS NEEDED TO RUN APPLICATIONS
    |   | Android\               <- ANDROID BUILD SCRIPTS
    |   | Linux\                 <- LINUX MAKEFILE
    |   | Projects-CB\           <- CODE::BLOCKS PROJECTS
    |   | Projects-VS\           <- VISUAL C++ PROJECTS

ANDROID
-------
Requirements:
* latest Android SDK and NDK
* GNU Make ver. >= 3.81
* apache ant ver. >= 1.8.4

To compile core, cd to the Build/Android/Engine and run:

    ndk-build

If you wish to build core for all supported platforms, change APP_ABI in the Android.mk.
Now demo application can be built. To do this, cd to the Build/Android/Demo folder and run:

    android - update project -p ./ -t android-10
    ndk-build
    ant debug
    ant installd

WINDOWS
-------

To compile existing code under Windows, you can cd to the "Build/Projects-CB" and compile projects inside the workspace SELENE-Device.workspace in your Code::Blocks IDE.
Or you can cd to the "Build/Projects-VS" and build Visual Studio solution. Note, that only Visual Studio 2013 and later is suitable for compilation.

LINUX
-----

To compile existing code under Linux you should cd to the Build/Linux directory and run:

    make all

Requirements: GCC ver. >= 4.8.1 and GNU Make ver. >= 3.81

LICENSE
=======
Copyright (c) 2012-2013 Nezametdinov E. Ildus

This software is licensed under the MIT License (see LICENSE.txt).
