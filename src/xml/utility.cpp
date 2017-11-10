#include <vector>
#include <string>
#include "xml/xmltag.h"
#include "xml/utility.h"

namespace xml_process
{
  std::vector<CGateBase> read_all_gates(const char* bd_file)
  {
    CXmlPathBuilder bd_xml;
    std::vector<CGateBase> gates;
    bd_xml.read_xml(bd_file);
    xmlpath path = {"bdfacs", "experiment", "acquisition_worksheets",
    "worksheet_template", "gates", "gate"};

    for (auto node = bd_xml.goto_path(path); node; node = node -> next_sibling())
    {
      auto type_attrib = bd_xml.get_attrib(node, "type");
      if ((!type_attrib) || (strcmp(type_attrib -> value(), "Region_Classifier")))
        continue;

      gates.push_back(CGateBase());
      gates.back().read_bd_gate(node);
    }
    return gates;
  }
  std::vector<std::string> read_all_samples(const char* flowjo_file)
  {
    CXmlPathBuilder flow_xml;
    flow_xml.read_xml(flowjo_file);
    std::vector<std::string> sample_list;
    for (auto write_node = flow_xml.goto_path({"Workspace", "SampleList", "Sample"});
      write_node; write_node = write_node -> next_sibling())
    {
      auto dataset_node = flow_xml.get_child(write_node, "DataSet");
      if (!dataset_node)
        return std::vector<std::string>();
      auto sample_id = flow_xml.get_attrib(dataset_node, "sampleID");
      if (!sample_id)
        return std::vector<std::string>();
      sample_list.push_back(sample_id -> value());
    }
    return sample_list;
  }
}
