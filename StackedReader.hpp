#pragma once
#include "_reader.hpp"

#include <fstream>
#include <string>
#include <cstdint>
#include <sstream>
#include <vector>
#include <exception>

template <typename CharT = char>
class StackedReader : public reader_interface<CharT> {
protected:
    using rface = reader_interface<CharT>;
    using NameString = typename rface::NameString;
    using StringT = typename rface::StringT;
    using EOFErr = typename rface::EOFErr;
    static constexpr typename std::basic_ifstream<CharT>::int_type
        EndOfField = std::basic_ifstream<CharT>::traits_type::eof();

    class EmptyReaderStackErr : public std::exception {};

    struct File {
        std::basic_ifstream<CharT> ifs;
        NameString name;
        CharT Look;
        typename std::basic_ifstream<CharT>::int_type Look2;
        //designed for extremes in either dimension
        std::uintmax_t Char, Line;
    };
    std::vector<File> fs;
public:
    ///Copies filename, opens it, and adds it to the stack.
    ///peek() is undefined until read() is called.
    ///@Returns true if file was opened successfully.
    bool open(NameString const& fname) override{
        fs.push_back(File());
        auto& back = fs.back();
        back.ifs.open(fname);
        if(!back.ifs.is_open())
            return false;
        back.name = fname;
        back.Look2 = back.ifs.get();
        back.Line = 1;
        back.Char = 0;
        return true;
    }

    ///Copies filename and opens it, then reads one character.
    ///@Returns false if Reader.open() fails or if file was empty.
    ///The value of peek() is defined as the first CharT in the file if operation succeeds.
    bool openRead(NameString const& fname) override{
        if(this->open(fname) && fs.back().Look2 != EndOfField){
            this->read();
            return true;
        }else return false;
    }

    ///@Returns true if the file is open.
    bool isOpen()const override{
        return !fs.empty() && fs.back().ifs.is_open();
    }

    ///@Returns true if no characters are left to read.
    bool atEOF()const override{
        return fs.size()==1 && fs[0].Look2 == EndOfField;
    }

    ///@Returns the filename as a string.
    NameString const& getName()const override{
        if(fs.empty())
            throw EmptyReaderStackErr();
        return fs.back().name;
    }

    ///@Returns the current line position of the reader.
    std::uintmax_t getLine()const override{
        if(fs.empty())
            throw EmptyReaderStackErr();
        return fs.back().Line;
    }

    ///@Retruns the current CharT position of the current line.
    std::uintmax_t getChar()const override{
        if(fs.empty())
            throw EmptyReaderStackErr();
        return fs.back().Char;
    }

    ///@Returns the last read CharT value.
    CharT peek()const override{
        if(fs.empty())
            throw EmptyReaderStackErr();
        return fs.back().Look;
    }

    ///@Returns the next CharT value to be read.
    ///@Fails if at EOF (no chars to peek)
    CharT peekNext()const override{
        if(fs.size()==1 && fs[0].Look2==EndOfField)
            throw EOFErr();
        return fs.back().Look2;
    }

    ///@Returns the next CharT in the file
    ///@Fails if at EOF (no chars to read)
    CharT read() override{
        if(fs.empty())
            throw EmptyReaderStackErr();
        if(fs.size()>1 && fs.back().Look2 == EndOfField)
            fs.pop_back();
        if(fs.size()==1 && fs.back().Look2 == EndOfField)
            throw EOFErr();
        auto& back = fs.back();
        back.Look = back.Look2;
        back.Look2 = back.ifs.get();
        if(StringT(1,back.Look)==StringT(1,'\n')){
            ++back.Line;
            back.Char = 0;
        }else ++back.Char;
        return back.Look;
    }

    ///@Returns a string of the next characters until delim or at EOF.
    ///String is empty if at EOF (no chars to read)
    StringT readUntil(CharT delim) override{
        std::basic_ostringstream<CharT> buf;
        if(fs.empty())
            throw EmptyReaderStackErr();
        while(!this->atEOF()){
            CharT c = this->read();
            if(c==delim) break;
            buf.put(c);
        }
        return buf.str();
    }
};

