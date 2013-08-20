// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "Exporter.h"
#include <algorithm>
#include <iostream>
#include <string>

using namespace selene;

int main(int argc, char* args[])
{
        std::cout << "SELENE Device mesh exporter" << std::endl;
        std::cout << argc;

        if(argc > 1)
                std::cout << " arguments";
        else
                std::cout << " argument";

        std::cout << " passed." << std::endl;
        std::cout << "Input:" << std::endl;
        for(int i = 0; i < argc; ++i)
                std::cout << "   <" << args[i] << ">" << std::endl;

        RawMesh rawMesh;

        if(argc >= 3)
        {
                std::string rawMeshFileName = args[1];
                std::replace(rawMeshFileName.begin(),
                             rawMeshFileName.end(), '?', ' ');

                std::string fileName = args[2];
                std::replace(fileName.begin(), fileName.end(), '?', ' ');

                if(rawMesh.read(rawMeshFileName.c_str()))
                {
                        //Exporter exporter(rawMesh);
                        //exporter.doExport(fileName.c_str());
                }
        }

        return 0;
}
