#pragma once
#include <cstdlib>
#include <stdexcept>
#include <string>


class Matrices
{
public:
    Matrices(char const * input_file_name);
    Matrices(const Matrices & source);
    Matrices(double** matrix, const size_t row_count, const size_t column_count);
    Matrices& operator=(const Matrices & source);
    Matrices operator+(const Matrices& second_matrix) const;
    Matrices operator*(const Matrices& second_matrix) const;
    void read(char const* input_file_name);
    void print() const;
    ~Matrices();
    class MatricesException: public std::runtime_error
    {
    public:
        explicit MatricesException (const std::string& what_arg);
    };

private:
    size_t m_row_count;
    size_t m_column_count;
    double** m_matrix;
    void freeMemory();
    void allocMemory();
};

