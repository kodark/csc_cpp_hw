#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <queue>
#include <vector>
#include <cstring>
#include "huffman.hpp"

using namespace std;

static int const BITS_IN_BYTE = 8;

static int const SIZE_OF_ARRAY = 256;

    HuffmanCode::HuffmanCode()
        :m_headCharNode(0)
    {
        clear();
    }

    HuffmanCode::~HuffmanCode()
    {
        clear();
    }

    void HuffmanCode::compress(const std::string& inputFile, const std::string& outputFile)
    {
        clear();        
        std::ifstream input(inputFile, std::ifstream::binary);
        if (!input)
        {
            throw HuffmanCodeException("Cannot open input file");
        }
        std::ofstream output(outputFile, std::ofstream::binary);
    
        countCharFrequency(input);
        calculateCodeTable();
        writeCodeTable(input, output);
    }
    
    void HuffmanCode::unpack(const std::string& inputFile, const std::string& outputFile)
    {
        clear();
        std::ifstream input(inputFile, std::ifstream::binary);
        if (!input)
            throw HuffmanCodeException("Cannot open input file");
        std::ofstream output(outputFile, std::ofstream::binary);

        readCodeTable(input);
        writeDecompressedFile(output);
    }

    void HuffmanCode::countCharFrequency(std::istream& input)
    {
        input.seekg (0, input.end);
        unsigned length = (unsigned) input.tellg();
        input.seekg (0, input.beg);
        std::vector<char> buffer(length);
        input.read (buffer.data(),length);

        for (size_t i = 0; i < length; ++i)
        {
            uint8_t index = buffer[i];
            ++m_charFrequency[index];
        }

        for (size_t i = 0; i != SIZE_OF_ARRAY; ++i)
        {
            if (m_charFrequency[i] != 0)
            {
                ++m_charCount;
            }
        }
    }

    void HuffmanCode::calculateCodeTable()
    {
        std::priority_queue<CharNode*, std::vector<CharNode*>, CompCharNodePointer> myQueue;
        for (size_t i = 0; i != SIZE_OF_ARRAY; ++i)
        {
            CharNode* newCharNode;
            if (m_charFrequency[i] != 0)
            {
                newCharNode = new CharNode(i, m_charFrequency[i]);
                myQueue.push(newCharNode);
            }
        }

        while (myQueue.size() > 2)
        {
            CharNode* firstCharNode = myQueue.top();
            myQueue.pop();
            CharNode* secondCharNode = myQueue.top();
            myQueue.pop();

            CharNode* newCharNode;
            newCharNode = new CharNode(0, firstCharNode->m_frequency + secondCharNode->m_frequency, firstCharNode, secondCharNode);
            myQueue.push(newCharNode);
        }

        if (myQueue.size() == 2)
        {
            CharNode* firstCharNode = myQueue.top();
            myQueue.pop();
            CharNode* secondCharNode = myQueue.top();
            myQueue.pop();

            CharNode* newCharNode;
            newCharNode = new CharNode(0, firstCharNode->m_frequency + secondCharNode->m_frequency, firstCharNode, secondCharNode);
            m_headCharNode = newCharNode;

            m_headCharNode->zero->toTable(m_codeTable, "0");
            m_headCharNode->one->toTable(m_codeTable, "1");
        }
        else
        if (myQueue.size() == 1)
        {
            CharNode* firstCharNode = myQueue.top();
            myQueue.pop();
            m_headCharNode = firstCharNode;
            m_headCharNode->toTable(m_codeTable, "0");
        }
    }

    void HuffmanCode::writeCodeTable(std::istream& input, std::ostream& output)
    {
        m_sizeOfAdditionalInfo = 0;
		std::uint64_t m_charCountuint = (std::uint64_t)m_charCount;
        output.write((char*)&m_charCountuint, sizeof(std::uint64_t));
        m_sizeOfAdditionalInfo += sizeof(std::uint64_t);

        if (m_charCount != 0)
        {
            for (size_t i = 0; i < SIZE_OF_ARRAY; ++i)
                if (m_charFrequency[i] != 0)
                {
                    output << (uint8_t) i;
                    output.write((char*)&m_charFrequency[i], sizeof(std::uint64_t));
                    m_sizeOfAdditionalInfo += sizeof(uint8_t) + sizeof(std::uint64_t);
                }
        

            input.clear();
            {
                input.seekg (0, input.end);
                unsigned length = (unsigned) input.tellg();
                input.seekg (0, input.beg);

                std::vector<char> ReadingBuffer(length);
                input.read (ReadingBuffer.data(),length);

                std::vector<char> buffer(length * BITS_IN_BYTE, 0);
 //               memset(buffer.data(), 0, length * BITS_IN_BYTE);
                size_t allBitsCount = 0;
                size_t bitBufferPointer = 0;
                size_t charBufferPointer = 0;

                for (size_t i = 0; i < length; ++i)
                {
                    uint8_t index = ReadingBuffer[i];
                    size_t sizeOfString = m_codeTable[index].size();
                    allBitsCount += sizeOfString;

                    for(size_t jString = 0; jString < sizeOfString; ++jString)
                    {
                        buffer[charBufferPointer] += m_codeTable[index][jString] - '0';
                        ++bitBufferPointer;
                        if (bitBufferPointer % BITS_IN_BYTE == 0)
                            ++charBufferPointer;
                        else
                            buffer[charBufferPointer]<<=1;
                    }
                }

                ++bitBufferPointer;
                while (bitBufferPointer % BITS_IN_BYTE != 0)
                {
                    buffer[charBufferPointer]<<=1;
                    ++bitBufferPointer;
                }

                output.write((char*)&allBitsCount, sizeof(std::uint64_t));
                m_sizeOfAdditionalInfo += sizeof(std::uint64_t);
                size_t sizeOfBuffer = (allBitsCount / BITS_IN_BYTE) + ((allBitsCount % BITS_IN_BYTE) != 0);
                output.write(buffer.data(), sizeOfBuffer);

                cout << length << endl << sizeOfBuffer << endl << m_sizeOfAdditionalInfo << endl;
            }
        }
        else
            cout << 0 << endl << 0 << endl << m_sizeOfAdditionalInfo << endl;
    }

    void HuffmanCode::readCodeTable(std::istream& input)
    {
        m_sizeOfAdditionalInfo = 0;
        input.seekg (0, input.end);
        unsigned length = (unsigned) input.tellg();
        input.seekg (0, input.beg);
        std::vector<char> buffer(length);
        input.read (buffer.data(),length);
        m_charCount = *((std::uint64_t*)buffer.data());
        m_sizeOfAdditionalInfo += sizeof(std::uint64_t);
        if (m_charCount != 0)
        {
            for (size_t i = 0; i < m_charCount; ++i)
            {
                size_t index = (sizeof(uint8_t) + sizeof(std::uint64_t)) * i;
                if (index + sizeof(uint8_t) + sizeof(std::uint64_t) >= length)
                {
                    throw HuffmanCodeException("Incorrect input file");
                }
                m_charFrequency[(uint8_t)buffer[index + sizeof(std::uint64_t)]] = *((std::uint64_t*)(buffer.data() + index + sizeof(uint8_t) + sizeof(std::uint64_t)));
                int n = 0;
            }
            m_sizeOfAdditionalInfo += m_charCount * (sizeof(std::uint64_t) + sizeof(uint8_t));

            calculateCodeTable();

            size_t index = (sizeof(uint8_t) + sizeof(std::uint64_t)) * m_charCount + sizeof(std::uint64_t);

            if (index + sizeof(std::uint64_t) >= length)
                {
                    throw HuffmanCodeException("Incorrect input file");
                }
            m_bitsCount = *((std::uint64_t*)(buffer.data() + index));
            m_sizeOfAdditionalInfo += sizeof(std::uint64_t);
            index += sizeof(std::uint64_t);


            size_t byteCount = m_bitsCount / BITS_IN_BYTE + (m_bitsCount % BITS_IN_BYTE != 0);
            m_bits.resize(byteCount);

            if (index + byteCount > length)
                {
                    throw HuffmanCodeException("Incorrect input file");
                }
            memcpy(m_bits.data(), &buffer[index], byteCount);
        }
    }

    void HuffmanCode::clear()
    {
        m_bits.clear();
        delete m_headCharNode;
        m_charCount = 0;
        m_headCharNode = 0;
        m_sizeOfAdditionalInfo = 0;
        for (size_t i = 0; i != SIZE_OF_ARRAY; ++i)
        {
            m_charFrequency[i] = 0;
            m_codeTable[i].clear();
        }
    }

    void HuffmanCode::writeDecompressedFile(std::ostream& output)
    {
        size_t sizeOfDecompressedFile = 0;
        if (m_charCount != 0)
        {
            size_t index = 0;
            while (index < m_bitsCount)
            {
                if (m_headCharNode == 0)
                {
                    throw HuffmanCodeException("Incorrect input file");
                }
                m_headCharNode->recoveryByNode(index, m_bits, output);
                ++sizeOfDecompressedFile;
                if (m_charCount == 1)
                    ++index;
            }
            
            m_bits.clear();
            cout << m_bitsCount / BITS_IN_BYTE + (m_bitsCount % BITS_IN_BYTE != 0) << endl <<
            sizeOfDecompressedFile << endl << m_sizeOfAdditionalInfo << endl;
        }
        else
            cout << 0 << endl << 0 << endl << sizeof(std::uint64_t) << endl;
        

    }

    void HuffmanCode::CharNode::toTable(std::string* codeTable, const std::string& code) const
    {
        if (this->one == 0 || this->zero == 0)
            codeTable[this->m_char] = code;
        if (this->one != 0)
            this->one->toTable(codeTable, code + "1");
        if (this->zero != 0)
            this->zero->toTable(codeTable, code + "0");
    }

    void HuffmanCode::CharNode::recoveryByNode(size_t & index, std::vector<char> & m_bits, std::ostream& output) const
    {
        
        if (this->one == 0 && this->zero == 0)
        {
            char tempChar = this->m_char;
            output.write(&tempChar, 1);
            return;
        }
        char tempChar = 1 << (BITS_IN_BYTE - 1 - index % BITS_IN_BYTE);
        if ((m_bits[index / BITS_IN_BYTE] & tempChar ) != tempChar)
        {
            if (this->zero != 0)
            {
                this->zero->recoveryByNode(++index, m_bits, output);
            }
            else
            {
                throw HuffmanCodeException("Incorrect input file");
            }            
        }
        else
        {
            if (this->one != 0)
            {
                this->one->recoveryByNode(++index, m_bits, output);
            }
            else
            {
                throw HuffmanCodeException("Incorrect input file");
            }
        }

    }

    HuffmanCode::CharNode::~CharNode()
    {
        delete zero;
        delete one;
    }
    
    HuffmanCode::CharNode::CharNode(uint8_t newChar, std::uint64_t frequency, CharNode* zero, CharNode* one)
		:m_char(newChar)
		,m_frequency(frequency)
		,zero(zero)
		,one(one)
    {	}

    HuffmanCode::HuffmanCodeException::HuffmanCodeException(const char* text)
        :m_text(text)
    {    }

    const char* HuffmanCode::HuffmanCodeException::what() const throw()
    {
        return m_text;
    }
