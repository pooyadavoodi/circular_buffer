#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <assert.h>

namespace CircularBuffer
{

class status
{
private:
    enum Code
    {
        kSuccess = 0,
        kAllocationFault = 1,
        kBufferFull = 2,
        kBufferEmpty = 3,
        kInputFileFault = 4,
        kQuit = 5
    };
    Code code_;
    std::string msg_;

    //make an instance of status with appropriate values set to code_ and msg_
    status(Code code, std::string msg);

    //return a string corresponding to code
    std::string codeString(Code code);

public:
    status() : code_(kSuccess), msg_(codeString(code_)) {}

    static status Success(std::string msg = std::string())
    {
        return status(kSuccess,msg);
    }
    static status AllocationFault(std::string msg = std::string())
    {
        return status(kAllocationFault,msg);
    }
    static status BufferFull(std::string msg = std::string())
    {
        return status(kBufferFull,msg);
    }
    static status BufferEmpty(std::string msg = std::string())
    {
        return status(kBufferEmpty,msg);
    }
    static status InputFileFault(std::string msg = std::string())
    {
        return status(kInputFileFault,msg);
    }
    static status Quit(std::string msg = std::string())
    {
        return status(kQuit,msg);
    }

    bool IsSuccess() const { return code_ == kSuccess; }
    bool IsAllocationFault() const { return code_ == kAllocationFault; }
    bool IsBufferFull() const { return code_ == kBufferFull; }
    bool IsBufferEmpty() const { return code_ == kBufferEmpty; }
    bool IsInputFileFault() const { return code_ == kInputFileFault; }
    bool IsQuit() const { return code_ == kQuit; }

    //return msg_
    std::string toString() const { return msg_; }

};//end of class status

template <class T>
class circular_buffer
{
private:
    std::vector<T> buf;
    bool empty;
    size_t begin;
    size_t end;

    //append e to buf
    status append_element(const T & e);
    //remove an element from the beginning of buf
    status remove_element();
public:
    circular_buffer() : empty(true), begin(0), end(0) {}
    ~circular_buffer() {}

    //return true if buf is full
    bool IsFull() { return ((!empty) && (begin == end)); }
    //return true if buf is empty
    bool IsEmpty() { return empty; }

    //initilialize buf with N elements of default values and setting other member variables accordingly
    //return 1 for success
    status init(size_t N);

    //append all the element of v into buf
    //return 1 for success
    status append(const std::vector<T> & v);

    //remove n elements from bug
    //return 1 for success
    status remove(size_t n);

    //print all the elements of buf
    status print();
};//end of class circular_buffer

class buffer_system
{
private:
    circular_buffer<std::string> buf;
    enum command{APP, REM, LIST, QUIT};//append, remove, print, quit
    bool IsError(const status & stat);

    //read the next command from infile and put it in cmd and n. If cmd has no n, set n=-1
    status get_next_command(std::istream & infile, command & cmd, int & n);

    //run cmd on buf: if cmd is append or remove then n matters, otherwise n is not used
    //return 1 on success
    status run_command(std::istream & infile, command cmd, size_t n = 0);
public:
    buffer_system() {}
    ~buffer_system() {}

    int run(std::istream & infile);

    //run the buffer commands from in_filename
    //return 1 on success
    int run(std::string in_filename);

};//end of class buffer_system

}//end of namespace CircularBuffer



#endif // CIRCULAR_BUFFER_H







