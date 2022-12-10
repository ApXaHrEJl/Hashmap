#pragma once

#include <cassert>
#include <cstddef>
#include <iterator>
#include <stdexcept>

namespace libcsc::hashmap {

template <
    class K,
    class V,
    class F = std::hash<K>,
    class E = std::equal_to<K>,
    size_t TABLE_SIZE = 16384>
class HashMap {
 private:
  class HashNode {
   public:
    HashNode(const K& key, const V& value)
        : key_(key), value_(value), next_(nullptr) {}

    K key() const { return key_; }

    V value() const { return value_; }

    HashNode* next() const { return next_; }

    void set_next(HashNode* next) { next_ = next; }

    V& operator*() { return value_; }

   private:
    K key_;
    V value_;
    HashNode* next_;
  };

  void table_init() {
    table_ = new HashNode*[TABLE_SIZE];
    for (size_t i = 0; i < TABLE_SIZE; ++i) {
      table_[i] = nullptr;
    }
  }

  void free() noexcept {
    if (table_) {
      for (size_t i = 0; i < TABLE_SIZE; ++i) {
        HashNode* cur = table_[i];
        while (cur != nullptr) {
          HashNode* prev = cur;
          cur = cur->next();
          delete prev;
        }
      }
      delete table_;
    }
  }

  F Hash;
  E Equal;
  HashNode** table_;

 public:
  class Iterator {
    friend HashMap;

   private:
    Iterator(HashNode** table, size_t index, HashNode* node)
        : table_(table), index_(index), node_(node) {}

    Iterator(HashNode** table, size_t index) : table_(table), index_(index) {
      if (index_ == TABLE_SIZE) {
        node_ = nullptr;
      } else {
        node_ = table_[index_];
      }
    }
    HashNode** table_;
    size_t index_;
    HashNode* node_;

   public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = V;
    using pointer = HashNode*;
    using reference = HashNode&;
    using size_type = size_t;

    K key() const {
      assert(node_);
      return node_->key();
    }

    Iterator& operator++() {
      if (node_->next()) {
        node_ = node_->next();
        return *this;
      }
      do {
        if (++index_ >= TABLE_SIZE) {
          node_ = nullptr;
          return *this;
        }
      } while (table_[index_] == nullptr);

      node_ = table_[index_];
      return *this;
    }

    Iterator operator++(int) {
      const auto it = *this;
      ++(*this);
      return it;
    }

    bool operator==(const Iterator& other) const {
      if (this == &other) {
        return true;
      }
      return node_ == other.node_;
    }

    bool operator!=(const Iterator& other) const { return !(*this == other); }

    const V& operator*() const {
      assert(node_);
      return node_->value();
    }

    V& operator*() {
      assert(node_);
      return **node_;
    }
  };

  HashMap() { table_init(); }

  HashMap(const HashMap& other) {
    table_init();
    for (auto it = other.begin(); it != other.end(); ++it) {
      insert(it.key(), *it);
    }
  }

  HashMap(HashMap&& other) noexcept { *this = std::move(other); }

  ~HashMap() { free(); }

  Iterator end() const { return Iterator(table_, TABLE_SIZE); }

  Iterator begin() const {
    for (size_t i = 0; i < TABLE_SIZE; ++i) {
      if (table_[i]) {
        return Iterator(table_, i);
      }
    }
    return end();
  }

  Iterator find(const K& key) const {
    const auto index = Hash(key) % TABLE_SIZE;
    auto cur = table_[index];
    while (cur != nullptr) {
      if (Equal(cur->key(), key)) {
        return Iterator(table_, index, cur);
      }
      cur = cur->next();
    }
    return end();
  }

  Iterator insert(const K& key, const V& value) {
    const auto index = Hash(key) % TABLE_SIZE;
    HashNode* prev = nullptr;
    auto cur = table_[index];

    while (cur != nullptr && cur->key() != key) {
      prev = cur;
      cur = cur->next();
    }

    if (cur == nullptr) {
      cur = new HashNode(key, value);
      if (prev == nullptr) {
        table_[index] = cur;
      } else {
        prev->set_next(cur);
      }
    } else {
      **cur = value;
    }
    return Iterator(table_, index, cur);
  }

  void remove(const K& key) {
    const auto index = Hash(key) % TABLE_SIZE;
    HashNode* prev = nullptr;
    auto cur = table_[index];

    while (cur != nullptr && cur->key() != key) {
      prev = cur;
      cur = cur->next();
    }

    if (cur == nullptr) {
      return;
    } else {
      if (prev == nullptr) {
        table_[index] = cur->next();
      } else {
        prev->set_next(cur->next());
      }
      delete cur;
    }
  }

  const V& at(const K& key) const {
    auto it = find(key);
    if (it == end()) {
      throw std::out_of_range("call a non-existent element");
    }
    return *it;
  }

  V& at(const K& key) {
    auto it = find(key);
    if (it == end()) {
      throw std::out_of_range("call a non-existent element");
    }
    return *it;
  }

  HashMap& operator=(const HashMap& other) {
    if (this != &other) {
      free();
      table_init();
      for (auto it = other.begin(); it != other.end(); ++it) {
        insert(it.key(), *it);
      }
    }
    return *this;
  }

  HashMap& operator=(HashMap&& other) noexcept {
    if (this != &other) {
      free();
      table_ = other.table_;
      other.table_ = nullptr;
    }
    return *this;
  }

  V& operator[](const K& key) {
    auto it = find(key);
    if (it == end()) {
      V tmp{};
      it = insert(key, tmp);
    }
    return *it;
  }

  V& operator[](K&& key) { return operator[](key); }
};

}  // namespace libcsc::hashmap
