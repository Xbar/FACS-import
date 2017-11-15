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

class CGateTest: public CGateBase
{
public:
  void test_biexp(list<double>& input, int zero_cross)
  {
    transform_biexp(input, zero_cross);
  }
  list<double>& get_x()
  {
    return gate_x;
  }

  list<double>& get_y()
  {
    return gate_y;
  }
};

TEST(GATE, write_polygon)
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
    break;
  }

  // Now validate gate
  auto gate = flow_xml.goto_path({"Workspace", "SampleList", "Sample"});
  auto graph_node = flow_xml.get_child(gate, "Graph");
  EXPECT_TRUE(graph_node != nullptr);
  gate = flow_xml.goto_path({"Workspace", "SampleList", "Sample",
  "Subpopulations", "Population"});
  auto attrib = flow_xml.get_attrib(gate, "name");
  EXPECT_STREQ(attrib -> value(), "P1");
}

TEST(GATE, biexp)
{
  CGateTest test;/*
  list<double> biexp = {0, 100, 1000, 4000};
  test.test_biexp(biexp, -15);
  for (auto num: biexp)
  {
    cout << num << " ";
  }
  cout << "\n";*/

  const char xml_file[] = "bd.xml";
  CXmlPathBuilder my_xml;
  my_xml.read_xml(xml_file);

  for (auto node = my_xml.goto_path({"bdfacs", "experiment", "acquisition_worksheets",
  "worksheet_template", "gates", "gate"}); node; node = node -> next_sibling())
  {
    auto type_attrib = my_xml.get_attrib(node, "type");
    if ((!type_attrib) || (strcmp(type_attrib -> value(), "Region_Classifier")))
      continue;

    test.read_bd_gate(node);
    if (test.get_name()[0] == 'Q')
    {
      for (auto num: test.get_x())
      {
        cout << num << " ";
      }
      cout << "\n";
      for (auto num: test.get_y())
      {
        cout << num << " ";
      }
      cout << "\n";
    }
  }
}
