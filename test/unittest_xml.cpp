#include <gtest/gtest.h>
#include <string>
#include <fstream>
#include <stack>
#include <cstdlib>
#include "xml/xmltag.h"
#include "test_helper.h"

using namespace std;
using namespace xml_process;

TEST(XML_IO, read_write)
{
  const char xml_file[] = "bd.xml";
  const char xml_out[] = "bd.test.o";
  string xml_string = read_file_contents(xml_file);
  CXmlPathBuilder my_xml;
  my_xml.read_xml(xml_file);
  my_xml.write_xml(xml_out);
  string out_string = read_file_contents(xml_out);
  EXPECT_EQ(xml_string, out_string);
}

TEST(XML_IO, find_tag)
{
  const char xml_file[] = "bd.xml";
  CXmlPathBuilder my_xml;
  CTagFinder test_xml;
  my_xml.read_xml(xml_file);
  test_xml.set_xml_doc(my_xml.get_doc());
  auto gates = test_xml.get_next_tag("gates");
  list<string> path = {"bdfacs", "experiment", "acquisition_worksheets",
  "worksheet_template", "gates"};
  auto node = my_xml.goto_path(path);
  EXPECT_EQ(node, gates);

  gates = test_xml.get_next_tag("gates");
  list<string> path2 = {"bdfacs", "experiment", "specimen", "tube", "gates"};
  node = my_xml.goto_path(path2);
  EXPECT_EQ(node, gates);
}

TEST(XML_IO, find_attrib)
{
  const char xml_file[] = "bd.xml";
  CXmlPathBuilder my_xml;
  my_xml.read_xml(xml_file);

  list<string> path = {"bdfacs", "experiment", "acquisition_worksheets",
  "worksheet_template", "gates"};
  auto node = my_xml.goto_path(path);
  auto child = node -> first_node();
  auto attrib = my_xml.get_attrib(child, "fullname");
  EXPECT_STREQ(attrib -> value(), "All Events");
}

TEST(XML_IO, create_tag)
{
  const char xml_file[] = "flowjo.xml";
  CXmlPathBuilder my_xml;
  my_xml.read_xml(xml_file);

  list<string> path = {"Workspace", "SampleList", "Sample", "Subpopulations",
  "Population", "Gate"};
  my_xml.build_path(path);
  auto node = my_xml.goto_path(path);
  EXPECT_TRUE(node != nullptr);
  EXPECT_STREQ(node -> name(), "Gate");
}

TEST(XML_IO, create_attributes)
{
  const char xml_file[] = "flowjo.xml";
  CXmlPathBuilder my_xml;
  my_xml.read_xml(xml_file);

  list<string> path = {"Workspace", "SampleList", "Sample", "Subpopulations",
  "Population", "Gate", "gating:PolygonGate"};
  my_xml.build_path(path);
  auto node = my_xml.goto_path(path);

  attrib_list attributes = {{"eventsInside", "1"},
    {"annoOffsetX", "0"}, {"annoOffsetY", "0"}, {"tint", "#000000"},
    {"isTinted", "0"}, {"lineWeight", "Normal"}};
  my_xml.set_attrib_list(node, attributes);
  auto attrib_out = my_xml.get_attrib_list(node);
  //my_xml.write_xml("jo.test.o");
  //EXPECT_EQ(attrib_out, attributes);
  for (auto iter = attrib_out.begin(), iter2 = attributes.begin();
    iter != attrib_out.end(); iter++, iter2++)
  {
    EXPECT_EQ(iter -> name, iter2 -> name);
    EXPECT_EQ(iter -> value, iter2 -> value);
  }
}
