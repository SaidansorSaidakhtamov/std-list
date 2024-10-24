#include "list.hpp"

template <typename T, typename Allocator>
List<T, Allocator>& List<T, Allocator>::operator=(const List& other) {
  if (this == &other) {
    return *this;
  }
  alloc_ = alloc_traits::propagate_on_container_copy_assignment::value
           ? other.alloc_
           : alloc_;
  node_alloc_ = node_traits::propagate_on_container_copy_assignment::value
                ? other.node_alloc_
                : node_alloc_;
  base_alloc_ = basenode_traits::propagate_on_container_copy_assignment::value
                ? other.base_alloc_
                : base_alloc_;
  if (size_ == other.size_) {
    auto iter_this = begin();
    for (const auto& iter : other) {
      *iter_this = iter;
      ++iter_this;
    }
  } else if (size_ < other.size_) {
    auto iter_other = other.begin();
    for (auto& iter : *this) {
      iter = *iter_other;
      ++iter_other;
    }
    for (; iter_other != other.end(); ++iter_other) {
      push_back(*iter_other);
    }
    size_ = other.size_;
  } else {
    auto iter_this = begin();
    for (const auto& iter : other) {
      *iter_this = iter;
      ++iter_this;
    }
    size_t tmp_size = size_;
    for (size_t i = tmp_size; i > other.size_; --i) {
      pop_back();
    }
  }
  return *this;
}

template <typename T, typename Allocator>
List<T, Allocator>::List(std::initializer_list<T> init, const Allocator& alloc)
    : size_(0), alloc_(alloc), node_alloc_(alloc), base_alloc_(alloc) {
  head_ = basenode_traits::allocate(base_alloc_, 1);
  head_->next = head_;
  head_->prev = head_;
  try {
    for (const auto& iter : init) {
      try {
        push_back(iter);
      } catch (...) {
        clear();
        throw;
      }
    }
  } catch (...) {
    basenode_traits::destroy(base_alloc_, head_);
    basenode_traits::deallocate(base_alloc_, head_, 1);
    throw;
  }
}

template <typename T, typename Allocator>
List<T, Allocator>::List(const List& other)
    : size_(0),
      alloc_(alloc_traits::select_on_container_copy_construction(other.alloc_)),
      node_alloc_(node_traits::select_on_container_copy_construction(
          other.node_alloc_)),
      base_alloc_(basenode_traits::select_on_container_copy_construction(
          other.base_alloc_)) {
  head_ = basenode_traits::allocate(base_alloc_, 1);
  head_->next = head_;
  head_->prev = head_;
  try {
    for (const auto& iter : other) {
      try {
        push_back(iter);
      } catch (...) {
        clear();
        throw;
      }
    }
  } catch (...) {
    basenode_traits::destroy(base_alloc_, head_);
    basenode_traits::deallocate(base_alloc_, head_, 1);
    throw;
  }
}

template <typename T, typename Allocator>
void List<T, Allocator>::pop_front() {
  Node* tmp = static_cast<Node*>(head_->next);
  head_->next = tmp->next;
  tmp->next->prev = head_;
  node_traits::destroy(node_alloc_, tmp);
  node_traits::deallocate(node_alloc_, tmp, 1);
  --size_;
}

template <typename T, typename Allocator>
void List<T, Allocator>::pop_back() {
  BaseNode* tmp = head_->prev;
  head_->prev = tmp->prev;
  tmp->prev->next = head_;
  node_traits::destroy(node_alloc_, static_cast<Node*>(tmp));
  node_traits::deallocate(node_alloc_, static_cast<Node*>(tmp), 1);
  --size_;
}

template <typename T, typename Allocator>
void List<T, Allocator>::push_front(T&& elem) {
  Node* tmp = node_traits::allocate(node_alloc_, 1);
  try {
    node_traits::construct(node_alloc_, tmp, std::move(elem));
    BaseNode* temp = head_->next;
    temp->prev = tmp;
    tmp->next = temp;
    tmp->prev = head_;
    head_->next = tmp;
    ++size_;
  } catch (...) {
    node_traits::deallocate(node_alloc_, tmp, 1);
    throw;
  }
}

template <typename T, typename Allocator>
void List<T, Allocator>::push_front(const T& elem) {
  Node* tmp = node_traits::allocate(node_alloc_, 1);
  try {
    node_traits::construct(node_alloc_, tmp, elem);
    BaseNode* temp = head_->next;
    temp->prev = tmp;
    tmp->next = temp;
    tmp->prev = head_;
    head_->next = tmp;
    ++size_;
  } catch (...) {
    node_traits::deallocate(node_alloc_, tmp, 1);
    throw;
  }
}

