#include <string>
#include <vector>
#include "interface/validate.h"
#include "xml/basic.h"
#include "xml/xmltag.h"
#include "xml/gate_tree.h"
#include "xml/utility.h"

using namespace xml_process;

extern "C" int validate_bd(const char* bd_file)
{
  auto gates = read_all_gates(bd_file);
  return (gates.size() > 0);
}

extern "C" int validate_flowjo(const char* flowjo_file)
{
  auto sample_list = read_all_samples(flowjo_file);
  return (sample_list.size() > 0);
}
