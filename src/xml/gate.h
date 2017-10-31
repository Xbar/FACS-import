#ifndef XML_GATE_H
#define XML_GATE_H

#include <list>
#include <string>
#include "rapidxml.hpp"
#include "xml/basic.h"
#include "xml/xmltag.h"

namespace xml_process
{
  ///////////////////////////////////////////////////////////////////////////////
  // Base class for gates
  class CGateBase
  {
  public:
    CGateBase(): gate_x(std::list<double>()), gate_y(std::list<double>()),
      log_x(0), log_y(0), label_x(""), label_y(""),
      gate_name(""), gate_id(""), gate_input(""), gate_parent(""),
      parent_id(""), gate_type("")
    {}
    CGateBase(const CGateBase& other): gate_x(other.gate_x), gate_y(other.gate_y),
      log_x(other.log_x), log_y(other.log_y),
      label_x(other.label_x), label_y(other.label_y),
      gate_name(other.gate_name), gate_id(other.gate_id),
      gate_input(other.gate_input), gate_parent(other.gate_parent),
      parent_id(other.parent_id), gate_type(other.gate_type)
    {}
    CGateBase(CGateBase&& other): gate_x(other.gate_x), gate_y(other.gate_y),
      log_x(other.log_x), log_y(other.log_y),
      label_x(other.label_x), label_y(other.label_y),
      gate_name(other.gate_name), gate_id(other.gate_id),
      gate_input(other.gate_input), gate_parent(other.gate_parent),
      parent_id(other.parent_id), gate_type(other.gate_type)
    {}
    ~CGateBase() {};

    void read_bd_gate(const rapidxml::xml_node<>* bd_gate);
    void read_flowjo_gate(const rapidxml::xml_node<>* flowjo_gate);

    rapidxml::xml_node<>* write_bd_gate(rapidxml::xml_node<>* position,
      CXmlPathBuilder& builder);
    rapidxml::xml_node<>* write_flowjo_gate(rapidxml::xml_node<>* position,
      CXmlPathBuilder& builder);

    std::string get_parent()
    { return gate_parent; }

    std::string get_name()
    { return gate_name; }

    std::string get_parent_id()
    { return parent_id; }

    std::string get_id()
    { return gate_id; }

    void set_parent(CGateBase& parent);

    void reinit();

#ifdef DEBUG_STDOUT
    void print_coordinates();
#endif

  protected:
    std::list<double> gate_x, gate_y;
    bool log_x, log_y;
    std::string label_x, label_y;
    std::string gate_name, gate_id, gate_input, gate_parent;
    std::string parent_id, gate_type;

    void transform_biexp(std::list<double>& coords);
    void transform_log(std::list<double>& coords);
    void transform_rect(std::list<double>& coords);

    void build_polygon_gate(rapidxml::xml_node<>* position,
      CXmlPathBuilder& builder);
    void build_rectangle_gate(rapidxml::xml_node<>* position,
      CXmlPathBuilder& builder);
    void build_interval_gate(rapidxml::xml_node<>* position,
      CXmlPathBuilder& builder);
  };
/*
  ///////////////////////////////////////////////////////////////////////////////
  // PolygonGate
  class CGatePolygon: public CGateBase
  {
  public:
    void read_bd_gate(rapidxml::xml_node<>* bd_gate);
    void read_flowjo_gate(rapidxml::xml_node<>* flowjo_gate);

    rapidxml::xml_node<>* write_bd_gate(CXmlPathBuilder& builder);
    rapidxml::xml_node<>* write_flowjo_gate(CXmlPathBuilder& builder);
  };
*/
}
#endif
