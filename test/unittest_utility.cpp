#include <gtest/gtest.h>
#include <string>
#include <fstream>
#include <stack>
#include <cstdlib>
#include "xml/xmltag.h"
#include "xml/utility.h"
#include "xml/gate_tree.h"
#include "test_helper.h"

using namespace std;
using namespace xml_process;

TEST(UTILITY, read_gates)
{
  auto gates = read_all_gates("bd.xml");
  EXPECT_EQ(gates.size(), 9);
  vector<string> ans = {"P1", "P2", "P3", "Q1", "Q2", "Q3", "Q4", "P4", "P5"};
  for (int i = 0; i < gates.size(); i++)
  {
    EXPECT_EQ(ans[i], gates[i].get_name());
  }
}

TEST(UTILITY, read_samples)
{
  auto samples = read_all_samples("flowjo.xml");
  vector<string> ans = {"1", "2", "3", "4", "5", "6"};
  EXPECT_EQ(samples, ans);
}
