#include "xml/gate_tree.h"
#include <map>

namespace xml_process
{
  std::vector<CGateBase> read_all_gates(const char* bd_file)
  {
    CXmlPathBuilder bd_xml;
    std::vector<CGateBase> gates;
    bd_xml.read_xml(bd_file);
    xmlpath path = {"bdfacs", "experiment", "acquisition_worksheets",
    "worksheet_template", "gates", "gate"};

    for (auto node = bd_xml.goto_path(path); node; node = node -> next_sibling())
    {
      auto type_attrib = bd_xml.get_attrib(node, "type");
      if ((!type_attrib) || (strcmp(type_attrib -> value(), "Region_Classifier")))
        continue;

      gates.push_back(CGateBase());
      gates.back().read_bd_gate(node);
    }
    return gates;
  }

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
    CXmlPathBuilder& builder)
  {
    for (int i = 0; i < gates.size(); i++)
    {
      if (is_root[i])
        write_tree(i, position, builder);
    }
  }

  void CGateTree::write_tree(int gate_idx, rapidxml::xml_node<>* position,
    CXmlPathBuilder& builder)
  {
    auto next_pos = gates[gate_idx].write_flowjo_gate(position, builder);
    for (auto iter = tree[gate_idx].begin(); iter != tree[gate_idx].end();
      iter++)
    {
      write_tree(*iter, next_pos, builder);
    }
  }

}
