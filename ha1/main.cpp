#include <iostream>
#include <string>
#include <cstring>
#include <algorithm>
#include <map>
#include <cstdio>
#include "matrices.hpp"


using namespace std;


int main(int argc, char ** argv)
{
    try
    {
        if (argc % 2)
            throw Matrices::MatricesException("Invalid number commands in cmd");

        Matrices matrix(argv[1]);

        Matrices second_matrix = matrix;
        for (int i = 2; i < argc; i+=2)
        {            
            std::string const add_op = "--add";
            std::string const mult_op = "--mult";
            
            second_matrix.read(argv[i+1]);
            if (argv[i] != add_op && argv[i] != mult_op)
                throw Matrices::MatricesException("Invalid command in cmd!");
            if (argv[i] == add_op)
            {
                matrix = matrix + second_matrix;
            }
            else
            {
                matrix = matrix * second_matrix;
            }
            
        }
        matrix.print();
    } catch (Matrices::MatricesException const & matrixError)
    {
        cerr << matrixError.what() << endl;
        return 5;
    }
    
    return 0;
}
