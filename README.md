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
    |   | Projects CB\           <- CODE::BLOCKS PROJECTS
    |   | Projects VS\           <- VISUAL C++ PROJECTS

ANDROID
-------
Requirements:
* latest Android SDK and NDK (r8d)
* GNU Make ver. >= 3.81
* apache ant ver. >= 1.8.4

To compile core, cd to the Build/Android/Engine and run:

    ndk-build

It will build static library for x86 platform. If you wish to build libraries for all platforms, change APP_ABI in Android.mk:

    APP_ABI := all

Now applications can be built. To do this, cd to the Build/Android/Demos/SimpleApplication or Build/Android/Demos/SimpleAnimation folder and run:

    ndk-build
    ant debug
    ant installd

This applications will run on your android device, but will not render, because currently there is no OpenGL ES renderer implemented.

WINDOWS
-------

To compile existing code under Windows, you should cd to the "Build/Projects CB" and compile projects inside the workspace SELENE-Device.workspace in your Code::Blocks IDE.
Note, that neither Visual Studio 2012, nor Visual Studio 2013 Preview are able to compile this (because they lack C++11 support). But in the near future (as M$ states),
when Visual Studio 2013 RTM comes out, you will be able to compile it with VS. Projects for VS are in "Build/Projects VS" folder.

LINUX
-----

To compile existing code under Linux you should cd to the Build/Linux directory and run:

    make all

Requirements: GCC ver. >= 4.7.3 and GNU Make ver. >= 3.81

LINKS TO DOWNLOADS
==================
Download Visual Studio 2012 Express Edition projects, Linux makefiles, Android build scripts and assets from google drive:
* [Build system and assets](https://docs.google.com/file/d/0Byy41LxMuTKUREd1MmRoUVpfZTA/edit?usp=sharing)

LICENSE
=======
Copyright (c) 2012-2013 Nezametdinov E. Ildus

This software is licensed under the MIT License (see LICENSE.txt). This work can be used in any commercial and/or open-source project.
