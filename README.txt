SELENE Device is a 3D graphics rendering engine. It is cross-platform, but currently it has only Windows renderer. For programming style guidlines see GUIDELINES.txt.

================================================================================
Compiling under Linux
================================================================================

To compile existing code under Linux you should cd to the Make directory and run
        make all

Requirements: GCC ver. >= 4.6.3 and GNU Make ver. >= 3.81

================================================================================
Compiling under Windows
================================================================================

To compile existing code under Windows, you should download and unzip SELENE-Device.zip and compile projects in solution called "SELENE Device.sln". Also you should download and unzip ApplicationData.zip.

Note, that your directory tree should be like this (to build and run everything out-of-box):

+ ..                            <- PARENT DIRECTORY
|\
| + SELENE-Device\              <- SOURCE CODE (CLONED REPOSITORY)
| |\
| | \
| |  | .git\
| |  | Make\
| |  | Source\
| |  | ~ other files ~
| |
| + VisualStudioProjects\
|  \
|   \
|    | ApplicationData\         <- APPLICATION DATA (TO RUN DEMOS)
|    | SELENE-Device\           <- VISUAL C++ PROJECTS
|
|~ other files ~
+

================================================================================
LICENSE
================================================================================
This software is licensed under the MIT License (see LICENSE.txt). I hold the copyright, but this work can be used in any commercial and/or open-source project.
