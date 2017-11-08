#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include "xml/gate.h"
#include "base/error.h"

namespace xml_process
{
  const double LINEAR_TRANSITION_POINT = 100;
  const double LINEAR_TRANSITION_VALUE = 10;
  const double EXP_SLOPE = 905;
  const double DISPLAY_MAX = (1 << 18);
  const double PARAMETER_MAX = 4095;
  // Logistic weight
  // half_point: where weight is 0.5
  // Slope: Divide x by slope to get shallower change
  double logistic_weight(double x, double half_point, double slope)
  {
    return 1.0 / (1.0 + pow(10.0, (half_point - x) / slope));
  }

  // Linear function
  // zero_cross: intercept on y
  // point_x, point_y: another point on the line
  double linear_value(double x, double zero_cross, double point_x, double point_y)
  {
    return (point_y - zero_cross) / point_x * x + zero_cross;
  }

  // Exponential function
  // Slope: divide x by slope to get shallower curve
  // x_max: x range, 0 - x_max
  // y_max: y range, 0 - y_max
  double exp_value(double x, double slope, double x_max, double y_max)
  {
    double b = log10(y_max) - x_max / slope;
    return pow(10.0, x / slope + b);
  }

  // Smoothened biexp function
  double smoothened_biexp(double x, double zero_cross)
  {
    double weight = logistic_weight(x, LINEAR_TRANSITION_POINT, EXP_SLOPE);
    return weight * exp_value(x, EXP_SLOPE, PARAMETER_MAX, DISPLAY_MAX) +
      (1 - weight) * linear_value(x, zero_cross, LINEAR_TRANSITION_POINT,
        LINEAR_TRANSITION_VALUE);
  }

  // Parse gate paramters from BD gate node
  void CGateBase::read_bd_gate(const rapidxml::xml_node<>* bd_gate)
  {
    bool biexp_x, biexp_y;
    FACS_IO_REQUIRE(strcmp(bd_gate -> name(), "gate") == 0,
      "Input node is not a gate.");

    reinit();
    for (auto node = bd_gate -> first_node(); node;
      node = node -> next_sibling())
    {
      if (strcmp("name", node -> name()) == 0)
      {
        gate_name = node -> value();
      }
      else if (strcmp("parent", node -> name()) == 0)
      {
        gate_parent = node -> value();
      }
      else if (strcmp("region", node -> name()) == 0)
      {
        for (auto param = node -> first_attribute(); param;
          param = param -> next_attribute())
        {
          if (*(param -> name()) == 'x')
          {
            label_x = param -> value();
          }
          else if (*(param -> name()) == 'y')
          {
            label_y = param -> value();
          }
          else if (*(param -> name()) == 't')
          {
            gate_type = param -> value();
          }
        }
        auto point = node -> first_node();
        FACS_IO_REQUIRE(strcmp("points", point -> name()) == 0,
          "Unrecognized file format");
        for (point = point -> first_node(); point;
          point = point -> next_sibling())
        {
          for (auto coord = point -> first_attribute(); coord;
            coord = coord -> next_attribute())
          {
            if (*(coord -> name()) == 'x')
            {
              gate_x.push_back(atof(coord -> value()));
            }
            else if (*(coord -> name()) == 'y')
            {
              gate_y.push_back(atof(coord -> value()));
            }
          }
        }

      }
      else if (strcmp("is_x_parameter_scaled", node -> name()) == 0)
      {
        biexp_x = (*(node -> value()) == 't');
      }
      else if (strcmp("is_y_parameter_scaled", node -> name()) == 0)
      {
        biexp_y = (*(node -> value()) == 't');
      }
      else if (strcmp("is_x_parameter_log", node -> name()) == 0)
      {
        log_x = (*(node -> value()) == 't');
      }
      else if (strcmp("is_y_parameter_log", node -> name()) == 0)
      {
        log_y = (*(node -> value()) == 't');
      }
      else if (strcmp("x_parameter_scale_value", node -> name()) == 0)
      {
        zero_cross_x = atof(node -> value());
      }

      else if (strcmp("y_parameter_scale_value", node -> name()) == 0)
      {
        zero_cross_y = atof(node -> value());
      }
      else if (strcmp("input", node -> name()) == 0)
      {
        gate_input = node -> value();
      }
    }
    if (biexp_x)
      transform_biexp(gate_x, zero_cross_x);
    else if (log_x)
      transform_log(gate_x);

    if (biexp_y)
      transform_biexp(gate_y, zero_cross_y);
    else if (log_y)
      transform_log(gate_y);
    gate_id = "ID" + std::to_string(rand());

    // Rectangle gate
    if (gate_type == "RECTANGLE_REGION")
    {
      transform_rect(gate_x);
      transform_rect(gate_y);
    }

    // Sub gate in binner gate
    if ((gate_input != gate_parent) && gate_x.size() == 4)
    {
      gate_type = "RECTANGLE_REGION";
      transform_rect(gate_x);
      transform_rect(gate_y);
      if (gate_x.front() < 11)
        gate_x.front() = -1;
      else
        gate_x.back() = -1;
      if (gate_y.front() < 11)
        gate_y.front() = -1;
      else
        gate_y.back() = -1;
    }

    auto split_pos = gate_input.rfind('\\');
    if (split_pos != std::string::npos)
      gate_input = gate_input.substr(split_pos + 1);
    split_pos = gate_parent.rfind('\\');
    if (split_pos != std::string::npos)
      gate_parent = gate_parent.substr(split_pos + 1);
  }

