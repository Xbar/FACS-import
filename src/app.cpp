#include <iostream>
#include "interface/bd2flowjo.h"

int main(int argc, char** argv)
{
  if (argc == 4)
  {
    return bd2flowjo(argv[1], argv[2], argv[3]);
  }
  else
  {
    std::cout << "Usage: [BD xml file] [Cytobank wsp file] [Output file]\n";
  }
  return 0;
}
