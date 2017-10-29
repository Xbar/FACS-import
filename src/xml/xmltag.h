#ifndef XMLTAG_H
#define XMLTAG_H

#include <memory>
#include <vector>
#include <list>
#include <stack>
#include "rapidxml.hpp"

///////////////////////////////////////////////////////////////////////////////
// Basic XML IO
// Store the XML data and parser in shared pointer
// Can be easily copied/moved
class CXmlPathBuilder
{
public:
  // Path type in XML file.
  // List of tag names. The parent node comes before its child.
  using xmlpath = std::list<std::string>;

  CXmlPathBuilder():xml_doc(nullptr), xml_content(nullptr)
    {}
  CXmlPathBuilder(const CXmlPathBuilder& other):
    xml_doc(other.xml_doc), xml_content(other.xml_content)
    {}
  CXmlPathBuilder(CXmlPathBuilder&& other):
    xml_doc(other.xml_doc), xml_content(other.xml_content)
    {}
  ~CXmlPathBuilder() {}

  void read_xml(const char* filename);
  void write_xml(const char* filename);
  std::shared_ptr<rapidxml::xml_document<> > get_doc();

  rapidxml::xml_node<>* get_child(const rapidxml::xml_node<>* parent,
      const char* tagname);
  rapidxml::xml_node<>* add_child(rapidxml::xml_node<>* parent,
          const char* tagname, const char* value=nullptr);
  rapidxml::xml_node<>* update_child(rapidxml::xml_node<>* parent,
          const char* tagname, const char* value=nullptr);
  rapidxml::xml_node<>* get_node_from_path(const xmlpath& path);
  void build_path_under_node(rapidxml::xml_node<>* base, const xmlpath& path);

  rapidxml::xml_attribute<>* get_attrib(const rapidxml::xml_node<>* node,
    const char* attrib_name);
  rapidxml::xml_attribute<>* set_attrib(rapidxml::xml_node<>* node,
    const char* attrib_name, const char* attrib_value);

private:
  std::shared_ptr<rapidxml::xml_document<> > xml_doc;
  std::shared_ptr<std::vector<char> > xml_content;

  char* allocate_xml_string(const char* str);
  rapidxml::xml_node<>* allocate_xml_node(const char* name,
    const char* value=nullptr);
  rapidxml::xml_attribute<>* allocate_xml_attribute(const char* name,
    const char* value);

};

#endif
