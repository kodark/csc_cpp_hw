#ifndef LAZY_STRING_HPP
#define LAZY_STRING_HPP

#include <memory>
#include <string>
#include <cctype>

namespace std_utils
{

    template<class charT, class traits = std::char_traits<charT> >
    class lazy_basic_string
    {
        typedef traits traits_type;
        typedef typename traits_type::char_type value_type;
        typedef value_type& reference;
        typedef const value_type& const_reference;
        typedef value_type* pointer;
        typedef const value_type* const_pointer;

    public:
        lazy_basic_string()
            : m_buffer(std::make_shared<CommonString>(0)) { }

        lazy_basic_string(const lazy_basic_string& other)
            : m_buffer(other.m_buffer) { }

        lazy_basic_string(const charT* c_str)
            : m_buffer(std::make_shared<CommonString>(traits::length(c_str)))
        {
            if (m_buffer->m_length > 0)
            {
                traits::copy(m_buffer->m_string, c_str, m_buffer->m_length);
            }
        }

        lazy_basic_string(charT current_char)
            : m_buffer(std::make_shared<CommonString>(1))
        {
            traits::assign(m_buffer->m_string, 1, current_char);
        }

        lazy_basic_string(size_t size, charT current_char)
            : m_buffer(std::make_shared<CommonString>(size))
        {
            traits::assign(m_buffer->m_string, size, current_char);
        }

        ~lazy_basic_string() { }

        lazy_basic_string& operator=(lazy_basic_string other)
        {
            swap(other);
            return *this;
        }

        lazy_basic_string& operator+=(const lazy_basic_string& other)
        {
            size_t other_size = other.m_buffer->m_length;

            if (other_size > 0)
            {
                std::shared_ptr<CommonString> new_buffer = std::make_shared<CommonString>(m_buffer->m_length + other_size);
                if (m_buffer->m_length > 0)
                {
                    traits::copy(new_buffer->m_string, m_buffer->m_string, m_buffer->m_length);
                }

                traits::copy(new_buffer->m_string + m_buffer->m_length, other.m_buffer->m_string, other_size);
                m_buffer = new_buffer;
            }

            return *this;
        }

    private: class Proxy;

    public:
        Proxy operator[](size_t index)
        {
            return Proxy(index, this);
        }

        charT operator[](size_t index) const
        {
            return m_buffer->m_string[index];
        }

        void swap(lazy_basic_string& other)
        {
            std::swap(m_buffer, other.m_buffer);
        }

        size_t size() const noexcept
        {
            return m_buffer->m_length;
        }

        const charT* c_str() const noexcept
        {
            return m_buffer->m_string;
        }

        bool empty() const noexcept
        {
            return (m_buffer->m_length == 0);
        }

        void clear()
        {
            lazy_basic_string clear_string = lazy_basic_string();
            swap(clear_string);
        }

    private:
        class CommonString
        {
        public:
            CommonString(size_t length)
                : m_length(length)
            {
                m_string = new charT[m_length + 1];
                traits::assign(m_string + m_length, 1, charT());
            }

            CommonString(const CommonString & other)
                : m_length(other.m_length)
            {
                m_string = new charT[m_length + 1];
                traits::assign(m_string + m_length, 1, charT());
            }

            CommonString(CommonString && other)
                : m_string(other.m_string)
                , m_length(other.m_length)
            {
                    other.m_length = 0;
                    other.m_string = new charT[1];
                    traits::assign(other.m_string, 1, charT());
            }

            void swap(CommonString& other)
            {
                std::swap(m_string, other.m_string);
                std::swap(m_length, other.m_length);
            }

            CommonString& operator=(CommonString other)
            {
                swap(other);
                return *this;
            }

            CommonString& operator=(CommonString&& other)
            {
                delete [] m_string;
                m_length = other.m_length;
                other.m_length = 0;
                m_string = other.m_string;
                other.m_string = new charT[1];
                traits::assign(other.m_string, 1, charT());
            }

            ~CommonString()
            {
                delete [] m_string;
            }

        private:
            charT* m_string;
            size_t m_length;

            friend class lazy_basic_string<charT, traits>;
        };

        std::shared_ptr<CommonString> m_buffer;

        void make_unique_if_need()
        {
            if (!m_buffer.unique()) {
                std::shared_ptr<CommonString> new_buffer = std::make_shared<CommonString>(m_buffer->m_length);
                traits::copy(new_buffer->m_string, m_buffer->m_string, m_buffer->m_length);
                m_buffer = new_buffer;
            }
        }

        class Proxy
        {
        public:
            operator charT()
            {
                return str->c_str()[index];
            }

