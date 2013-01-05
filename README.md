SELENE Device is a 3D graphics rendering engine. It is cross-platform, but currently it has only Windows renderer. For programming style guidlines see GUIDELINES.txt.

BUILDING
========

To build SELENE-Device you should download SELENE-Device-Build-System.zip archive (links below) and unzip it to the direcory, where SELENE-Device resides.
Note, that your directory tree should be like this (to build and run everything out-of-box):

    - ..                            <- PARENT DIRECTORY
    |\
    | | ApplicationData\            <- APPLICATION DATA (TO RUN DEMOS)
    | + SELENE-Device\              <- SOURCE CODE (CLONED REPOSITORY)
    | |\
    | | | .git\
    | | | Source\
    | | | ~ other files ~
    | |
    | + SELENE-Device-Build-System\
    |  \
    |   | Android\            <- ANDROID PROJECTS
    |   | Linux\              <- LINUX MAKEFILE
    |   | Windows\            <- VISUAL C++ PROJECTS
    |
    |~ other files ~

Compiling under Linux
=====================

To compile existing code under Linux you should cd to the SELENE-Device-Build-System/Linux directory and run:

    make all

Requirements: GCC ver. >= 4.6.3 and GNU Make ver. >= 3.81

Compiling under Android
=======================
Requirements:
* latest Android SDK and NDK (r8d)
* GNU Make ver. >= 3.81
* apache ant ver. >= 1.8.4

To compile core, cd to the SELENE-Device-Build-System/Android/Engine and run:

    ndk-build

It will build static library for x86 platform. If you wish to build libraries for all platforms, change APP_ABI in Android.mk:

    APP_ABI := all

Now applications can be built. To do this, cd to the SELENE-Device-Build-System/Android/Demos/SimpleApplication or SELENE-Device-Build-System/Android/Demos/SimpleAnimation folder and run:

    ndk-build
    ant debug
    ant installd

This applications will run on your android device, but will not render, because, as mentioned earlier,
currently there is no OpenGL ES renderer.

Compiling under Windows
=======================

To compile existing code under Windows, you should cd to the SELENE-Device-Build-System/Windows and compile projects inside the solution SELENE-Device.sln in your Visual C++ 2010.

LINKS TO DOWNLOADS
==================
Download Visual Studio 2010 Express Edition projects and application data from google drive:
* [Application data](https://docs.google.com/open?id=0Byy41LxMuTKUOTh5VVFiS1V4bkE)
* [Visual Studio 2010 Express Edition projects](https://docs.google.com/open?id=0Byy41LxMuTKULW5odzdhRWcxVEE)

LICENSE
=======
Copyright (c) 2012 Nezametdinov E. Ildus

This software is licensed under the MIT License (see LICENSE.txt). I hold the copyright, but this work can be used in any commercial and/or open-source project.
