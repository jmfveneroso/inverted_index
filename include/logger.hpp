#ifndef __LOGGER_HPP__
#define __LOGGER_HPP__

#include <iostream>
#include <string>
#include <chrono>

namespace TP1 {

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
    size_t ms = duration_cast<milliseconds>(
      system_clock::now().time_since_epoch()
    ).count();
   
    std::cout << "[" << ms << "] " << msg << std::endl; 
  }

};

} // End of namespace.

#endif