            Proxy& operator=(const charT & ch)
            {
                str->make_unique_if_need();
                str->m_buffer->m_string[index] = ch;
                return *this;
            }

        private:
            friend class lazy_basic_string;
            size_t index;
            lazy_basic_string* str;

            Proxy(size_t index, lazy_basic_string* str)
                : index(index)
                , str(str) {}

            Proxy(const Proxy& other)
                : index(other.index)
                , str(other.str) {}
        };
    };

    struct CaseIndependentCharTraits : public std::char_traits<char>
    {
        static bool eq(char char_first, char char_second)
        {
             return std::toupper(char_first) == std::toupper(char_second);
        }

        static bool lt(char char_first, char char_second)
        {
             return std::toupper(char_first) <  std::toupper(char_second);
        }

        static int compare(const char* string_first, const char* string_second, size_t size)
        {
            while (size-- != 0)
            {
                if (std::toupper(*string_first) < std::toupper(*string_second))
                {
                    return -1;
                }
                if (std::toupper(*string_first) > std::toupper(*string_second))
                {
                    return 1;
                }

                ++string_first; ++string_second;
            }
            return 0;
        }

        static const char* find(const char* s, int size, char a)
        {
            while ((size-- > 0) && (std::toupper(*s) != std::toupper(a)))
            {
                ++s;
            }

            return s;
        }
    };


    typedef std_utils::lazy_basic_string<char> lazy_string;
    typedef std_utils::lazy_basic_string<wchar_t> lazy_wstring;
    typedef std_utils::lazy_basic_string<char, CaseIndependentCharTraits> lazy_istring;


    template<class charT, class traits = std::char_traits<charT> >
    std_utils::lazy_basic_string<charT, traits> operator+(const std_utils::lazy_basic_string<charT, traits> & first_string, const std_utils::lazy_basic_string<charT, traits> & second_string)
    {
        std_utils::lazy_basic_string<charT, traits> temp(first_string);
        temp += second_string;
        return temp;
    }

    template<class charT, class traits = std::char_traits<charT> >
    bool operator<(const std_utils::lazy_basic_string<charT, traits> & first_string, const std_utils::lazy_basic_string<charT, traits> & second_string)
    {
        size_t first_size = first_string.size();
        size_t second_size = second_string.size();
        size_t min_size = std::min(first_size, second_size);

        int cmp = traits::compare(first_string.c_str(), second_string.c_str(), min_size);
        if (cmp)
        {
            return cmp < 0;
        }
        else
        {
            return first_size < second_size;
        }
    }

    template<class charT, class traits = std::char_traits<charT> >
    bool operator==(const std_utils::lazy_basic_string<charT, traits> & first_string, const std_utils::lazy_basic_string<charT, traits> & second_string)
    {
        size_t first_size = first_string.size();
        size_t second_size = second_string.size();
        return !((first_size != second_size) || traits::compare(first_string.c_str(), second_string.c_str(), second_size));
    }

    template<class charT, class traits = std::char_traits<charT> >
    bool operator>(const std_utils::lazy_basic_string<charT, traits> & first_string, const std_utils::lazy_basic_string<charT, traits> & second_string)
    {
        return !(first_string <= second_string);
    }

    template<class charT, class traits = std::char_traits<charT> >
    bool operator<=(const std_utils::lazy_basic_string<charT, traits> & first_string, const std_utils::lazy_basic_string<charT, traits> & second_string)
    {
        return ((first_string == second_string) || (first_string < second_string));
    }

    template<class charT, class traits = std::char_traits<charT> >
    bool operator>=(const std_utils::lazy_basic_string<charT, traits> & first_string, const std_utils::lazy_basic_string<charT, traits> & second_string)
    {
        return !(first_string < second_string);
    }

    template<class charT, class traits = std::char_traits<charT> >
    bool operator!=(const std_utils::lazy_basic_string<charT, traits> & first_string, const std_utils::lazy_basic_string<charT, traits> & second_string)
    {
        return !(first_string == second_string);
    }

    template<class charT, class traits = std::char_traits<charT> >
    void swap(std_utils::lazy_basic_string<charT, traits> & first_string, std_utils::lazy_basic_string<charT, traits> & second_string)
    {
        first_string.swap(second_string);
    }

    template<class charT, class traits = std::char_traits<charT> >
    std_utils::lazy_basic_string<charT, traits> operator+(const charT* first_c_string, const std_utils::lazy_basic_string<charT, traits> & second_string)
    {
        std_utils::lazy_basic_string<charT, traits> temp(first_c_string);
        temp += second_string;
        return temp;
    }

