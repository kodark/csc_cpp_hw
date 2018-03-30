#pragma once
#include <string>
#include <vector>
#include <exception>
#include <cstdint>

class HuffmanCode{
public:
    HuffmanCode();
    ~HuffmanCode();
    void compress(const std::string& inputFile, const std::string& outputFile);
    void unpack(const std::string& inputFile, const std::string& outputFile);
    class HuffmanCodeException: public std::exception
    {
    public:
        HuffmanCodeException(const char* text);
        virtual const char* what() const throw();
    private:
        const char* m_text;
    };
private:
    struct CharNode
    {
        uint8_t m_char;
        uint64_t m_frequency;
        CharNode* zero;
        CharNode* one;
        CharNode(uint8_t newChar, uint64_t frequency, CharNode* zero = nullptr, CharNode* one = nullptr);
        void toTable(std::string* codeTable, const std::string & code) const;
        void recoveryByNode(size_t & index, std::vector<char> & m_bits, std::ostream& output) const;
        ~CharNode();
    };

    struct CompCharNodePointer
    {
        bool operator()(CharNode* first, CharNode* second)
        {
            return (first->m_frequency > second->m_frequency);
        }
    };
    std::uint64_t m_charFrequency[256];
    CharNode* m_headCharNode;
    std::string m_codeTable[256];
    std::uint64_t m_charCount;
    std::vector<char> m_bits;
    uint64_t m_bitsCount;
    uint64_t m_sizeOfAdditionalInfo;
    void countCharFrequency(std::istream& input);
    void calculateCodeTable();
    void writeCodeTable(std::istream& input, std::ostream& output);
    void readCodeTable(std::istream& input);
    void clear();
    void writeDecompressedFile(std::ostream& output);
};
