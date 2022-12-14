#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#define NAMESPACE_BEGIN namespace my {
#define NAMESPACE_END }

NAMESPACE_BEGIN

class RWayTrie {
 public:
  struct Node;
  typedef std::shared_ptr<Node> NodeSPtr;
  typedef std::weak_ptr<Node> NodeWPtr;

 public:
  RWayTrie();
  ~RWayTrie();

  // Disallow copy and assign
  RWayTrie(const RWayTrie&) = delete;
  RWayTrie& operator=(const RWayTrie&) = delete;

  void Add(const std::string& key, void* privData = nullptr);
  bool Remove(const std::string& key, void** pPrivData = nullptr);
  std::vector<std::string> Keys(const std::string &prefix="") const;
  int Size() const;

 private:
  int size_;
  NodeSPtr root_;
};

NAMESPACE_END