#include "matrices.hpp"
#include <fstream>
#include <iostream>

using namespace std;

Matrices::Matrices(char const * input_file_name)
{
    std::ifstream input_file(input_file_name);
    if (!input_file)
        throw MatricesException("Cannot open input file");

    input_file >> m_row_count >> m_column_count;
    allocMemory();
    for (size_t i = 0; i < m_row_count; ++i)
    {
        for (size_t j = 0; j < m_column_count; ++j)
        {
            input_file >> m_matrix[i][j];
        }
    }
}

Matrices::Matrices(const Matrices & source)
    : m_row_count(source.m_row_count)
    , m_column_count(source.m_column_count)
{
    allocMemory();
    for (size_t i = 0; i < m_row_count; ++i)
    {
        for (size_t j = 0; j < m_column_count; ++j)
        {
            m_matrix[i][j] = source.m_matrix[i][j];
        }
    }
}

Matrices::Matrices(double** matrix, size_t  row_count, size_t column_count)
{
    m_matrix = matrix;
    m_row_count = row_count;
    m_column_count = column_count;
}

Matrices& Matrices::operator=(const Matrices & source)
{
    if (this == &source)
        return *this;

    freeMemory();
    
    m_row_count = source.m_row_count;
    m_column_count = source.m_column_count;
    allocMemory();
    for (size_t i = 0; i < m_row_count; ++i)
    {
        for (size_t j = 0; j < m_column_count; ++j)
        {
            m_matrix[i][j] = source.m_matrix[i][j];
        }
    }
    return *this;
}

Matrices Matrices::operator+(const Matrices& second_matrix) const
{
    if (m_row_count != second_matrix.m_row_count || m_column_count != second_matrix.m_column_count)
        throw MatricesException("Dimensions are invalid");
    Matrices sum_matrix = *this;
    for (size_t i = 0; i < sum_matrix.m_row_count; ++i)
    {
        for (size_t j = 0; j < sum_matrix.m_column_count; ++j)
        {
            sum_matrix.m_matrix[i][j] += second_matrix.m_matrix[i][j];
        }
    }
    return sum_matrix;
}

Matrices Matrices::operator*(const Matrices& second_matrix) const
{
    if (this->m_column_count != second_matrix.m_row_count)
        throw MatricesException("Dimensions are invalid");
        
    double** prod_matrix = new double* [this->m_row_count];
    for (size_t i = 0; i < this->m_row_count; ++i)
    {    
        prod_matrix[i] = new double [second_matrix.m_column_count];
        for (size_t j = 0; j < second_matrix.m_column_count; ++j)
        {
            double sum = 0;
            for (size_t k = 0; k < this->m_column_count; ++k)
            {
                sum += this->m_matrix[i][k] * second_matrix.m_matrix[k][j];
            }
            prod_matrix[i][j] = sum;
        }
    }

    Matrices prod_result_object(prod_matrix, this->m_row_count, second_matrix.m_column_count);
    
    return prod_result_object;
}

void Matrices::read(char const * input_file_name)
{
    freeMemory();    
    
    std::ifstream input_file(input_file_name);
    if (!input_file)
        throw MatricesException("File cannot open");

    input_file >> m_row_count >> m_column_count;
    allocMemory();
    for (size_t i = 0; i < m_row_count; ++i)
    {
        for (size_t j = 0; j < m_column_count; ++j)
        {
            input_file >> m_matrix[i][j];
        }
    }
}

void Matrices::print() const
{
    cout << m_row_count << ' ' << m_column_count << endl;
    for (size_t i = 0; i < m_row_count; ++i)
    {
        for (size_t j = 0; j < m_column_count; ++j)
        {
            cout << m_matrix[i][j] << ' ';
        }
        cout << endl;
    }
}

Matrices::~Matrices()
{
    freeMemory();
}

void Matrices::freeMemory()
{
    for (size_t i = 0; i < m_row_count; ++i)
    {
        delete [] m_matrix[i];
    }
    delete [] m_matrix;
    
    m_matrix = nullptr;
    m_row_count = 0;
    m_column_count = 0;
}

void Matrices::allocMemory()
{
    m_matrix = new double* [m_row_count];
    for (size_t i = 0; i < m_row_count; ++i)
    {
        m_matrix[i] = new double [m_column_count];
    }
}

Matrices::MatricesException::MatricesException(const std::string& what_arg)
        :std::runtime_error(what_arg)
{    }
