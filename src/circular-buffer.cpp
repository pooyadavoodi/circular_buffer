#include "circular-buffer.h"

namespace CircularBuffer
{

//make an instance of status with appropriate values set to code_ and msg_
status::status(Code code, std::string msg) : code_(code)
{
    std::stringstream s;
    s << "Status is " << codeString(code) << " --- " << msg;
    msg_ = s.str();
}

//return a string corresponding to code
std::string status::codeString(Code code)
{
    assert((code == kSuccess) ||
           (code == kAllocationFault) ||
           (code == kBufferFull) ||
            (code == kBufferEmpty) ||
           (code == kInputFileFault) ||
           (code == kQuit));
    switch(code)
    {
        case kSuccess:
            return "Success";
        case kAllocationFault:
            return "Allocation Fault";
        case kBufferFull:
            return "Buffer Full";
        case kBufferEmpty:
            return "Buffer Empty";
        case kInputFileFault:
            return "Input File Fault";
        case kQuit:
            return "Quit";
    }
    return "Undefined";//this line is never reached
}

//append e to buf
template <class T>
status circular_buffer<T>::append_element(const T & e)
{
    if(IsFull())
        return status::BufferFull("Buffer is full and cannot append any element");
    buf[end] = e;
    end = (end == buf.size()-1) ? (0) : (end+1);
    empty = false;
    return status::Success();
}

//remove an element from the beginning of buf
template <class T>
status circular_buffer<T>::remove_element()
{
    if(IsEmpty())
        return status::BufferEmpty("Buffer is empty and cannot remove any element");
    begin = (begin == buf.size()-1) ? (0) : (begin+1);
    empty = ((!empty) && (begin == end)) ? (true) : (false);
    return status::Success();
}

//initilialize buf with N elements of default values and setting other member variables accordingly
//return 1 for success
template <class T>
status circular_buffer<T>::init(size_t N)
{
    buf.insert(buf.end(), N, T());
    empty = true;
    begin = end = 0;
    return status::Success();
}

//append all the element of v into buf
//return 1 for success
template <class T>
status circular_buffer<T>::append(const std::vector<T> & v)
{
    for(size_t i = 0; i<v.size() ; i++)
    {
        status stat = this->append_element(v[i]);
        if(!stat.IsSuccess())
            return stat;
    }
    return status::Success();
}

//remove n elements from bug
//return 1 for success
template <class T>
status circular_buffer<T>::remove(size_t n)
{
    for(size_t i = 0; i<n ; i++)
    {
        status stat = this->remove_element();
        if(!stat.IsSuccess())
            return stat;
    }
    return status::Success();
}

//print all the elements of buf
template <class T>
status circular_buffer<T>::print()
{
    if(IsEmpty())//this is not an error
        return status::Success();
    //std::cout << "The buffer contains the elements:" << std::endl;
    for(size_t i = begin;
        ((i<buf.size()) && (i<end));
        i = (i == buf.size()-1) ? (0) : (i+1))
            std::cout << buf[i] << std::endl;
    return status::Success();
}

bool buffer_system::IsError(const status & stat)
{
    if(!stat.IsSuccess() && !stat.IsQuit())
    {
        std::cerr<<stat.toString();
        return true;
    }
    return false;
}

//read the next command from infile and put it in cmd and n. If cmd has no n, set n=-1
status buffer_system::get_next_command(std::istream & infile, command & cmd, int & n)
{
    std::string str_cmd;
    infile >> str_cmd;
    if(str_cmd.compare("A") == 0)
    {
        cmd = APP;
        infile >> n;
        return status::Success();
    }
    if(str_cmd.compare("R") == 0)
    {
        cmd = REM;
        infile >> n;
        return status::Success();
    }
    if(str_cmd.compare("L") == 0)
    {
        cmd = LIST;
        n = -1;
        return status::Success();
    }
    if(str_cmd.compare("Q") == 0)
    {
        cmd = QUIT;
        n = -1;
        return status::Success();
    }
    return status::InputFileFault("Cannot find a right command in input file");
}

//run cmd on buf: if cmd is append or remove then n matters, otherwise n is not used
//return 1 on success
status buffer_system::run_command(std::istream & infile, command cmd, size_t n)
{
    assert((cmd == APP) ||
            (cmd == REM) ||
           (cmd == LIST) ||
            (cmd == QUIT));
    if(cmd == APP)
    {
        std::vector<std::string> args;
        for(size_t i = 0; i<n ; i++)
        {
            std::string s;
            infile >> s;
            args.push_back(s);
        }
        return buf.append(args);
    }
    if(cmd == REM)
        return buf.remove(n);
    if(cmd == LIST)
        return buf.print();
    if(cmd == QUIT)
        return status::Quit();
    return status::Quit("This is a bug.");//this line is never reached
}

int buffer_system::run(std::istream & infile)
{
    status stat;//used to check for status returned by various functions

    //read the size of buffer
    size_t N;
    infile >> N;
    stat = buf.init(N);
    if(IsError(stat))
        return 1;

    //read and run commands
    command cmd;
    int n;
    do
    {
        stat = get_next_command(infile, cmd, n);
        if(IsError(stat))
            return 1;
        assert((cmd == APP) ||
            (cmd == REM) ||
            (cmd == LIST) ||
           (cmd == QUIT));
        stat = run_command(infile, cmd, n);
        if(IsError(stat))
            return 1;
    }while(cmd != QUIT);
    return 0;
}


//run the buffer commands from in_filename
//return 1 on success
int buffer_system::run(std::string in_filename)
{
    std::ifstream infile(in_filename);
    if(!infile.good())
    {
        std::cerr<<"Cannot open file "<<in_filename<<std::endl;
        return 1;
    }
    return run(infile);
}

}//end of namespace CircularBuffer










