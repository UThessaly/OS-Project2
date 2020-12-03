#include <string>
#include <functional>


namespace project2 {
        
    enum class PipeStreamType {
        eRead = 0,
        eWrite = 1,
    };

    class PipeStreamCreateError : public std::runtime_error {
        public:
            PipeStreamCreateError(std::string const& message) : std::runtime_error(message) {};
    };

    class PipeStream {
        public:
            /**
             * Pipes use the `pipe()` syscall. 
             * 
             * None of the pipes is closed by default
             */
            PipeStream();

            /** Write a string to the pipe */
            void WriteString(std::string string);  

            /** Write anything to the pipe */  
            // void Write(std::any value);

            /** Read a string from the pipe */        
            std::string ReadString(int size = -1);

            /** Read anything from the pipe */
            // std::any Read();

            /** Returns the Write File Descriptor */
            int WriteFD();

            /** Returns the Read File Descriptor */
            int ReadFD();
        
        private:
            int pipefd[2];
    };
}