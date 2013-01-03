SELENE Device is a 3D graphics rendering engine. It is cross-platform, but currently it has only Windows renderer. For programming style guidlines see GUIDELINES.txt.

Compiling under Linux
=====================

To compile existing code under Linux you should cd to the Build/Linux directory and run:

    make all

Requirements: GCC ver. >= 4.6.3 and GNU Make ver. >= 3.81

Compiling under Android
=======================
Requirements:
* latest Android SDK and NDK
* apache ant

To compile core, cd to the Build/Android/Engine and run:

    ndk-build

It will build static library for x86 platform. If you wish to build libraries for all platforms, change APP_ABI in Android.mk:

    APP_ABI := all

Now applications can be built. To do this, cd to the Build/Android/Demos/SimpleApplication or Build/Android/Demos/SimpleAnimation folder and run:

    ndk-build
    ant debug
    ant installd

This applications will run on your android device, but will not render, because, as mentioned earlier,
currently there is no OpenGL ES renderer.

Compiling under Windows
=======================

To compile existing code under Windows, you should download and unzip SELENE-Device.zip and compile projects in solution called "SELENE Device.sln". Also you should download and unzip ApplicationData.zip.

Note, that your directory tree should be like this (to build and run everything out-of-box):

    - ..                            <- PARENT DIRECTORY
    |\
    | | ApplicationData\            <- APPLICATION DATA (TO RUN DEMOS)
    | + SELENE-Device\              <- SOURCE CODE (CLONED REPOSITORY)
    | |\
    | | | .git\
    | | | Make\
    | | | Source\
    | | | ~ other files ~
    | |
    | + ProjectsVisualStudio\
    |  \
    |   | SELENE-Device\            <- VISUAL C++ PROJECTS
    |
    |~ other files ~

LINKS TO DOWNLOADS
==================
Download Visual Studio 2010 Express Edition projects and application data from google drive:
* [Application data](https://docs.google.com/open?id=0Byy41LxMuTKUdG5fV3dEc2hVcHM)
* [Visual Studio 2010 Express Edition projects](https://docs.google.com/open?id=0Byy41LxMuTKULW5odzdhRWcxVEE)

LICENSE
=======
Copyright (c) 2012 Nezametdinov E. Ildus

This software is licensed under the MIT License (see LICENSE.txt). I hold the copyright, but this work can be used in any commercial and/or open-source project.
