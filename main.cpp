#include "homework_3.h"

using namespace homework3;

int main(int argc, char const *argv[])
{
  try
  {
    homework_3(std::cout);
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}