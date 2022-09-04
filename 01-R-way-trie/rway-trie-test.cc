#include "rway-trie.h"
#include <iostream>

int main(int argc, char** argv) {
  my::RWayTrie trie;
  trie.Add(std::string("apple\x00XYZ", 9));
  trie.Add(std::string("apple\x00JKL", 9));
  trie.Add("apple", reinterpret_cast<void*>(1));
  trie.Add("apple", reinterpret_cast<void*>(2));
  trie.Add("apple", reinterpret_cast<void*>(3));
  trie.Add("apple", reinterpret_cast<void*>(4));
  trie.Add("apple", reinterpret_cast<void*>(5));
  trie.Add("boy");
  trie.Add("cancel");
  std::cout << "trie.size() = " << trie.Size() << std::endl;

  int* val = nullptr;
  bool removed = trie.Remove("apple",reinterpret_cast<void**>(&val));
  std::cout << "removed:" << removed << ", old value: "<<val << std::endl;
  removed = trie.Remove("huawei",reinterpret_cast<void**>(&val));
  std::cout << "removed:" << removed << ", old value: "<<val << std::endl;

  std::cout << "*********************" << std::endl;
  return 0;
}