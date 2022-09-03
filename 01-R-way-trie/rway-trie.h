#pragma once

#include <cassert>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#define NAMESPACE_BEGIN namespace gx {
#define NAMESPACE_END }

NAMESPACE_BEGIN

class RWayTrie {
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
  void Remove(const std::string& key);

  int Size() const;

 private:
  int size_;
  NodeSPtr root_;
};

NAMESPACE_END