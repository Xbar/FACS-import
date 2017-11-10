#ifndef XML_UTILITY_H
#define XML_UTILITY_H

#include <vector>
#include <string>
#include "xml/gate.h"

namespace xml_process
{
  std::vector<CGateBase> read_all_gates(const char* bd_file);
  std::vector<std::string> read_all_samples(const char* flowjo_file);
}

#endif
