#ifndef TEST_HELPER_H
#define TEST_HELPER_H

#include <memory>
#include <vector>
#include <list>
#include <stack>
#include "rapidxml.hpp"

class CTagFinder
{
public:
  CTagFinder():xml_doc(nullptr), last_enum_path(nullptr),
    is_node_expanded(nullptr)
    {}
  CTagFinder(const CTagFinder& other):
    xml_doc(other.xml_doc), last_enum_path(other.last_enum_path),
    is_node_expanded(other.is_node_expanded)
    {}
  CTagFinder(CTagFinder&& other):
    xml_doc(other.xml_doc), last_enum_path(other.last_enum_path),
    is_node_expanded(other.is_node_expanded)
    {}
  ~CTagFinder() {}

  void set_xml_doc(std::shared_ptr<rapidxml::xml_document<> > _doc)
  {
    xml_doc = _doc;
  }

  rapidxml::xml_node<>* get_next_tag(const char* tagname);
  void reset_enumerator()
  { last_enum_path = nullptr;
    is_node_expanded = nullptr;
  }

  char* allocate_xml_string(const char* str);
  rapidxml::xml_node<>* allocate_xml_node(const char* name,
    const char* value=nullptr);
  rapidxml::xml_attribute<>* allocate_xml_attribute(const char* name,
    const char* value);

private:
  typedef std::stack<rapidxml::xml_node<>*> node_path;
  typedef std::stack<bool> node_state;
  std::shared_ptr<rapidxml::xml_document<> > xml_doc;
  std::shared_ptr<node_path> last_enum_path;
  std::shared_ptr<node_state> is_node_expanded;
};


#endif
