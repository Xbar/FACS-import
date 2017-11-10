#include <gtest/gtest.h>
#include <string>
#include <fstream>
#include <stack>
#include <cstdlib>
#include "xml/xmltag.h"
#include "xml/utility.h"
#include "xml/gate_tree.h"
#include "test_helper.h"

using namespace std;
using namespace xml_process;

TEST(GATETREE, build_tree)
{
  auto gates = read_all_gates("bd.xml");
  CGateTree my_tree(gates);
  vector<string> output, ans;
  for (auto gate: gates)
  {
    output.push_back(gate.get_parent_id());
    ans.push_back(gate.get_id());
  }
  for (int i = 3; i < ans.size(); i++)
    ans[i] = ans[2];
  ans[2] = ans[1];
  ans[1] = ans[0];
  ans[0] = "";
  for (int i = 0; i < output.size(); i++)
    EXPECT_EQ(ans[i], output[i]);
}

TEST(GATETREE, DISABLED_write_gates)
{
  auto gates = read_all_gates("bd.xml");
  CGateTree gate_tree(gates);
  CXmlPathBuilder flow_xml;
  flow_xml.read_xml("flowjo.xml");
  auto write_node = flow_xml.build_path({"Workspace", "Groups", "GroupNode"});
  gate_tree.write_gates(write_node, flow_xml);
  auto head_node = flow_xml.goto_path({"Workspace"});
  flow_xml.set_attrib(head_node, "xmlns:gating", "http://www.isac-net.org/std/Gating-ML/v2.0/gating");
  //flow_xml.write_xml("gatetree.test.o");

  //string output = read_file_contents("gatetree.test.o");
  //string ans = read_file_contents("gatetree.out.xml");
  //EXPECT_EQ(ans, output);
}