  // Transform biexp coordinates to log
  void CGateBase::transform_biexp(std::list<double>& coords, double zero_cross)
  {
    for (auto iter = coords.begin(); iter != coords.end(); iter++)
    {
      (*iter) = smoothened_biexp(*iter, zero_cross);
    }
  }

  // Transform log to linear
  void CGateBase::transform_log(std::list<double>& coords)
  {
    for (auto iter = coords.begin(); iter != coords.end(); iter++)
    {
      (*iter) = std::pow(10.0, *iter);
    }
  }

  // Transform linear to rectangle
  void CGateBase::transform_rect(std::list<double>& coords)
  {
    double min_value = coords.front(), max_value = coords.front();
    for (auto iter = coords.begin(); iter != coords.end(); iter++)
    {
      min_value = (*iter < min_value)?(*iter):min_value;
      max_value = (*iter > max_value)?(*iter):max_value;
    }
    coords = {min_value, max_value};
  }

  // Not implemented
  void CGateBase::read_flowjo_gate(const rapidxml::xml_node<>* flowjo_gate)
  {
    return;
  }

  // Not implemented
  rapidxml::xml_node<>* CGateBase::write_bd_gate(rapidxml::xml_node<>* position,
    CXmlPathBuilder& builder)
  {
    return nullptr;
  }

  // Generate a node with gate parameters
  rapidxml::xml_node<>* CGateBase::write_flowjo_gate(rapidxml::xml_node<>* position,
    CXmlPathBuilder& builder)
  {
    auto current_node = builder.get_child(position, "Subpopulations");
    if (!current_node)
    {
      builder.add_child(position, "Subpopulations");
      current_node = builder.get_child(position, "Subpopulations");
    }
    current_node = builder.add_child(current_node, "Population");
    // Build population node
    auto pop_node = current_node;
    attrib_list current_attrib = {{"name", gate_name},
      {"owningGroup", "All Samples"},
      {"expanded", "1"}};
    builder.set_attrib_list(current_node, current_attrib);
    // Set graph attrib
    auto graph_node = builder.add_child(current_node, "Graph");
    builder.set_attrib(graph_node, "type", "Pseudocolor");
    auto axis_node = builder.add_child(graph_node, "Axis");
    builder.set_attrib_list(axis_node, {{"dimension", "x"}, {"name", label_x}});
    axis_node = builder.add_child(graph_node, "Axis");
    builder.set_attrib_list(axis_node, {{"dimension", "y"}, {"name", label_y}});
    // Build Gate node
    current_node = builder.add_child(current_node, "Gate");
    builder.set_attrib(current_node, "gating:id", gate_id.c_str());
    if (parent_id.length() > 0)
      builder.set_attrib(current_node, "gating:parent_id", parent_id.c_str());
    // Build gating based on gate type
    if (gate_type == "POLYGON_REGION")
      build_polygon_gate(current_node, builder);
    else if (gate_type == "RECTANGLE_REGION")
      build_rectangle_gate(current_node, builder);
    else if (gate_type == "INTERVAL_REGION")
      build_interval_gate(current_node, builder);
    else
      FACS_IO_ERROR("Unsupported gate type %s.", gate_type.c_str());
    return pop_node;
  }

  // Generate a node with polygon gate parameters
  void CGateBase::build_polygon_gate(rapidxml::xml_node<>* position,
    CXmlPathBuilder& builder)
  {
    auto current_node = builder.add_child(position, "gating:PolygonGate");
    builder.set_attrib_list(current_node, {{"eventsInside", "1"}, {"annoOffsetX", "0"},
      {"annoOffsetY", "0"}, {"tint", "#000000"}, {"isTinted", "0"},
      {"lineWeight", "Normal"}, {"userDefined", "1"}, {"quadId", "-1"},
      {"gateResolution", "256"}});

    // Build gating parameters
    // x label
    auto param_node = builder.add_child(current_node, "gating:dimension");
    auto sub_param = builder.add_child(param_node, "data-type:fcs-dimension");
    builder.set_attrib(sub_param, "data-type:name", label_x.c_str());
    // y label
    param_node = builder.add_child(current_node, "gating:dimension");
    sub_param = builder.add_child(param_node, "data-type:fcs-dimension");
    builder.set_attrib(sub_param, "data-type:name", label_y.c_str());

    for (auto iter_x = gate_x.begin(), iter_y = gate_y.begin();
          iter_x != gate_x.end(); iter_x++, iter_y++)
    {
      param_node = builder.add_child(current_node, "gating:vertex");
      sub_param = builder.add_child(param_node, "gating:coordinate");
      builder.set_attrib(sub_param, "data-type:value",
        (std::to_string(*iter_x).c_str()));
      sub_param = builder.add_child(param_node, "gating:coordinate");
      builder.set_attrib(sub_param, "data-type:value",
        (std::to_string(*iter_y).c_str()));
    }
  }