    template<class charT, class traits = std::char_traits<charT> >
    std_utils::lazy_basic_string<charT, traits> operator+(const std_utils::lazy_basic_string<charT, traits> & first_string, const charT* second_c_string)
    {
        std_utils::lazy_basic_string<charT, traits> second_string(second_c_string);
        std_utils::lazy_basic_string<charT, traits> temp(first_string);
        temp += second_string;
        return temp;
    }

    template<class charT, class traits = std::char_traits<charT> >
    std_utils::lazy_basic_string<charT, traits> operator+(const charT first_char_string, const std_utils::lazy_basic_string<charT, traits> & second_string)
    {
        std_utils::lazy_basic_string<charT, traits> temp(first_char_string);
        temp += second_string;
        return temp;
    }

    template<class charT, class traits = std::char_traits<charT> >
    std_utils::lazy_basic_string<charT, traits> operator+(const std_utils::lazy_basic_string<charT, traits> & first_string, const charT second_char_string)
    {
        std_utils::lazy_basic_string<charT, traits> second_string(second_char_string);
        std_utils::lazy_basic_string<charT, traits> temp(first_string);
        temp += second_string;
        return temp;
    }

    template<class charT, class traits = std::char_traits<charT> >
    bool operator==(const charT* first_c_string, const std_utils::lazy_basic_string<charT, traits> & second_string)
    {
        std_utils::lazy_basic_string<charT, traits> first_string(first_c_string);
        return first_string == second_string;
    }

    template<class charT, class traits = std::char_traits<charT> >
    bool operator==(const std_utils::lazy_basic_string<charT, traits> & first_string, const charT* second_c_string) {
        std_utils::lazy_basic_string<charT, traits> second_string(second_c_string);
        return first_string == second_string;
    }

    template<class charT, class traits = std::char_traits<charT> >
    bool operator!=(const charT* first_c_string, const std_utils::lazy_basic_string<charT, traits> & second_string)
    {
        std_utils::lazy_basic_string<charT, traits> first_string(first_c_string);
        return first_string != second_string;
    }

    template<class charT, class traits = std::char_traits<charT> >
    bool operator!=(const std_utils::lazy_basic_string<charT, traits> & first_string, const charT* second_c_string)
    {
        std_utils::lazy_basic_string<charT, traits> second_string(second_c_string);
        return first_string != second_string;
    }

    template<class charT, class traits = std::char_traits<charT> >
    bool operator<(const charT* first_c_string, const std_utils::lazy_basic_string<charT, traits> & second_string)
    {
        std_utils::lazy_basic_string<charT, traits> first_string(first_c_string);
        return first_string < second_string;
    }

    template<class charT, class traits = std::char_traits<charT> >
    bool operator<(const std_utils::lazy_basic_string<charT, traits> & first_string, const charT* second_c_string)
    {
        std_utils::lazy_basic_string<charT, traits> second_string(second_c_string);
        return first_string < second_string;
    }

    template<class charT, class traits = std::char_traits<charT> >
    bool operator>(const charT* first_c_string, const std_utils::lazy_basic_string<charT, traits> & second_string)
    {
        std_utils::lazy_basic_string<charT, traits> first_string(first_c_string);
        return first_string > second_string;
    }

    template<class charT, class traits = std::char_traits<charT> >
    bool operator>(const std_utils::lazy_basic_string<charT, traits> & first_string, const charT* second_c_string)
    {
        std_utils::lazy_basic_string<charT, traits> second_string(second_c_string);
        return first_string > second_string;
    }

    template<class charT, class traits = std::char_traits<charT> >
    bool operator<=(const charT* first_c_string, const std_utils::lazy_basic_string<charT, traits> & second_string)
    {
        std_utils::lazy_basic_string<charT, traits> first_string(first_c_string);
        return first_string <= second_string;
    }

    template<class charT, class traits = std::char_traits<charT> >
    bool operator<=(const std_utils::lazy_basic_string<charT, traits> & first_string, const charT* second_c_string)
    {
        std_utils::lazy_basic_string<charT, traits> second_string(second_c_string);
        return first_string <= second_string;
    }

    template<class charT, class traits = std::char_traits<charT> >
    bool operator>=(const charT* first_c_string, const std_utils::lazy_basic_string<charT, traits> & second_string)
    {
        std_utils::lazy_basic_string<charT, traits> first_string(first_c_string);
        return first_string >= second_string;
    }

    template<class charT, class traits = std::char_traits<charT> >
    bool operator>=(const std_utils::lazy_basic_string<charT, traits> & first_string, const charT* second_c_string)
    {
        std_utils::lazy_basic_string<charT, traits> second_string(second_c_string);
        return first_string >= second_string;
    }


} // std_utils

#endif // LAZY_STRING_HPP

