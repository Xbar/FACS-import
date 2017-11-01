#include <vector>
#include "interface/bd2flowjo.h"
#include "base/error.h"
#include "xml/basic.h"
#include "xml/xmltag.h"
#include "xml/gate.h"
#include "xml/gate_tree.h"

using namespace xml_process;

extern "C" int bd2flowjo(const char* bd_file, const char* flowjo_file,
    const char* output_file)
{
  CXmlPathBuilder flow_xml;
  std::vector<CGateBase> gates = read_all_gates(bd_file);
  if (gates.size() < 1)
    return -1;
  if (flow_xml.read_xml(flowjo_file) != 0)
    return -2;

  CGateTree gate_tree(gates);
  auto write_node = flow_xml.build_path({"Workspace", "Groups", "GroupNode"});
  if (!write_node)
    return -3;
  flow_xml.set_attrib_list(write_node, {{"name", "All Samples"},
    {"owningGroup", "All Samples"}, {"expanded", "1"}});
  gate_tree.write_gates(write_node, flow_xml);
  write_node = flow_xml.add_child(write_node, "Group");
  flow_xml.set_attrib_list(write_node, {{"name", "All Samples"},
    {"live", "1"}, {"role", "ws.group.dlog.test"}});
  auto sample_ref_node = flow_xml.add_child(write_node, "SampleRefs");
  std::vector<std::string> sample_list;
  for (write_node = flow_xml.goto_path({"Workspace", "SampleList", "Sample"});
    write_node; write_node = write_node -> next_sibling())
  {
    auto sample_node = flow_xml.add_child(write_node, "SampleNode");
    gate_tree.write_gates(sample_node, flow_xml);
    auto dataset_node = flow_xml.get_child(write_node, "DataSet");
    if (!dataset_node)
      return -3;
    auto sample_id = flow_xml.get_attrib(dataset_node, "sampleID");
    if (!sample_id)
      return -3;
    flow_xml.set_attrib(sample_node, "name", sample_id -> value());
    sample_list.push_back(sample_id -> value());
  }
  for (auto iter = sample_list.begin(); iter != sample_list.end(); iter++)
  {
    auto sample_node = flow_xml.add_child(sample_ref_node, "SampleRef");
    if (!sample_node)
      return -3;
    flow_xml.set_attrib(sample_node, "sampleID", iter -> c_str());
  }

  auto ret_val = flow_xml.write_xml(output_file);
  if (!ret_val)
    return -4;
  return 0;
}
