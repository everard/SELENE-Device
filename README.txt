SELENE Device is a 3D graphics rendering engine. It is cross-platform, but currently it has only Windows renderer. Core and exporter can be compiled under Linux with GCC-4.7.

This repository contains Visual Studio 2010 solution and projects. In order to compile the Windows platform, VC+ Directory which contains direct3D 9 headers and libraries must be specified in project called "Windows".

Currently there is only one demo availiable (it is called "SimpleApplication"). To run it you should unzip the ApplicationData.zip archive (see Downloads) to the folder, which contains SELENE Device, for example:
        /home/user/SELENE-Device      <-- SELENE Device folder
        /home/user/ApplicationData    <-- folder, which contains application data

For programming style guidlines see GUIDELINES.txt.

This software is licensed under the MIT License (see LICENSE.txt). I hold the copyright, but this work can be used in any commercial and/or open-source project.
