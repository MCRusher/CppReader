#pragma once

#include <fstream>
#include <cstring>
#include <new>
#include <cstdint>
#include <cstddef>

class _reader {
protected:
    ///meant to help diagnose over-reading of the stream, not to be checked for, use AtEOF or AtEOFNext for that.
    class EOFErr : std::exception {};
public:
	///Opens the file with fname and initializes the reader object to a valid state.
    ///@Returns true if file was opened successfully and reader was initialized.
    virtual bool Open(char const * fname) = 0;
    ///Opens the file with fname and initializes the reader object to a valid state.
    ///@Returns true if file was opened successfully and reader was initialized.
    virtual bool Open(std::string const& fname) = 0;

	///@Returns true if stream is open and can be read from.
	virtual bool IsOpen()const = 0;

	///@Returns true if a stream is empty, else returns false.
	///Should not be called on an uninitialized object.
	///May modify internal state and cannot be called as const.
    virtual bool IsEmpty() = 0;
	///Returns the name of the stream exactly as opened.
    virtual std::string const& GetName()const = 0;

    ///Gets the current line position (1-indexed) of the stream
    virtual std::uint64_t GetLine()const = 0;

	///Gets the current character position (1-indexed) of the stream
	///resets every time the line changes
    virtual std::uint64_t GetChar()const = 0;

	///@Returns the last read char in the stream.
	///Will be '\0' until Read is called for the first time.
	///Also returns '\0' if the value is out of char range.
    virtual char Peek()const = 0;
	///@Returns the next character to be read in the stream.
	///Also returns '\0' if the value is out of char range.
    virtual char PeekNext()const = 0;

	///@Returns true if the stream has reached EOF.
    virtual bool AtEOF()const = 0;
	///@Returns true if the stream will reach EOF on the next call to Read.
    virtual bool AtEOFNext()const = 0;
	///@Returns the next character in the stream and updates the PeekNext character.
	///Also returns '\0' if the value is out of char range.
	///@Throws a private EOFErr exception when called with AtEOF==true, since this indicates an over-reading bug.
    virtual char Read() = 0;
};
