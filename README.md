SELENE Device is a 3D graphics rendering engine. It is cross-platform, and currently has Windows and Android renderers. For programming style guidlines see GUIDELINES.txt.

CONFIGURATION & COMPILATION
===========================

1. Create parent directory, which will contain SELENE Device:

        mkdir SELENE-Device
        cd SELENE-Device
        git clone git://github.com/everard/SELENE-Device.git Repository

2. Download Build.zip archive and unzip it to the SELENE-Device directory.

To compile and run everything out-of-box your directory tree should be like this:

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
* latest Android SDK and NDK (r9)
* GNU Make ver. >= 3.81
* apache ant ver. >= 1.8.4

To compile core, cd to the Build/Android/Engine and run:

    ndk-build

It will build static library for x86 platform. If you wish to build libraries for all platforms, change APP_ABI in Android.mk:

    APP_ABI := all

Now demo can be built. To do this, cd to the Build/Android/Demo folder and run:

    android - update project -p ./ -t android-10
    ndk-build
    ant debug
    ant installd

WINDOWS
-------

To compile existing code under Windows, you can cd to the "Build/Projects-CB" and compile projects inside the workspace SELENE-Device.workspace in your Code::Blocks IDE.
Or you can cd to the "Build/Projects-VS" and compile Visual Studio solution. Note, that only Visual Studio 2013 and later is suitable for compilation.

LINUX
-----

To compile existing code under Linux you should cd to the Build/Linux directory and run:

    make all

Requirements: GCC ver. >= 4.8.1 and GNU Make ver. >= 3.81

LINKS TO DOWNLOADS
==================
Download Visual Studio 2013 Express Edition projects, Code::Blocks projects, Linux makefiles, Android build scripts, assets and plug-ins for Blender from google drive:
* [Build system, assets and plug-ins for Blender](https://drive.google.com/folderview?id=0Byy41LxMuTKUY0Y2aDNzOVZlTnM&usp=sharing)

LICENSE
=======
Copyright (c) 2012-2013 Nezametdinov E. Ildus

This software is licensed under the MIT License (see LICENSE.txt). This work can be used in any commercial and/or open-source project.
