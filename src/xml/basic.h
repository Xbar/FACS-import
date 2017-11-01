#ifndef XML_BASIC_H
#define XML_BASIC_H

#include <string>
#include <list>
namespace xml_process
{
  ///////////////////////////////////////////////////////////////////////////////
  // Attribute type
  struct Attribute
  {
    std::string name;
    std::string value;
  };

  // Path type in XML file.
  // List of tag names. The parent node comes before its child.
  using xmlpath = std::list<std::string>;
  using attrib_list = std::list<Attribute>;

}

#endif
