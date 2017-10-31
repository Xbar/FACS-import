#include <string>
#include "rapidxml.hpp"
#include "rapidxml_print.hpp"
#include "rapidxml_utils.hpp"
#include "test_helper.h"
#include "base/error.h"

using namespace std;
// Read all contents in a file
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

// Return next node with the tag name as the param
rapidxml::xml_node<>* CTagFinder::get_next_tag(const char* tagname)
{
  if (is_node_expanded == nullptr || last_enum_path == nullptr)
  {
    is_node_expanded = std::make_shared<node_state>();
    last_enum_path = std::make_shared<node_path>();
    last_enum_path -> push(xml_doc -> first_node());
    is_node_expanded -> push(0);
  }
  while (!(last_enum_path -> empty()))
  {
    auto current_node = last_enum_path -> top();
    auto current_expanded = is_node_expanded -> top();

    if (!current_expanded)
    {
      // Mark as explored
      is_node_expanded -> top() = 1;
      // Found the tag
      if (strcmp(tagname, current_node -> name()) == 0)
        return current_node;
      // Have children
      if (current_node -> first_node())
      {
        last_enum_path -> push(current_node -> first_node());
        is_node_expanded -> push(0);
      }
    }
    else // Expanded then explore siblings
    {
      last_enum_path -> pop();
      is_node_expanded -> pop();
      if (current_node -> next_sibling())
      {
        last_enum_path -> push(current_node -> next_sibling());
        is_node_expanded -> push(0);
      }
    }
  }
  return nullptr;
}

char* CTagFinder::allocate_xml_string(const char* str)
{
  return xml_doc -> allocate_string(str);
}

rapidxml::xml_node<>* CTagFinder::allocate_xml_node(const char* name,
  const char* value)
{
  FACS_IO_REQUIRE(name, "Node name cannot be null.");
  char* node_name = xml_doc -> allocate_string(name);
  char* node_value = nullptr;
  if (value)
    node_value = xml_doc -> allocate_string(value);
  return xml_doc -> allocate_node(rapidxml::node_element, node_name, node_value);
}

rapidxml::xml_attribute<>* CTagFinder::allocate_xml_attribute(const char* name,
  const char* value)
{
  FACS_IO_REQUIRE(name, "Attribute name cannot be null.");
  FACS_IO_REQUIRE(value, "Attribute value cannot be null.");
  char* node_name = xml_doc -> allocate_string(name);
  char* node_value = xml_doc -> allocate_string(value);
  return xml_doc -> allocate_attribute(node_name, node_value);
}
