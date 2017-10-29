#include <cstdlib>
#include "base/error.h"

CError::CError(const char* func, const char* file, const int line, const char* msg, ...):
message("")
{
  char str[4096];
  va_list list;
  va_start(list, msg);
  vsnprintf(str, sizeof(str), msg, list);
  va_end(list);
  message = str;
  snprintf(str, sizeof(str), "\nIn %s of file: %s line: %d\n", func, file, line);
  message += str;
}

const char* CError::what() const throw()
{
  return message.c_str();
}

std::ostream& operator<<(std::ostream& os, CError& exp)
{
  os << exp.message;
  return os;
}
