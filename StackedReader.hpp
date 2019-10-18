#pragma once

#include "_reader.hpp"
#include <fstream>
#include <cstring>
#include <new>
#include <cstdint>
#include <vector>

class StackedReader : public _reader {
protected:
	struct File {
		std::ifstream ifs;
		std::string name;
		int Look, Look2;
		std::uint64_t Line, Char;
		File(std::string const& fname)
		: ifs(fname), name(fname), Look(0), Look2(0), Line(1), Char(0) {
			Look2 = ifs.get();
		}
		File(char const * const fname)
		: ifs(fname), name(fname), Look(0), Look2(0), Line(1), Char(0) {
			Look2 = ifs.get();
		}
	};
	std::vector<File> files;
public:
	StackedReader(std::string const& fname){ files.emplace_back(fname); }
	StackedReader(char const * const fname){ files.emplace_back(fname); }
    StackedReader(){}

    ///@Returns true if file was opened and made the current file\
				false if file is already opened or if file could not be opened.
    bool Open(char const * fname)override{
		for(auto const& f : files){
			if(f.name == fname){
				return false;
			}
		}
		files.emplace_back(fname);
		return files.back().ifs.is_open();
    }
    ///@Returns true if file was opened and made the current file\
				false if file is already opened or if file could not be opened.
    bool Open(std::string const& fname)override{
		for(auto const& f : files){
			if(f.name == fname){
				return false;
			}
		}
		files.emplace_back(fname);
		return files.back().ifs.is_open();
    }

    ///@Returns true if the current has been opened successfully,\
				false if no files are open or open failed.
    bool IsOpen()const override{
        return files.size() > 0 && files.back().ifs.is_open();
    }

    ///@Returns Number of files currently in the stack
	std::size_t FileCount()const{
		static_assert(sizeof(std::vector<File>::size_type)<=sizeof(std::size_t));
		return files.size();
	}

	///@Returns true if current file has zero length,\
				false if |FileCount() == 0| or file is not empty
    bool IsEmpty()override{
    	//checks if position is zero, which is impossible for a valid Reader
    	//since Open&Constructors read a character
		return files.back().ifs.tellg()<=0 /*&& ifs.peek() == std::ifstream::traits_type::eof()*/;
    }

    ///Returns the name of the current file exactly as opened.
    ///should only be called when |FileCount() > 0 && IsOpen()|
    std::string const& GetName()const override{
        return files.back().name;
    }

    ///@Returns the line position (1-indexed) of the current file.
    std::uint64_t GetLine()const override{
		return files.back().Line;
    }

	///@Returns the character position (1-indexed) of the current file, \
				resets when line is changed.
    std::uint64_t GetChar()const override{
		return files.back().Char;
    }

	///@Returns the last read character in the current,\
				will be '\0' until Read is called for the first time \
				on the current file, or if the value is out of char range.
    char Peek()const override{
        return files.back().Look < 0 ? '\0' : files.back().Look;
    }

    ///@Returns the next character to be read in the current file, \
				or '\0' if the value is out of char range.
    char PeekNext()const override{
    	std::uint64_t tLook2;
    	/*if(files.back().Look2==-1 && files.size()>1)
			 tLook2 = files.at(files.size()-2).Look2;
		else*/
			tLook2 = files.back().Look2;
        return tLook2 < 0 ? '\0' : tLook2;
    }
    ///@Returns true if |FileCount() == 1| and the last file has reached EOF.
    bool AtEOF()const override{ return files.size() == 1 && files.back().Look==-1; }
	///@Returns true if |FileCount() == 1| and the last file will reach EOF on the next call to Read.
    bool AtEOFNext()const override{ return files.size() == 1 && files.back().Look2==-1; }
    ///@Returns the next character in the current file and updates the PeekNext character.
	///Also returns '\0' if the value is out of char range.
	///@Note will discard current file if |FileCount > 1| and it hits EOF, and return another call to itself. \
			 Should not be called with |FileCount()==0|.
	///@Throws a protected EOFErr exception when called with AtEOF==true, since this indicates an over-reading bug.
    char Read()override{
        if(files.back().Look==-1 && files.back().Look2==-1)
            throw EOFErr();
		//if the next character to be read is EOF (so file is now empty),
		//discard current file unless it is the only file left.
		else if(files.back().Look2==-1 && files.size()>1)
			files.pop_back();
        files.back().Look = files.back().Look2;
        if(files.back().Look=='\n'){
            ++files.back().Line;
            files.back().Char = 0;
        }else ++files.back().Char;
        files.back().Look2 = files.back().ifs.get();
        return files.back().Look < 0 ? '\0' : files.back().Look;
    }
};
