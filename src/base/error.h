#ifndef ERROR_H
#define ERROR_H

#include <iostream>
#include <string>
#include <exception>

#define FACS_IO_ERROR(...) {\
  throw CError(__PRETTY_FUNCTION__, __FILE__, __LINE__, __VA_ARGS__);\
}
//throw CError(msg, __PRETTY_FUNCTION__, __FILE__, __LINE__, __VA_ARGS__);
#define FACS_IO_REQUIRE(cond, ...) { \
  if(!(cond)) FACS_IO_ERROR(__VA_ARGS__); \
}
//AR_ERROR(msg, __VA_ARGS__)

class CError: public std::exception
{
public:
  CError(const char* func, const char* file, const int line, const char* msg, ...);
  virtual const char* what() const throw();
  friend std::ostream& operator<<(std::ostream& os, CError& exp);
private:
  std::string message;
};

#endif
