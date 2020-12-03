#include <iostream>
#include "process.hpp"
#include <unistd.h>
#include <vector>
#include <fstream>
#include <string>
#include <future>
#include <map>

int main(int argc, char **argv)
{
    std::string inputFile;
    if (argc != 1)
    {
        inputFile = argv[0];
    }
    else
    {
        std::cout << "Please enter path to file of words [press \"-\" for '../nonsense_words.txt']: ";
        std::cin >> inputFile;

        if (inputFile == "-")
            inputFile = "../nonsense_words.txt";
    }

    // inputFile = "nonsense_words.txt";

    std::vector<std::string> words;

    std::ifstream wordsFile(inputFile);

    if (!wordsFile)
    {
        std::cout << "Error opening file " << inputFile << std::endl;
        exit(EXIT_FAILURE);
    }

    /**
     * These brackets are here to keep the std::string word, only for the scope of the while loop
     * and then to later on destroy it
     */
    {
        std::string word;
        while (std::getline(wordsFile, word))
            words.push_back(word);
    }

    // The results map
    std::map<std::string, std::string> results;

    /**
     * Holds the futures, since futures are destroyed if they are out of scope. 
     * 
     * Example:
     * 
     * {
     *     std::async([]() {
     *          callMyFunc();
     *     });
     * }
     * printf("Hello");
     * 
     * When the code reaches the printf, the std::async will destroy the lambda function if execution has not 
     * started yet. Async functions are only valid for a given scope. Thus a "global" variable is
     * created for the futures, to extend the scopes of the async functions
     */
    std::vector<std::future<std::string>> futures;

    /**
     * The mutex. std::lock_guard<std::mutex> guard(results_mutex) 
     * 
     * This locks the mutex, and automatically releases it when out of scope
     * 
     * {
     *      std::lock_guard<std::mutex> myGuardName(myMutexName);
     * }
     * 
     * printf("Hello, World");
     * 
     * When code reaches the printf, the mutex will be automagically unlocked
     * 
     * This happens because the std::lock_guard has a destructor, which is automagically
     * called when the scope ends that unlocks the mutex
     */
    std::mutex results_mutex;

    // Loop through all the words in the Input File (nonsense_words.txt)
    for (const auto &word : words)
    {
        // std::cout << "for " << word << std::endl;
        /**
         * futures.push_back: As mentioned above, keep the async function from destroying the lambda
         * 
         * std::async(std::launch::async, myLambda);
         *             ^                      ^
         *             |                     [outside Arguments]() { return a string because std::future<string> }
         *             starts the myLambda function, on another thread
         */
        futures.push_back(std::async(std::launch::async, [&results, &results_mutex, &word]() {
            /** 
             * The Child Process that will call the Exec
             * 
             * ChildProcess(command, { args }); 
             * 
             * This automagically pipes the output of that program through subprocess.ReadString();
             */
            auto subprocess = project2::ChildProcess("./find_correct_word", {"../dictionary.txt", word});
            // Starts the Subprocess
            subprocess.Run();

            // Reads the output from the piped subprocess
            std::string output = subprocess.ReadString();

            // Waits for the subprocess to complete execution
            subprocess.Wait();

            {
                // Locks the mutex
                std::lock_guard<std::mutex> guard(results_mutex);

                // Adds the program's output, to the map of the results
                results[word] = output;
            }
            /**
             * Not really needed, but it's just cool to do
             * 
             * future.wait()
             * future.get()
             */
            // return output;
            return output;
        }));
    }

    // Waits for the threads to finish
    for (auto &future : futures)
    {
        future.wait();
    }

    /**
     * Basically:
     * 
     * for (key, value from results):
     *  print key, value
     */
    for (const auto &[word, result] : results)
    {
        std::cout << word << " " << result << std::endl;
    }

    return 0;
}
