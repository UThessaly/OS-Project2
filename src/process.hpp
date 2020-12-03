#include <functional>
#include <string>
#include <vector>

#include <sys/types.h> 
#include <unistd.h> 

#include "pipe.hpp"

namespace project2 {    

    enum class ChildProcessType {
        eLambda,
        eExec
    };

    struct ChildProcessStatus { 
        int status;
        bool exited;
        bool signaled;
        bool stopped;
        bool continued;

        int returnValue;
    };

    class ChildProcess {
        public:
            /** This will create a child process and run the Lambda */
            ChildProcess(std::function<int(ChildProcess* childProcess)> functionToCall);

            /** This will create a child process and run exec with the given arguments */
            ChildProcess(std::string toExecute, std::vector<std::string> args = {});

            /** This will start the actual child process */
            void Run();

            /** This will wait for the child process to finish */
            std::optional<ChildProcessStatus> Wait();

            /** This will just run Run() and Wait() right after */ 
            std::optional<ChildProcessStatus> RunAndWait();

            bool IsChild() const;
            bool IsParent() const;
            
            /** Write a string to the pipe. Writes to parentWrite if parent and childWrite if child */
            void WriteString(std::string string);  

            /** Read a string from the pipe. Reads from childWrite if parent and parentWrite if child */        
            std::string ReadString(int size = -1);

            /**
             * Returns the pipe that the parent can use to write to the child 
             */
            PipeStream& ParentToChildPipe();

            /**
             * Returns the pipe that the child can use to write to the parent
             */
            PipeStream& ChildToParentPipe();

        private:
            PipeStream m_ParentWrite, m_ChildWrite;

            /** 
             * This is set to true when the fork happens
             */
            bool m_HasStarted = false;
            
            /** 
             * False when fork happens
             * 
             * True when the process is completed
             */
            bool m_DoneRunning = false;

            /**
             * The process status once it the process has exited
             */
            ChildProcessStatus m_ProcessStatus;


            /** True when the process is a child process */
            bool m_IsChild = false;

            /**  
             * The Process ID. Used by parent processes
             */
            pid_t m_Id;

            /** The type of the child process */
            ChildProcessType m_Type;

            /** Type Specific Variables */
            //
            std::function<int(ChildProcess* childProcess)> m_Lambda;
            std::string m_ToExecute;
            std::vector<std::string> m_ToExecuteArgs = {};

            /** 
             * This does the actual forking 
             * 
             * @returns true - Child Process
             * @returns false - Parent Process
             */
            std::optional<bool> Fork();
            
            /** This will run the Lambda function */
            void RunLambda();

            /** This will run Exec */ 
            void RunExec();
    };

    typedef std::function<int(ChildProcess* childProcess)> ChildProcessLambda;
    // ChildProcessLambda ExecLambda(std::string execPath, std::vector<std::string> args = {});
}