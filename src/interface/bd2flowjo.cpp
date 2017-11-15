#include <vector>
#include "interface/bd2flowjo.h"
#include "base/error.h"
#include "xml/basic.h"
#include "xml/xmltag.h"
#include "xml/gate.h"
#include "xml/gate_tree.h"
#include "xml/utility.h"

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
  auto sample_list = read_all_samples(flowjo_file);
  if (sample_list.size() < 1)
    return -2;

  CGateTree gate_tree(gates);
  auto workspace_node = flow_xml.goto_path({"Workspace"});
  flow_xml.set_attrib(workspace_node, "flowJoVersion", "10.0");
  /*auto write_node = flow_xml.build_path({"Workspace", "Groups", "GroupNode"});
  if (!write_node)
    return -3;
  flow_xml.set_attrib_list(write_node, {{"name", "All Samples"},
    {"owningGroup", "All Samples"}, {"expanded", "1"}});
  gate_tree.write_gates(write_node, flow_xml);
  write_node = flow_xml.add_child(write_node, "Group");
  flow_xml.set_attrib_list(write_node, {{"name", "All Samples"},
    {"live", "1"}, {"role", "ws.group.dlog.test"}});
  auto sample_ref_node = flow_xml.add_child(write_node, "SampleRefs");
  for (auto sample: sample_list)
  {
    // Add sample reference in the group
    auto ref_node = flow_xml.add_child(sample_ref_node, "SampleRef");
    if (!ref_node)
      return -3;
    flow_xml.set_attrib(ref_node, "sampleID", sample.c_str());
  }*/
  // Add gates to all samples
  auto sample_node = flow_xml.goto_path({"Workspace", "SampleList",
    "Sample"});
  for (auto sample: sample_list)
  {
    if (!sample_node)
      return -3;
    auto sample_node_node = flow_xml.add_child(sample_node, "SampleNode");
    if (!sample_node_node)
      return -3;
    flow_xml.set_attrib(sample_node_node, "expanded", "1");
    gate_tree.write_gates(sample_node_node, flow_xml);
    flow_xml.set_attrib(sample_node_node, "name", sample.c_str());
    sample_node = sample_node -> next_sibling();
  }


  auto ret_val = flow_xml.write_xml(output_file);
  if (ret_val != 0)
    return -4;
  return 0;
}
