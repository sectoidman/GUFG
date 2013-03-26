#include <string>

#ifndef ___tokenizer
#define ___tokenizer

using std::string;

struct tokenizer
{
  string s, d;
  size_t start, end;
  tokenizer(string, string);
  string current();
  void next_pos();
  string next();
  string next(string);
  string operator()();
  string operator()(string);
};

#endif
