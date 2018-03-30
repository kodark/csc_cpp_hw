#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include "huffman.hpp"



int main(int argc, char* argv[])
{
	std::ios_base::sync_with_stdio(0);
    try{
        HuffmanCode s;
        if (argc != 6)
        {
            throw HuffmanCode::HuffmanCodeException("Incorrect number of arguments in command line!");
        }

        bool isCompressing = false;
        std::string const COMPRESS_FLAG("-c"), UNCOMPRESS_FLAG("-u");
        if (argv[1] == COMPRESS_FLAG)
        {
			isCompressing = true;
		} else
		{
			if (argv[1] == UNCOMPRESS_FLAG)
			{
				isCompressing = false;
			} else
			{
				throw HuffmanCode::HuffmanCodeException("Incorrect first argument in command line!");
			}
			
		}

        std::string inputFile = "";
        std::string outputFile = "";
		std::string const INPUT_FILE_LONG_FLAG("--file"), INPUT_FILE_SHORT_FLAG("-f"), OUTPUT_FILE_LONG_FLAG("--output"), OUTPUT_FILE_SHORT_FLAG("-o");
        for (size_t i = 2; i < 5; ++i)
        {
			if (argv[i] == INPUT_FILE_SHORT_FLAG || argv[i] == INPUT_FILE_LONG_FLAG)
			{
				if (inputFile.size() != 0)
				{
					throw HuffmanCode::HuffmanCodeException("Incorrect input file arguments in command line!");
				}
				++i;
				inputFile = argv[i];
			}
			else 
			{
				if (argv[i] == OUTPUT_FILE_LONG_FLAG || argv[i] == OUTPUT_FILE_SHORT_FLAG)
				{
					if (outputFile.size() != 0)
					{
						throw HuffmanCode::HuffmanCodeException("Incorrect output file arguments in command line!");
					}
					++i;
					outputFile = argv[i];
				}
				else
				{
					throw HuffmanCode::HuffmanCodeException("Incorrect input and output file arguments in command line!");
				}
			} 
        }    

        if (isCompressing)
            s.compress(inputFile, outputFile);
        else
            s.unpack(inputFile, outputFile);

    } catch (const HuffmanCode::HuffmanCodeException &currentHuffmanCodeException)
    {
        std::cerr << "HuffmanCodeException: " << currentHuffmanCodeException.what();
        return -1;
    }
    
    return 0;
}
