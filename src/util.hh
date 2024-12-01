#ifndef UTIL_H_
#define UTIL_H_

#include <stdexcept>

class CompilationError : public std::runtime_error
{
public:
  using std::runtime_error::runtime_error;
};

#endif // UTIL_H_
