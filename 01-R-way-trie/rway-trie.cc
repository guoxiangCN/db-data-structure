#include "rway-trie.h"

#include <cassert>

#ifndef NODE_DEBUG_MSG
#include <iostream>
#define NODE_DEBUG_MSG                                       \
  do {                                                       \
    std::cout << "Node::~Node() val = " << val << std::endl; \
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
  root_->val = '*';  // for debug used only.
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
  assert(node->val == key[key.length() - 1]);
  if (!node->term) {
    size_++;
  }
  node->term = true;
  node->priv_data = privData;
}

bool RWayTrie::Remove(const std::string& key, void** pPrivData) {
  assert(!key.empty());
  if (pPrivData) {
    *pPrivData = nullptr;
  }

  uint64_t bitmask = MaskString(key);
  auto node = root_->shared_from_this();
  for (const auto& c : key) {
    auto iter = node->children.find(c);
    if (iter != node->children.cend()) {
      node = iter->second;
    } else {
      return false;
    }
  }

  bool found = false;
  if (node->term) {
    found = true;
    if (pPrivData) {
      *pPrivData = node->priv_data;
    }
  }

  node->term = false;
  if (node->children.size() == 0) {
    auto parent = node->parent.lock();
    if (parent) {
      parent->children.erase(node->val);
    }
  }

  return found;
}

static void keysHelper(RWayTrie::NodeSPtr node, std::vector<std::string>& keys,
                       std::string& prefix, int level) {
  if (level != 0) {
    prefix.push_back(node->val);
  }

  if (node->term) {
    keys.push_back(prefix);
  }

  for (const auto& p : node->children) {
    keysHelper(p.second, keys, prefix, level + 1);
  }

  if (level != 0) {
    prefix.pop_back();
  }
}

std::vector<std::string> RWayTrie::Keys(const std::string& prefix) const {
  std::vector<std::string> keys;
  std::string pre = prefix;
  auto node = root_;
  for (const auto& c : prefix) {
    auto iter = node->children.find(c);
    if (iter == node->children.cend()) {
      return keys;
    } else {
      node = iter->second;
    }
  }
  keysHelper(node, keys, pre, 0);
  return keys;
}

int RWayTrie::Size() const { return size_; }

NAMESPACE_END