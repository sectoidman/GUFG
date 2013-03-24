#include "tokenizer.h"
#include <stdexcept>

using std::out_of_range;

tokenizer::tokenizer(string in, string delims)
{
  s = in;
  d = delims;
  start = 0;
  end = 0;
}

string tokenizer::current()
{
  try
  {
    return s.substr(start, s.find_first_of(d, end) - start);
  }
  catch (out_of_range e)
  {
    return "";
  }
}

void tokenizer::next_pos()
{
  start = s.find_first_not_of(d, end);
  end = s.find_first_of(d, start);
}

string tokenizer::next()
{
  next_pos();
  return current();
}

string tokenizer::next(string new_delims)
{
  d = new_delims;
  return next();
}

string tokenizer::operator()()
{
  return next();
}

string tokenizer::operator()(string new_delims)
{
  return next(new_delims);
}
