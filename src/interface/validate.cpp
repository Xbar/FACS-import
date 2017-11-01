#include <string>
#include <vector>
#include "interface/validate.h"
#include "xml/basic.h"
#include "xml/xmltag.h"
#include "xml/gate_tree.h"

using namespace xml_process;

extern "C" int validate_bd(const char* bd_file)
{
  auto gates = read_all_gates(bd_file);
  return (gates.size() > 0);
}

extern "C" int validate_flowjo(const char* flowjo_file)
{
  CXmlPathBuilder flow_xml;
  flow_xml.read_xml(flowjo_file);
  std::vector<std::string> sample_list;
  for (auto write_node = flow_xml.goto_path({"Workspace", "SampleList", "Sample"});
    write_node; write_node = write_node -> next_sibling())
  {
    auto dataset_node = flow_xml.get_child(write_node, "DataSet");
    if (!dataset_node)
      return 0;
    auto sample_id = flow_xml.get_attrib(dataset_node, "sampleID");
    if (!sample_id)
      return 0;
    sample_list.push_back(sample_id -> value());
  }
  return (sample_list.size() > 0);
}
