#include "rway-trie.h"
#include <iostream>

int main(int argc, char** argv) {
  gx::RWayTrie trie;
//   trie.Add("apple");
  trie.Add(std::string("apple\x00XYZ", 9));
  trie.Add(std::string("apple\x00JKL", 9));
  trie.Add(std::string("appleyui", 9));
  trie.Add("apple");
  //   trie.Add("applet");
  std::cout << "trie.size() = " << trie.Size() << std::endl;

  std::cout << "*********************" << std::endl;
  return 0;
}