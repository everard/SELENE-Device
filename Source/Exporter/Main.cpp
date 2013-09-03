// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "Exporter.h"
#include <iostream>
#include <string>

using namespace selene;

void showHelp()
{
        std::cout << "status: showing help page" << std::endl << std::endl;
        std::cout << "NAME" << std::endl;
        std::cout << "        Exporter - SELENE Device mesh exporter" << std::endl << std::endl;
        std::cout << "SYNOPSIS" << std::endl;
        std::cout << "        Exporter -i input_file -o output_file" << std::endl;
        std::cout << "        Exporter -h" << std::endl << std::endl;
        std::cout << "DESCRIPTION" << std::endl;
        std::cout << "        Exporter converts intermediate mesh format (SDIF) to the ";
        std::cout << "SELENE Device mesh format (SDMF)." << std::endl << std::endl;
        std::cout << "OPTIONS" << std::endl;
        std::cout << "        -i, --input" << std::endl;
        std::cout << "                Specifies input file name." << std::endl;
        std::cout << "        -o, --output" << std::endl;
        std::cout << "                Specifies output file name. ";
        std::cout << "Input and output file names may be the same." << std::endl;
        std::cout << "        -h, --help" << std::endl;
        std::cout << "                Shows help." << std::endl << std::endl;
        std::cout << "EXIT STATUS" << std::endl;
        std::cout << "        0      Successful program execution." << std::endl;
        std::cout << "        1      Input file is broken or does not exist." << std::endl;
        std::cout << "        2      Could not create output file." << std::endl << std::endl;
        std::cout << "HISTORY" << std::endl;
        std::cout << "        2010 - Originally written by Nezametdinov E. Ildus." << std::endl;
        std::cout << "        (neil.log@gmail.com)." << std::endl;
        std::cout << "        2010 - present day: Author is still developing and maintaining ";
        std::cout << "SELENE Device and its exporter." << std::endl;
}

int main(int argc, char* args[])
{
        std::string inputFileName(""), outputFileName("");
        std::string* currentFileName = nullptr;

        std::cout << "SELENE Device exporter" << std::endl;

        for(int i = 0; i < argc; ++i)
        {
                std::string argument(args[i]);
                if(currentFileName != nullptr)
                {
                        *currentFileName = argument;
                        currentFileName = nullptr;
                        continue;
                }

                if(argument == "-i" || argument == "--input")
                {
                        currentFileName = &inputFileName;
                }
                else if(argument == "-o" || argument == "--output")
                {
                        currentFileName = &outputFileName;
                }
                else if(argument == "-h" || argument == "--help")
                {
                        showHelp();
                        return 0;
                }
        }

        if(inputFileName == "")
        {
                std::cout << "error: no input file name specified" << std::endl;
                showHelp();
                return 0;
        }

        if(outputFileName == "")
        {
                std::cout << "error: no output file name specified" << std::endl;
                showHelp();
                return 0;
        }

        RawMesh rawMesh;

        if(rawMesh.read(inputFileName.c_str()))
        {
                Exporter exporter;
                if(!exporter.processMesh(rawMesh, outputFileName.c_str()))
                        return 2;
        }
        else
                return 1;

        return 0;
}
