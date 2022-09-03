#include "rway-trie.h"

#ifndef NODE_DEBUG_MSG
#include <iostream>
#define NODE_DEBUG_MSG                                             \
  do {                                                             \
    std::cout << "Node::~Node() called, val=" << val << std::endl; \
  } while (0)
#endif

NAMESPACE_BEGIN

static constexpr char kNUL = 0;

static uint64_t MaskString(const std::string& str) {
  uint64_t mask = 0;
  for (const auto& c : str) {
    mask |= 1ull << (c - 'a');
  }
  return mask;
}

// template <typename Mutex>
struct RWayTrie::Node : public std::enable_shared_from_this<Node> {
  Node() {
    term = false;
    depth = 0;
    priv_data = nullptr;
    mask = 0;
    parent.reset();
    term_count = 0;
  }
  ~Node() { NODE_DEBUG_MSG; }

  NodeSPtr AddChild(char cval, const std::string& path, uint64_t bitmask,
                    void* privData, bool term) {
    auto node = std::make_shared<Node>();
    node->val = cval;
    node->path = path;
    node->mask = bitmask;
    node->term = term;
    node->priv_data = privData;
    node->parent = this->shared_from_this();
    node->depth = depth + 1;

    this->children[cval] = node;
    this->mask |= bitmask;
    return node;
  }

  char val;
  std::string path;  // REQUIRED ?
  bool term;
  int depth;
  void* priv_data;
  uint64_t mask;
  NodeWPtr parent;
  std::unordered_map<char, RWayTrie::NodeSPtr> children;
  int term_count;  // FOR WHAT ?
};

RWayTrie::RWayTrie() {
  root_.reset(new Node);
  root_->val = 'R';  // for debug used only.
  size_ = 0;
}

RWayTrie::~RWayTrie() {}

void RWayTrie::Add(const std::string& key, void* privData) {
  assert(!key.empty());
  uint64_t bitmask = MaskString(key);
  auto node = root_->shared_from_this();
  node->mask |= bitmask;
  node->term_count++;
  for (const auto& c : key) {
    auto iter = node->children.find(c);
    if (iter != node->children.cend()) {
      node = iter->second;
      node->mask |= bitmask;
    } else {
      node = node->AddChild(c, "", bitmask, nullptr, false);
    }
    node->term_count++;
  }
  // 注意这里, 会额外插入一个子节点存数据,和一般的Trie略有差异
  // 比如"apple"，数据并不是存在"e"上，而是会在e下新建一个空节点.
  node = node->AddChild(kNUL, key, 0, privData, true);
  size_++;
}

void RWayTrie::Remove(const std::string& key) {
  // TODO
}

int RWayTrie::Size() const { return size_; }


NAMESPACE_END