# **Project 2**

2117083
2116085

**Please use a Markdown Viewer such as the one [Here](https://dillinger.io/) or simply go to 
the GitHub Repository [Here](https://github.com/UThessaly/OS-Project2)**

## **Contents**

- [**Project 2**](#project-2)
  - [**Contents**](#contents)
  - [**What we've completed**](#what-weve-completed)
  - [**Compilation**](#compilation)
  - [**C++17**](#c17)
    - [**Optional**](#optional)
    - [**Maps**](#maps)
  - [**Future**](#future)
  - [**How the Library Works**](#how-the-library-works)
    - [**Lambdas**](#lambdas)
    - [**Exec**](#exec)

## **What we've completed**

We've completed everything. The entire project was completed in C++, using the C++17 standard. 

We decided to use the Async functions from `#include <future>`. 

We've also decided to use Input File Streams (`ifstream`) from `#include <fstream>`

In addition to that, instead of `char**`, we're using `map` from `#include <map>`, as well as instead of `int[]`, we're using `vector<int>` and `array<int, 5>` if length is already known. 

## **Compilation**

In order to compile, we need some linux programs. 

```bash
$ sudo apt-get install cmake make gcc g++ build-essentials # Not sure for build-essentials 
$ cd <project-path>

$ mkdir build

# Builds the application in build/ directory
$ cd build && cmake .. && make

$ chmox +x ./Project2
$ chmod +x ./find_correct_word

$ ./Project2 <dictionary> # ./Project2 without args, will ask for file in runtime

```

***Warning***: The `nonsense_words.txt` should be in the `./build` Directory **OR** 
the full path must be given

## **C++17**

The C++17 Standard has added some extremely useful features such as

```cpp
#include <optional>
#include <filesystem>
```

And many more. 

### **Optional**

We're mainly using `<optional>` from the C++17 Standard, as it allows us to return a value if there's no error, and return nothing if there is an error.

```cpp
#include <optional>

std::optional<int> div(int x, int y) {
    if(y == 0) 
        return {} // Returns an empty object
    
    return x / y;
}
```

### **Maps**

Another main reason for using C++17, is for their use of maps, and array iterators.

In other languages, we have to do something along the lines of

```java
HashMap<int, string> map = new HashMap();

for(int key : map.keys()) {
    string value = map.get(key);
    // ...
}
```

But in C++17, we can simply do

```cpp
std::map<int, std::string> map;

for(auto& [key, value] : map) {
    // ...
}
```


## **Future**

The future library adds Asynchronous capabilities to a program, without having to create an entire library around Threads and PThread in POSIX. 

Instead of having to do 

```cpp
pthread_create(...);

pthread_exit(...);

pthread_join(...);
```

This can be done instead

```cpp
#include <future>

auto asyncThread = std::async(std::launch::async, []() { // std::async = pthread_create
    return "anything not just numbers"; // better pthread_exit
})

asyncThread.join(); // pthread_wait

asyncThread.get(); // gets value from pthread_exit
```

Because we have to store the `async` threads somewhere, in order to be able to `join` them, we can use a Vector.

```cpp
#include <vector>
#include <future>

// Creates `times` (5) threads
template<typename T>
auto myFunc(int times) {
    // This will hold all the futures
    std::vector<std::future<T>> futures;

    for(int i = 0; i < times; i++) 
        futures.push_back( // basically, creates new array element, 
                        // and pushes back the following value
            std::async(std::launch::async, threadFuncOrLambda);
        );

    return futures;
}

void anotherFunc() {
    auto futures = myFunc<std::string>(100);

    for(auto& f : futures) {
        f.join();
        f.get(); // Value of f, which in this case is a string
    }
}
```

## **How the Library Works**

The library that we've created gives us the ability to create new processes, and then call either a function on them OR create a completely new process such as `/bin/ls`

### **Lambdas** 

One of the ways you can use the library, is through Lambdas.

```cpp
project2::ChildProcess child([](project2::ChildProcess createdChildProcess) {
    // Basically, the int main() for the subprocess
    createdChildProcess.WriteString("Hello, World from Child");

    std::cout << "Child: Hi!" << std::endl; // Child: Hi!

    return 0;
}); 

child.Run();

auto str = child.ReadString();
std::cout << "Parent: " << str << std::endl; // Parent: Hello, World from Child

child.Wait();

```

### **Exec**

Exec can also be used with the library

```cpp
// type name(constructor);
// So std::string myString("hello") is the same as 
// std::string myString = std::string("hello")
project2::ChildProcess child("./find_correct_word", { "./dictionary.txt", "lleho" /* hello */ });

child.Run();

auto result = child.ReadString();

std::cout << "Result for lleho is: " << result << std::endl;
```