#include <libcsc/hashmap/HashMap.hpp>

#include <gtest/gtest.h>

#include <algorithm>
#include <sstream>
#include <string>

namespace {

struct f {
  size_t operator()(const int& k) const { return k % 10; }
};

char char_transform(char c) {
  if (c != 'd') {
    return 'd';
  }
  return '0';
}

std::string get_key_1(
    const libcsc::hashmap::HashMap<int, std::string, f>& map) {
  return map.at(1);
}

}  // namespace

TEST(HashMapSuite, MainTest) {
  libcsc::hashmap::HashMap<int, std::string, f> hmap;

  hmap.insert(1, "val1");
  hmap.insert(2, "val2");
  auto it = hmap.find(2);
  auto exp = *it;
  EXPECT_EQ(it.key(), 2);
  EXPECT_EQ(exp, "val2");

  hmap.remove(2);
  it = hmap.find(2);
  EXPECT_EQ(it, hmap.end());

  it = hmap.find(3);
  EXPECT_EQ(it, hmap.end());

  hmap[3] = "val3";
  it = hmap.find(3);
  exp = *it;
  EXPECT_EQ(it.key(), 3);
  EXPECT_EQ(exp, "val3");
}

TEST(HashMapSuite, CollisionTest) {
  libcsc::hashmap::HashMap<int, std::string, f> hmap;

  hmap[100420] = "val1";
  hmap[200520] = "val2";
  auto it = hmap.find(200520);
  auto exp = *it;
  EXPECT_EQ(it.key(), 200520);
  EXPECT_EQ(exp, "val2");

  it = hmap.find(100420);
  exp = *it;
  EXPECT_EQ(it.key(), 100420);
  EXPECT_EQ(exp, "val1");

  hmap[100420] = "val3";
  exp = *it;
  EXPECT_EQ(exp, "val3");

  hmap.insert(100420, "val4");
  it = hmap.find(100420);
  exp = *it;
  EXPECT_EQ(it.key(), 100420);
  EXPECT_EQ(exp, "val4");

  hmap.remove(100420);
  it = hmap.find(100420);
  EXPECT_EQ(it, hmap.end());
}

TEST(HashMapSuite, IteratorTest) {
  libcsc::hashmap::HashMap<std::string, std::string> hmap;
  std::stringstream out;

  hmap["abc"] = "bca";
  hmap["def"] = "fed";
  hmap["xyz"] = "zyx";
  hmap["abcd"] = "dcba";
  hmap["acbd"] = "bdca";

  for (auto it = hmap.begin(); it != hmap.end(); ++it) {
    out << it.key() << " - " << *it << "\n";
  }
  EXPECT_EQ(
      out.str(),
      "xyz - zyx\n"
      "abcd - dcba\n"
      "acbd - bdca\n"
      "def - fed\n"
      "abc - bca\n");

  const auto hmap2 = hmap;
  hmap = libcsc::hashmap::HashMap<std::string, std::string>();
  const auto exp = hmap.find("bca");
  EXPECT_EQ(exp, hmap.end());

  out.str("");
  for (auto it = hmap2.begin(); it != hmap2.end(); ++it) {
    out << it.key() << " - " << *it << "\n";
  }
  EXPECT_EQ(
      out.str(),
      "xyz - zyx\n"
      "abcd - dcba\n"
      "acbd - bdca\n"
      "def - fed\n"
      "abc - bca\n");
}

TEST(HashMapSuite, AtTest) {
  std::stringstream ss;
  try {
    libcsc::hashmap::HashMap<int, std::string, f> hmap;
    hmap[1] = "val1";

    auto exp = get_key_1(hmap);
    EXPECT_EQ(exp, "val1");

    hmap.at(1) = "val2";
    exp = hmap.at(1);
    EXPECT_EQ(exp, "val2");

    hmap.at(2);
  } catch (const std::out_of_range& e) {
    ss << e.what() << "\n";
  }
  EXPECT_EQ(ss.str(), "call a non-existent element\n");
}

TEST(HashMapSuite, STLAlgorithmTest) {
  libcsc::hashmap::HashMap<int, char, f> hmap;
  hmap[0] = 'a';
  hmap[1] = 'b';
  hmap[2] = 'c';
  hmap[100003] = 'X';
  hmap[100004] = 'Y';
  hmap[100005] = 'Z';

  auto exp = std::max_element(hmap.begin(), hmap.end());
  EXPECT_EQ(*exp, *hmap.find(2));

  exp = std::find_if(hmap.begin(), hmap.end(), islower);
  EXPECT_EQ(*exp, *hmap.find(0));

  exp = std::find_if_not(hmap.begin(), hmap.end(), islower);
  EXPECT_EQ(*exp, *hmap.find(100003));

  exp = std::find_if(hmap.begin(), hmap.end(), isdigit);
  EXPECT_EQ(exp, hmap.end());

  std::transform(hmap.begin(), hmap.end(), hmap.begin(), char_transform);

  exp = std::find(hmap.begin(), hmap.end(), 'd');
  EXPECT_EQ(*exp, *hmap.find(0));
  exp = std::find_if_not(
      hmap.begin(), hmap.end(), [](auto c) { return c == 'd'; });
  EXPECT_EQ(exp, hmap.end());
}