  // Generate a node with rectangle gate parameters
  void CGateBase::build_rectangle_gate(rapidxml::xml_node<>* position,
    CXmlPathBuilder& builder)
  {
    auto current_node = builder.add_child(position, "gating:RectangleGate");
    builder.set_attrib_list(current_node, {{"eventsInside", "1"}, {"annoOffsetX", "0"},
      {"annoOffsetY", "0"}, {"tint", "#000000"}, {"isTinted", "0"},
      {"lineWeight", "Normal"}, {"userDefined", "1"}, {"quadId", "-1"},
      {"gateResolution", "256"}});

    // Build gating parameters
    // x label
    auto param_node = builder.add_child(current_node, "gating:dimension");
    if (gate_x.front() >= 0)
      builder.set_attrib(param_node, "gating:min",
        std::to_string(gate_x.front()).c_str());
    if (gate_x.back() >= 0)
      builder.set_attrib(param_node, "gating:max",
        std::to_string(gate_x.back()).c_str());
    auto sub_param = builder.add_child(param_node, "data-type:fcs-dimension");
    builder.set_attrib(sub_param, "data-type:name", label_x.c_str());
    // y label
    param_node = builder.add_child(current_node, "gating:dimension");
    if (gate_y.front() >= 0)
      builder.set_attrib(param_node, "gating:min",
        std::to_string(gate_y.front()).c_str());
    if (gate_y.back() >= 0)
      builder.set_attrib(param_node, "gating:max",
        std::to_string(gate_y.back()).c_str());
    sub_param = builder.add_child(param_node, "data-type:fcs-dimension");
    builder.set_attrib(sub_param, "data-type:name", label_y.c_str());
  }

  // Generate a node with interval gate parameters
  void CGateBase::build_interval_gate(rapidxml::xml_node<>* position,
    CXmlPathBuilder& builder)
  {
    auto current_node = builder.add_child(position, "gating:RectangleGate");
    builder.set_attrib_list(current_node, {{"eventsInside", "1"}, {"annoOffsetX", "0"},
      {"annoOffsetY", "0"}, {"tint", "#000000"}, {"isTinted", "0"},
      {"lineWeight", "Normal"}, {"userDefined", "1"}, {"quadId", "-1"},
      {"gateResolution", "256"}});

    // Build gating parameters
    auto param_node = builder.add_child(current_node, "gating:dimension");
    if (label_x.length() > 0)
    {
      // x label
      if (gate_x.front() >= 0)
        builder.set_attrib(param_node, "gating:min",
          std::to_string(gate_x.front()).c_str());
      if (gate_x.back() >= 0)
        builder.set_attrib(param_node, "gating:max",
          std::to_string(gate_x.back()).c_str());
      auto sub_param = builder.add_child(param_node, "data-type:fcs-dimension");
      builder.set_attrib(sub_param, "data-type:name", label_x.c_str());
    }
    else
    {
      // y label
      if (gate_y.front() >= 0)
        builder.set_attrib(param_node, "gating:min",
          std::to_string(gate_y.front()).c_str());
      if (gate_y.back() >= 0)
        builder.set_attrib(param_node, "gating:max",
          std::to_string(gate_y.back()).c_str());
      auto sub_param = builder.add_child(param_node, "data-type:fcs-dimension");
      builder.set_attrib(sub_param, "data-type:name", label_y.c_str());
    }
  }

  void CGateBase::reinit()
  {
    gate_x.clear();
    gate_y.clear();
    label_x = "";
    label_y = "";
    gate_name = "";
    gate_id = "";
    gate_input = "";
    gate_parent = "";
    parent_id = "";
    gate_type = "";
  }

  void CGateBase::set_parent(CGateBase& parent)
  {
    FACS_IO_REQUIRE(parent.gate_name == gate_parent,
      "Parent should be %s, but given %s",
      gate_input.c_str(), parent.gate_name.c_str());
    parent_id = parent.gate_id;
  }

#ifdef DEBUG_STDOUT
  // Debug Output
  void CGateBase::print_coordinates()
  {
    for (auto iter_x = gate_x.begin(), iter_y = gate_y.begin();
          iter_x != gate_x.end(); iter_x++, iter_y++)
    {
      std::cout << (*iter_x) << " " << (*iter_y) << "\n";
    }
  }
#endif
}
