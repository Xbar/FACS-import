#include "xml/gate_tree.h"
#include "xml/utility.h"
#include <map>

namespace xml_process
{
  CGateTree::CGateTree(std::vector<CGateBase>& _gates):
    gates(_gates)
  {
    tree.resize(gates.size());
    is_root.assign(gates.size(), 1);
    build_tree();
  }

  void CGateTree::build_tree()
  {
    std::map<std::string, int> gates_dict;
    for (int i = 0; i < gates.size(); i++)
    {
      gates_dict[gates[i].get_name()] = i;
    }

    for (int i = 0; i < gates.size(); i++)
    {
      auto parent_gate = gates_dict.find(gates[i].get_parent());
      if (parent_gate != gates_dict.end())
      {
        int parent_idx = parent_gate -> second;
        gates[i].set_parent(gates[parent_idx]);
        tree[parent_idx].push_back(i);
        is_root[i] = 0;
      }
    }
  }

  void CGateTree::write_gates(rapidxml::xml_node<>* position,
    CXmlPathBuilder& builder, std::string group_name)
  {
    for (int i = 0; i < gates.size(); i++)
    {
      if (is_root[i])
        write_tree(i, position, builder, group_name);
    }
  }

  void CGateTree::write_tree(int gate_idx, rapidxml::xml_node<>* position,
    CXmlPathBuilder& builder, std::string group_name)
  {
    auto next_pos = gates[gate_idx].write_flowjo_gate(position, builder, group_name);
    for (auto iter = tree[gate_idx].begin(); iter != tree[gate_idx].end();
      iter++)
    {
      write_tree(*iter, next_pos, builder, group_name);
    }
  }

}
