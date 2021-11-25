#include "find.h"

using std::string;

namespace Find {
  bool FindByStr(const string &keyword, const string &str)
  {
    return (str.find(keyword) == string::npos ? false : true);
  }

  bool FindByChar(const char &keyword, const char &str)
  {
    return str == keyword;
  }
}