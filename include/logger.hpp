#ifndef __LOGGER_HPP__
#define __LOGGER_HPP__

#include <iostream>
#include <string>
#include <chrono>

namespace InvertedIndex {

using namespace std::chrono;

class ILogger {
 public:
  ~ILogger() {}
  virtual void Log(const std::string&) = 0;
};

class Logger : public ILogger {
 public:
  Logger() {}
 
  void Log(const std::string& msg) {
    static auto then = system_clock::now();
    size_t ms = duration_cast<milliseconds>(
      system_clock::now() - then
    ).count();
   
    std::cout << "[" << ms << "] " << msg << std::endl; 
  }

};

} // End of namespace.

#endif
