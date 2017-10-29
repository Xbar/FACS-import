#include <gtest/gtest.h>
#include <string>
#include <fstream>
#include <stack>
#include <cstdlib>
#include "xml/xmltag.h"
#include "test_helper.h"

using namespace std;

string read_file_contents(const char* filename)
{
  string line, result;
  ifstream myfile(filename);
  if (myfile.is_open())
  {
    while (getline(myfile, line))
      result += line;
  }
  return result;
}

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
  auto node = my_xml.get_node_from_path(path);
  EXPECT_EQ(node, gates);

  gates = test_xml.get_next_tag("gates");
  list<string> path2 = {"bdfacs", "experiment", "specimen", "tube", "gates"};
  node = my_xml.get_node_from_path(path2);
  EXPECT_EQ(node, gates);
}

TEST(XML_IO, find_attrib)
{
  const char xml_file[] = "bd.xml";
  CXmlPathBuilder my_xml;
  my_xml.read_xml(xml_file);

  list<string> path = {"bdfacs", "experiment", "acquisition_worksheets",
  "worksheet_template", "gates"};
  auto node = my_xml.get_node_from_path(path);
  auto child = node -> first_node();
  auto attrib = my_xml.get_attrib(child, "fullname");
  EXPECT_STREQ(attrib -> value(), "All Events");
}
