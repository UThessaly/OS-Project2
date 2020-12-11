#include "pipe.hpp"

#include <vector>
#include <string>

#include <sys/types.h> 
#include <unistd.h>

namespace project2 {
    PipeStream::PipeStream() {
        /**
         * Creates the Pipes
         * 
         * pipefd[0];
         * pipefd[1];
         */
        if(pipe(pipefd) < 0) {
            throw PipeStreamCreateError("Could not create pipe");
        }
    }

    int PipeStream::ReadFD() {
        /**
         * Returns pipefd[0] 
         */
        return pipefd[static_cast<int>(PipeStreamType::eRead)];
    }
    
    int PipeStream::WriteFD() {
        /**
         * Returns pipefd[1] 
         */
        return pipefd[static_cast<int>(PipeStreamType::eWrite)];
    }
    

    void PipeStream::WriteString(std::string string) {
        /**
         * string.c_str()
         * 
         * This is used for getting the string buffer (char*) 
         * as well as appending null terminator ('\0') at the end 
         * 
         * Example:
         * C++: "Hello"
         * C  : "Hello\0"
        */
        write(WriteFD(), string.c_str(), string.size() + 1);
    }

    std::string PipeStream::ReadString(const int size) { 
        /**
         * If size < 0, read  until '\0' or nothing else to read
         */
        if(size < 0) {
            std::vector<char> input;

            char currentChar[1];
            int readlen;
            while((readlen = read(ReadFD(), currentChar, 1)) > 0 && currentChar[0] != '\0') {
                input.push_back(currentChar[0]);
            }

            return std::string(input.begin(), input.end());
        }
        /**
         * Else just read as many bytes as requested 
         */
        char array[size];
        read(ReadFD(), array, size);
        return std::string(array);
    }
}