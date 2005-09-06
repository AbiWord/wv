
#include "wv++.h"

using namespace wv;

class MyDocParser : public Parser
{
protected:

  virtual void AppendChar ( U16 ch )
  {
    printf("%c", (char)ch);
  }

} ;

int main (int argc, char ** argv)
{
  Library::Init ();

  MyDocParser dp;
  
  dp.Process ( argv[1] );

  Library::Term ();

  return 0;
}
