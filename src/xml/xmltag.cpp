#include <string>
#include <cstdio>
#include <cstdlib>
#include "rapidxml.hpp"
#include "rapidxml_print.hpp"
#include "rapidxml_utils.hpp"
#include "xml/xmltag.h"
#include "base/error.h"

namespace xml_process
{
  // Read XML file
  int CXmlPathBuilder::read_xml(const char* filename)
  {
    FILE* fp = fopen(filename, "r");
    //FACS_IO_REQUIRE(fp, "Cannot open file %s.", filename);
    if (!fp)
      return -1;
    fseek(fp, 0, SEEK_END);
    size_t file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (file_size < 1)
      return -2;
    //FACS_IO_REQUIRE(file_size > 0, "File %s is empty.", filename);
    xml_content = std::make_shared<std::vector<char>>();
    xml_content -> resize(file_size + 1);

    char* xml_data = xml_content -> data();
    fread(xml_data, sizeof(char), file_size, fp);
    xml_data[file_size] = 0;

    xml_doc = std::make_shared<rapidxml::xml_document<> >();
    xml_doc -> parse<0>(xml_data);

    return 0;
  }

  // Output XML file
  int CXmlPathBuilder::write_xml(const char* filename)
  {
    FILE* fp = fopen(filename, "w");
    //FACS_IO_REQUIRE(fp, "Cannot open file %s for write.", filename);
    if (!fp)
      return -1;
    std::string xml_data;

    rapidxml::print(std::back_inserter(xml_data), *xml_doc, rapidxml::print_no_indenting);
    fwrite(xml_data.c_str(), sizeof(char), xml_data.length(), fp);
    fclose(fp);
    return 0;
  }

  // Return the child node with a given name
  rapidxml::xml_node<>* CXmlPathBuilder::get_child(const rapidxml::xml_node<>* parent,
                                                    const char* tagname)
  {
    if (parent)
    {
      for(auto child = parent -> first_node(); child;
        child = child -> next_sibling())
      {
        if (strcmp(child -> name(), tagname) == 0)
          return child;
      }
    }
    return nullptr;
  }

  // Add a child node with a tag name
  rapidxml::xml_node<>* CXmlPathBuilder::add_child(rapidxml::xml_node<>* parent,
                                                    const char* tagname,
                                                  const char* value)
  {
    if (parent)
    {
      auto node = allocate_xml_node(tagname, value);
      parent -> append_node(node);
      return node;
    }
    return nullptr;
  }

  // Update a child node with a tag name
  rapidxml::xml_node<>* CXmlPathBuilder::update_child(rapidxml::xml_node<>* parent,
                                                    const char* tagname,
                                                  const char* value)
  {
    if (parent)
    {
      auto node = get_child(parent, tagname);
      if (node)
        node -> value(value);
      else
        node = add_child(parent, tagname, value);
      return node;
    }
    return nullptr;
  }

  // Get attribute value by name
  rapidxml::xml_attribute<>*
  CXmlPathBuilder::get_attrib(const rapidxml::xml_node<>* node,
    const char* name)
  {
    if (node)
    {
      for (auto attrib = node -> first_attribute(); attrib;
        attrib = attrib -> next_attribute())
      {
        if (strcmp(attrib -> name(), name) == 0)
        {
          return attrib;
        }
      }
    }
    return nullptr;
  }

  // Add/update attribute value by name
  rapidxml::xml_attribute<>*
  CXmlPathBuilder::set_attrib(rapidxml::xml_node<>* node,
    const char* name, const char* value)
  {
    if (node)
    {
      auto attrib = get_attrib(node, name);
      if (attrib)
      {
        attrib -> value(value);
        return attrib;
      }
      attrib = allocate_xml_attribute(name, value);
      node -> append_attribute(attrib);
      return attrib;
    }
    return nullptr;
  }

  // Return node at a given path
  rapidxml::xml_node<>* CXmlPathBuilder::goto_path(const xmlpath& path,
    const rapidxml::xml_node<>* base)
  {
    rapidxml::xml_node<>* current_node = const_cast<rapidxml::xml_node<>*>(base);
    if (!base)
      current_node = xml_doc.get();
    for (auto iter = path.begin(); iter != path.end(); iter++)
    {
      current_node = get_child(current_node, iter -> c_str());
      if (!current_node)
        return nullptr;
    }
    return current_node;
  }

  // Build a tree of nodes
  rapidxml::xml_node<>* CXmlPathBuilder::build_path(const xmlpath& path,
    rapidxml::xml_node<>* base)
  {
    auto current_node = base;
    if (!base)
      current_node = xml_doc.get();
    bool already_built = 1;
    for (auto iter = path.begin(); iter != path.end(); iter++)
    {
      if (!already_built)
      {
        current_node = add_child(current_node, iter -> c_str());
      }
      else
      {
        auto next_node = get_child(current_node, iter -> c_str());
        if (!next_node)
        {
          already_built = 0;
          next_node = add_child(current_node, iter -> c_str());
        }
        current_node = next_node;
      }
    }
    return current_node;
  }

  // Get all attributes
  attrib_list CXmlPathBuilder::get_attrib_list(const rapidxml::xml_node<>* node)
  {
    attrib_list result;
    if (node)
    {
      for (auto attrib = node -> first_attribute(); attrib;
        attrib = attrib -> next_attribute())
      {
        result.push_back({std::string(attrib -> name()),
          std::string(attrib -> value())});
      }
    }
    return result;
  }

  // Set all attributes
  int CXmlPathBuilder::set_attrib_list(rapidxml::xml_node<>* node,
    const attrib_list& attributes)
  {
    for (auto iter = attributes.begin(); iter != attributes.end(); iter++)
    {
      if(!set_attrib(node, (iter -> name).c_str(), (iter -> value).c_str()))
        return -1;
    }
    return 0;
  }

  char* CXmlPathBuilder::allocate_xml_string(const char* str)
  {
    return xml_doc -> allocate_string(str);
  }

  rapidxml::xml_node<>* CXmlPathBuilder::allocate_xml_node(const char* name,
    const char* value)
  {
    FACS_IO_REQUIRE(name, "Node name cannot be null.");
    char* node_name = xml_doc -> allocate_string(name);
    char* node_value = nullptr;
    if (value)
      node_value = xml_doc -> allocate_string(value);
    return xml_doc -> allocate_node(rapidxml::node_element, node_name, node_value);
  }

  rapidxml::xml_attribute<>* CXmlPathBuilder::allocate_xml_attribute(const char* name,
    const char* value)
  {
    FACS_IO_REQUIRE(name, "Attribute name cannot be null.");
    FACS_IO_REQUIRE(value, "Attribute value cannot be null.");
    char* node_name = xml_doc -> allocate_string(name);
    char* node_value = xml_doc -> allocate_string(value);
    return xml_doc -> allocate_attribute(node_name, node_value);
  }

  std::shared_ptr<rapidxml::xml_document<> > CXmlPathBuilder::get_doc()
  {
    return xml_doc;
  }

}
