#include <vector>
#include <iostream>
#include "xml/gate_tree.h"
#include "xml/gate.h"

using namespace xml_process;

extern "C" int bd2flowjo(const char* bd_file, const char* flowjo_file,
    const char* output_file)
{
  CXmlPathBuilder flow_xml;
  std::vector<CGateBase> gates = read_all_gates(bd_file);
  flow_xml.read_xml(flowjo_file);

  CGateTree gate_tree(gates);
  auto write_node = flow_xml.build_path({"Workspace", "Groups", "GroupNode"});
  gate_tree.write_gates(write_node, flow_xml);
  write_node = flow_xml.add_child(write_node, "Group");
  flow_xml.set_attrib_list(write_node, {{"name", "All Samples"},
    {"live", "1"}, {"role", "ws.group.dlog.test"}});
  auto sample_ref_node = flow_xml.add_child(write_node, "SampleRefs");
  std::vector<std::string> sample_list;
  for (write_node = flow_xml.goto_path({"Workspace", "SampleList", "Sample"});
    write_node; write_node = write_node -> next_sibling())
  {
    gate_tree.write_gates(write_node, flow_xml);
    auto dataset_node = flow_xml.get_child(write_node, "DataSet");
    auto sample_id = flow_xml.get_attrib(dataset_node, "sampleID");
    sample_list.push_back(sample_id -> value());
  }
  for (auto iter = sample_list.begin(); iter != sample_list.end(); iter++)
  {
    auto sample_node = flow_xml.add_child(sample_ref_node, "SampleRef");
    flow_xml.set_attrib(sample_node, "sampleID", iter -> c_str());
  }

  flow_xml.write_xml(output_file);
  return 0;
}

int main(int argc, char** argv)
{
  if (argc == 4)
  {
    bd2flowjo(argv[1], argv[2], argv[3]);
  }
  else
  {
    std::cout << "Usage: [BD xml file] [Cytobank wsp file] [Output file]\n";
  }
}
