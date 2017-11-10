#ifndef GATE_TREE_H
#define GATE_TREE_H

#include <vector>
#include <string>
#include <list>
#include "xml/gate.h"
#include "rapidxml.hpp"

namespace xml_process
{
  class CGateTree
  {
  public:
    CGateTree(std::vector<CGateBase>& gates);
    // Do not allow copy or move
    CGateTree(CGateTree&) = delete;
    ~CGateTree() {};

    void write_gates(rapidxml::xml_node<>* position, CXmlPathBuilder& builder);

  private:
    std::vector<std::list<int>> tree;
    std::vector<CGateBase>& gates;
    std::vector<bool> is_root;

    void build_tree();
    void write_tree(const int gate_idx, rapidxml::xml_node<>* position,
      CXmlPathBuilder& builder);
  };
}

#endif
