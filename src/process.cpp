#include "process.hpp"
#include <optional>
#include <iostream>
#include <optional>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <algorithm>
#include <functional>

namespace project2
{
    /**
     * Creates a char** from std::vector<std::string>, and then calls exec
     */
    int Exec(std::vector<std::string> arguments_)
    {
        char **argv = new char *[arguments_.size() + 1];

        for (int i = 0; i < arguments_.size(); i++)
            argv[i] = arguments_[i].data();

        argv[arguments_.size()] = nullptr;

        // execvp(path_to_executable, { path_to_executable, args, NULL })
        execvp(argv[0], argv);

        // If perror is called, it means that execvp has failed
        perror("exec error");
        
        // for(int i = 0; i < arguments_.size(); i++) {
        //     if(argv[i] != nullptr) 
        //         std::cout << "ARG " << i << " " << argv[i] << std::endl;
        //     std::cout << "ARG " << i << " " << "nullptr" << std::endl;
        // }

        return 1;
    }

    /**
     * C++ Wouldn't play nicely with Exec being in a lambda, so a custom Exec() is called
     */
    ChildProcessLambda ExecLambda(std::string execPath, std::vector<std::string> args)
    {
        // Adds { execPath, ...args }
        args.insert(args.begin(), execPath);

        // { execPath, ...args, NULL }
        args.push_back(nullptr);

        /**
         * The Lambda Function that will be called when process.Run() is called
         */
        return [args](auto process) {
            Exec(args);

            // This return should never be reached
            return 1;
        };
    }

    /**
     * Creates a ChildProcess of Lambda type
     */
    ChildProcess::ChildProcess(std::function<int(ChildProcess *childProcess)> functionToCall)
    {
        m_Type = ChildProcessType::eLambda;

        m_Lambda = functionToCall;
    }

    /**
     * Creates a ChildProcess of Exec type
     */
    ChildProcess::ChildProcess(std::string toExecute, std::vector<std::string> args) // : ChildProcess(ExecLambda(toExecute, args))
    {
        m_Type = ChildProcessType::eExec;

        m_ToExecute = toExecute;
        m_ToExecuteArgs = args;

        /**
         * Update as of 3rd of December:
         * 
         * Due to a bug that was found, AFTER the program was running correctly
         * 
         * ExecLambda() and Exec() do not work, thus, the entire code of ExecLambda and Exec have been put
         * in `m_lambda`
         */

        m_Lambda = [this](auto process) {
            // Adds { execPath, ...args }
            std::vector<std::string> finalArgs;
            finalArgs.push_back(this->m_ToExecute);
            for(auto s : this->m_ToExecuteArgs) 
                finalArgs.push_back(s);

            /**
             * The Lambda Function that will be called when process.Run() is called
             */
            char **argv = new char *[finalArgs.size() + 1];

            for (int i = 0; i < finalArgs.size(); i++)
                argv[i] = finalArgs[i].data();

            argv[finalArgs.size()] = nullptr;

            // execvp(path_to_executable, { path_to_executable, args, NULL })
            /** argv = [ "/bin/ls", "-la", NULL ]; */
            execvp(argv[0], argv);

            // If perror is called, it means that execvp has failed
            perror("exec error");
            return 1;
        };
    }

    bool ChildProcess::IsChild() const
    {
        return m_IsChild;
    }

    bool ChildProcess::IsParent() const
    {
        return !m_IsChild;
    }

    void ChildProcess::Run()
    {
        switch (m_Type)
        {
        case ChildProcessType::eExec:
        case ChildProcessType::eLambda:
            return RunLambda();
        }
    }

    std::optional<bool> ChildProcess::Fork()
    {
        pid_t pid = fork();

        /** Fork Error Checking */
            if (pid < 0)
            {
                std::cerr << "Could not fork. Fork exited with error code: " << pid << std::endl;
                perror("fork error");
                return {};
        }

        // Indicates that the subprocess has started
        m_HasStarted = true;

        if (pid > 0)
        {

            /**
             * Parent
             */

            m_IsChild = false;
            m_Id = pid;

            /**
             * Pipes
             */
            close(m_ChildWrite.WriteFD());

            close(m_ParentWrite.ReadFD());
        }
        else
        {
            /**
             * Child 
             * 
             * pid = 0
             */

            m_IsChild = true;
            m_Id = getpid();

            /**
             * Pipes
             */

            /**
             * If Child AND ChildProcess type == Exec: 
             *  Pipe stdout to the pipe
             */
            if (m_Type == ChildProcessType::eExec)
            {
                dup2(m_ChildWrite.WriteFD(), STDOUT_FILENO);
                close(m_ChildWrite.WriteFD());
                close(m_ChildWrite.ReadFD());
                close(m_ParentWrite.WriteFD());
            }
            /**
             * Just normal pipes
             */
            else
            {
                close(m_ChildWrite.ReadFD());
                close(m_ParentWrite.WriteFD());
            }
        }

        return IsChild();
    }

    PipeStream &ChildProcess::ParentToChildPipe()
    {
        return m_ParentWrite;
    }

    PipeStream &ChildProcess::ChildToParentPipe()
    {
        return m_ChildWrite;
    }

    void ChildProcess::RunLambda()
    {
        auto forked = Fork();

        /** If fork error */
        if (!forked.has_value())
        {
            std::cerr << "Could not start Start Lambda: err on fork" << std::endl;
            exit(1);
        }

        /** Parent */
        if (!forked.value())
        {
            return;
        }

        /** Child */
        exit(m_Lambda(this));
    }

    std::optional<ChildProcessStatus> ChildProcess::Wait()
    {
        /** 
         * You can't wait for the child to finish from the child itself, duh?
         */
        if (IsChild())
        {
            return {};
        }

        if (m_Id < 1)
        {
            return {};
        }

        /** from waitpid status to ChildProcessStatus struct */
        int status;

        ChildProcessStatus processStatus;

        do
        {
            auto _wpid = waitpid(m_Id, &status, WUNTRACED | WCONTINUED);

            if (_wpid == -1)
            {
                perror("Error waiting for PID (waitpid)");
                return {};
            }

        } while (!WIFEXITED(status) && !WIFSIGNALED(status));

        processStatus.status = status;
        processStatus.continued = WIFCONTINUED(status);
        processStatus.exited = WIFEXITED(status);
        processStatus.signaled = WIFSIGNALED(status);
        processStatus.stopped = WIFSTOPPED(status);
        processStatus.returnValue = WEXITSTATUS(status);

        m_ProcessStatus = processStatus;

        m_DoneRunning = true;

        return m_ProcessStatus;
    }

    std::optional<ChildProcessStatus> ChildProcess::RunAndWait()
    {
        Run();
        return Wait();
    }

    std::string ChildProcess::ReadString(int size)
    {
        if (IsChild())
        {
            return m_ParentWrite.ReadString(size);
        }
        else
        {
            return m_ChildWrite.ReadString(size);
        }
    }

    void ChildProcess::WriteString(std::string string)
    {
        if (IsChild())
        {
            m_ChildWrite.WriteString(string);
        }
        else
        {
            m_ParentWrite.WriteString(string);
        }
    }
} // namespace project2