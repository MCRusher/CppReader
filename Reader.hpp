#pragma once
#include "_reader.hpp"

#include <fstream>
#include <string>
#include <cstdint>
#include <sstream>

template <typename CharT = char>
class Reader : public reader_interface<CharT> {
protected:
    using rface = reader_interface<CharT>;
    using NameString = typename rface::NameString;
    using StringT = typename rface::StringT;
    using EOFErr = typename rface::EOFErr;
    static constexpr typename std::basic_ifstream<CharT>::int_type
        EndOfField = std::basic_ifstream<CharT>::traits_type::eof();


    std::basic_ifstream<CharT> ifs;
    NameString name;
    CharT Look;//undefined until first read
    typename std::basic_ifstream<CharT>::int_type Look2;
    //designed for extremes in either dimension
    std::uintmax_t Char, Line;
public:
    ///Copies filename and opens it.
    ///peek() is undefined until read() is called.
    ///@Returns true if file was opened successfully.
    bool open(NameString const& fname) override{
        ifs.open(fname);
        if(!ifs.is_open())
            return false;
        name = fname;
        Look2 = ifs.get();
        Line = 1;
        Char = 0;
        return true;
    }

    ///Copies filename and opens it, then reads one character.
    ///Fails if Reader.open() fails or if file was empty.
    ///The value of peek() is defined as the first CharT in the file.
    bool openRead(NameString const& fname) override{
        if(this->open(fname) && Look2 != EndOfField){
            this->read();
            return true;
        }else return false;
    }

    ///@Returns true if the file is open.
    bool isOpen()const override{
        return ifs.is_open();
    }

    ///@Returns true if no characters are left to read.
    bool atEOF()const override{
        return Look2 == EndOfField;
    }

    ///@Returns the filename as a string.
    NameString const& getName()const override{
        return name;
    }

    ///@Returns the current line position of the reader.
    std::uintmax_t getLine()const override{
        return Line;
    }

    ///@Retruns the current CharT position of the current line.
    std::uintmax_t getChar()const override{
        return Char;
    }

    ///@Returns the last read CharT value.
    CharT peek()const override{
        return Look;
    }

    ///@Returns the next CharT value to be read.
    ///@Fails if at EOF (no chars to peek)
    CharT peekNext()const override{
        if(Look2==EndOfField)
            throw EOFErr();
        return Look2;
    }

    ///@Returns the next CharT in the file
    ///@Fails if at EOF (no chars to read)
    CharT read() override{
        if(Look2 == EndOfField)
            throw EOFErr();
        Look = Look2;
        Look2 = ifs.get();
        if(StringT(1,Look)==StringT(1,'\n')){
            ++Line;
            Char = 0;
        }else ++Char;
        return Look;
    }

    ///@Returns a string of the next characters until delim or at EOF.
    ///String is empty if at EOF (no chars to read)
    StringT readUntil(CharT delim) override{
        std::basic_ostringstream<CharT> buf;
        while(Look2!=EndOfField){
            CharT c = this->read();
            if(c==delim) break;
            buf.put(c);
        }
        return buf.str();
    }
};
