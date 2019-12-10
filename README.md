# CppReader
Two Readers conforming to a _reader interface, created for a 1 char look ahead lexer system

Both files have Doc comments to explain how each function works for each implementation.

Reader - A single file reader, tracks Line and Char position for error reports.

StackedReader - Intended for C-style import systems where new files can be spliced in the middle. ~~Unfortunately cannot be used to split individual tokens in half, though this is not useful nor is it good practice.~~
* it was useful, and now supports it.
- Reads whole file, then discards it, if another file was underneath it, continues the read.

~~Update: It's a pretty bad version of the implementation, and has some bugs.~~

Update2: It's a much better version of the implementation, and should be a lot more straightforward and less buggy.
- The main concern for bugs is in the StackedReader implementation, which is much more complex.