template <typename T, typename Allocator>
void List<T, Allocator>::clear() {
  size_t tmp = size_;
  for (size_t i = 0; i < tmp; ++i) {
    pop_back();
  }
}

template <typename T, typename Allocator>
void List<T, Allocator>::push_back(T&& elem) {
  Node* tmp = node_traits::allocate(node_alloc_, 1);
  try {
    node_traits::construct(node_alloc_, tmp, std::move(elem));
    BaseNode* temp = head_->prev;
    temp->next = tmp;
    tmp->prev = temp;
    tmp->next = head_;
    head_->prev = tmp;
    ++size_;
  } catch (...) {
    node_traits::deallocate(node_alloc_, tmp, 1);
    throw;
  }
}

template <typename T, typename Allocator>
void List<T, Allocator>::push_back(const T& elem) {
  Node* tmp = node_traits::allocate(node_alloc_, 1);
  try {
    node_traits::construct(node_alloc_, tmp, elem);
    BaseNode* temp = head_->prev;
    temp->next = tmp;
    tmp->prev = temp;
    tmp->next = head_;
    head_->prev = tmp;
    ++size_;
  } catch (...) {
    node_traits::deallocate(node_alloc_, tmp, 1);
    throw;
  }
}

template <typename T, typename Allocator>
List<T, Allocator>::List(size_t count, const Allocator& alloc)
    : size_(0), alloc_(alloc), node_alloc_(alloc), base_alloc_(alloc) {
  head_ = basenode_traits::allocate(base_alloc_, 1);
  head_->next = head_;
  head_->prev = head_;
  try {
    for (size_t i = 0; i < count; ++i) {
      Node* tmp = node_traits::allocate(node_alloc_, 1);
      try {
        node_traits::construct(node_alloc_, tmp);
        BaseNode* temp = head_->prev;
        temp->next = tmp;
        tmp->prev = temp;
        tmp->next = head_;
        head_->prev = tmp;
        ++size_;
      } catch (...) {
        node_traits::deallocate(node_alloc_, tmp, 1);
        throw;
      }
    }
  } catch (...) {
    clear();
    basenode_traits::destroy(base_alloc_, head_);
    basenode_traits::deallocate(base_alloc_, head_, 1);
    throw;
  }
}

template <typename T, typename Allocator>
List<T, Allocator>::List(size_t count, const T& value, const Allocator& alloc)
    : size_(0), alloc_(alloc), node_alloc_(alloc), base_alloc_(alloc) {
  head_ = basenode_traits::allocate(base_alloc_, 1);
  head_->next = head_;
  head_->prev = head_;
  try {
    for (size_t i = 0; i < count; ++i) {
      try {
        push_back(value);
      } catch (...) {
        clear();
        throw;
      }
    }
  } catch (...) {
    clear();
    basenode_traits::destroy(base_alloc_, head_);
    basenode_traits::deallocate(base_alloc_, head_, 1);
    throw;
  }
}

template <typename T, typename Allocator>
List<T, Allocator>::List()
    : size_(0),
      alloc_(Allocator()),
      node_alloc_(Allocator()),
      base_alloc_(Allocator()) {
  head_ = basenode_traits::allocate(base_alloc_, 1);
  head_->next = head_;
  head_->prev = head_;
}

template <typename T, typename Allocator>
template <bool IsConst>

typename List<T, Allocator>::template CommonIterator<IsConst>
List<T, Allocator>::CommonIterator<IsConst>::operator--(int) {
  CommonIterator tmp = *this;
  current_ = reinterpret_cast<Node*>(current_->prev);
  return tmp;
}

template <typename T, typename Allocator>
template <bool IsConst>
typename List<T, Allocator>::template CommonIterator<IsConst>
List<T, Allocator>::CommonIterator<IsConst>::operator++(int) {
  CommonIterator tmp = *this;
  current_ = reinterpret_cast<Node*>(current_->next);
  return tmp;
}

template <typename T, typename Allocator>
template <bool IsConst>
typename List<T, Allocator>::template CommonIterator<IsConst>&
List<T, Allocator>::CommonIterator<IsConst>::operator--() {
  current_ = reinterpret_cast<Node*>(current_->prev);
  return *this;
}

template <typename T, typename Allocator>
template <bool IsConst>
typename List<T, Allocator>::template CommonIterator<IsConst>&
List<T, Allocator>::CommonIterator<IsConst>::operator++() {
  current_ = reinterpret_cast<Node*>(current_->next);
  return *this;
}