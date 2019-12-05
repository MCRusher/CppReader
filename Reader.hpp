#pragma once

#include "_reader.hpp"
#include <fstream>
#include <cstring>
#include <new>
#include <cstdint>

class Reader : public _reader {
protected:
    std::ifstream ifs;
    std::string name;
    int Look, Look2;
    std::uint64_t Line, Char;
public:
	///Opens the file with fname and initializes the reader object to a valid state.
	///Can fail, should be checked with IsOpen.
    Reader(char const * const fname)
    : ifs(fname), name(fname), Look(0), Look2(0), Line(1), Char(0) {
    	Look2 = ifs.get();
    }
    ///Opens the file with fname and initializes the reader object to a valid state.
    ///Can fail, should be checked with IsOpen.
    Reader(std::string const& fname)
    : ifs(fname), name(fname), Look(0), Look2(0), Line(1), Char(0) {
		Look2 = ifs.get();
    }
    ///Creates an uninitialized reader object which must be "Open"ed before use.
    Reader(){}
	///Opens the file with fname and initializes the reader object to a valid state.
    ///@Returns true if file was opened successfully and reader was initialized.
    bool Open(char const * fname)override{
		ifs.open(fname);
		if(ifs.is_open()){
			name = fname;
			Look = 0;
			Look2 = ifs.get();
			Line = 1;
			Char = 0;
			return true;
		}
		return false;
    }
    ///Opens the file with fname and initializes the reader object to a valid state.
    ///@Returns true if file was opened successfully and reader was initialized.
    bool Open(std::string const& fname)override{
		ifs.open(fname);
		if(ifs.is_open()){
			name = fname;
			Look = 0;
			Look2 = ifs.get();
			Line = 1;
			Char = 0;
			return true;
		}
		return false;
    }

	///@Returns true if the file has been opened,
	///which also means the reader is in a valid state.
    bool IsOpen()const override{
        return ifs.is_open();
    }

	///@Returns true if file has zero length, else returns false.
	///Should not be called on an uninitialized object.
	///Modifies internal state and cannot be called as const.
    bool IsEmpty()override{
    	//checks if position is zero, which is impossible for a valid Reader
    	//since Open&Constructors read a character
		return ifs.tellg()<=0 /*&& ifs.peek() == std::ifstream::traits_type::eof()*/;
    }

	///Returns the name of the file exactly as opened.
    std::string const& GetName()const override{
        return name;
    }

	///@Returns the line position (1-indexed) of the file.
    std::uint64_t GetLine()const override{
		return Line;
    }

	///@Returns the character position (1-indexed) of the file, \
				resets when line is changed.
    std::uint64_t GetChar()const override{
		return Char;
    }

	///@Returns the last read char in the stream.
	///Will be '\0' until Read is called for the first time.
	///Also returns '\0' if the value is out of char range.
    char Peek()const override{
        return Look < 0 ? '\0' : Look;
    }
	///@Returns the next character to be read in the stream.
	///Also returns '\0' if the value is out of char range.
    char PeekNext()const override{
        return Look2 < 0 ? '\0' : Look2;
    }

	///@Returns true if the stream has reached EOF.
    bool AtEOF()const override{ return Look==-1; }
	///@Returns true if the stream will reach EOF on the next call to Read.
    bool AtEOFNext()const override{ return Look2==-1; }
	///@Returns the next character in the stream and updates the PeekNext character, \
				or '\0' if the value is out of char range.
	///@Throws a protected EOFErr exception when called when |AtEOF()|, since this indicates an over-reading bug.
    char Read()override{
        //if both are EOF(-1) then over-reading will occur, which is likely to cause bugs in the program.
        if(Look==-1 && Look2==-1)
            throw EOFErr();
        Look = Look2;
        if(Look=='\n'){
            ++Line;
            Char = 0;
        }else ++Char;
        Look2 = ifs.get();
        return Look < 0 ? 0 : Look;
    }
};

