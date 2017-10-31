#include <gtest/gtest.h>
#include <string>
#include <fstream>
#include <stack>
#include <cstdlib>
#include "xml/xmltag.h"
#include "xml/gate.h"
#include "test_helper.h"

using namespace std;
using namespace xml_process;

TEST(GATE, write_flowjo)
{
  const char xml_file[] = "bd.xml";
  CXmlPathBuilder my_xml;
  my_xml.read_xml(xml_file);
  list<string> path = {"bdfacs", "experiment", "acquisition_worksheets",
  "worksheet_template", "gates", "gate"};

  CGateBase my_gate;
  CXmlPathBuilder flow_xml;
  flow_xml.read_xml("flowjo.xml");
  for (auto node = my_xml.goto_path(path); node; node = node -> next_sibling())
  {
    auto type_attrib = my_xml.get_attrib(node, "type");
    if ((!type_attrib) || (strcmp(type_attrib -> value(), "Region_Classifier")))
      continue;

    my_gate.read_bd_gate(node);
    auto write_node = flow_xml.goto_path({"Workspace", "SampleList", "Sample"});
    my_gate.write_flowjo_gate(write_node, flow_xml);
  }

  flow_xml.write_xml("gate.test.o");
  string output = read_file_contents("gate.test.o");
  string ans = read_file_contents("gate.out.xml");
  EXPECT_EQ(ans, output);
}
