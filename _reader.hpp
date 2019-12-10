#pragma once

#include <fstream>
#include <cstring>
#include <new>
#include <cstdint>
#include <cstddef>

template <typename CharTI/*, typename NameCT*/>
class reader_interface {
protected:
    //meant to help diagnose over-reading of the stream, not to be checked for, use AtEOF or AtEOFNext for that.
    class EOFErr : std::exception {};
    typedef /*std::basic_string<NameCT>*/ std::string NameString;
    typedef std::basic_string<CharTI> StringT;
public:
    //Copies filename and opens it.
    //peek() is undefined until read() is called.
    //@Returns true if file was opened successfully.
    virtual bool open(NameString const& fname) = 0;

    virtual bool openRead(NameString const& fname) = 0;

	//@Returns true if stream is open and can be read from.
	virtual bool isOpen()const = 0;

	//Returns the name of the stream exactly as opened.
    virtual NameString const& getName()const = 0;

    //Gets the current line position (1-indexed) of the stream
    virtual std::uintmax_t getLine()const = 0;

	//Gets the current character position (1-indexed) of the stream
	//resets every time the line changes
    virtual std::uintmax_t getChar()const = 0;

	//@Returns the last read char in the stream.
	//Value is undefined until Read is called for the first time
    virtual CharTI peek()const = 0;
	//@Returns the next character to be read in the stream.
    //@Throws protected EOFErr exception on debug if no characters are left to peek.
    virtual CharTI peekNext()const = 0;

	//@Returns true if the stream has reached EOF.
    virtual bool atEOF()const = 0;
	//@Returns the next character in the stream and updates the PeekNext character.
	//Also returns '\0' if the value is out of char range.
	//@Throws a protected EOFErr exception on debug when at EOF.
    virtual CharTI read() = 0;

    virtual std::basic_string<CharTI> readUntil(CharTI delim) = 0;
};

