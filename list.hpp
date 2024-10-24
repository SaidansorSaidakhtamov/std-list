#include <iostream>

template <typename T, typename Allocator = std::allocator<T>>
class List {
 protected:
  struct BaseNode {
    BaseNode* next;
    BaseNode* prev;
    BaseNode() : next(nullptr), prev(nullptr) {}
    BaseNode(BaseNode* prev_node, BaseNode* next_node)
        : next(next_node), prev(prev_node) {}
    ~BaseNode() = default;
  };
  struct Node : public BaseNode {
    T value;
    using BaseNode::next;
    using BaseNode::prev;
    Node() : BaseNode(), value() {}
    Node(const T& val) : BaseNode(), value(val) {}
    Node(T&& val) : BaseNode(), value(std::move(val)) {}
    ~Node() = default;
  };

 public:
  using value_type = T;
  using allocator_type = Allocator;

  List();
  List(size_t count, const T& value, const Allocator& alloc = Allocator());
  explicit List(size_t count, const Allocator& alloc = Allocator());
  List(const List& other);
  List(std::initializer_list<T> init, const Allocator& alloc = Allocator());
  List<T, Allocator>& operator=(const List<T, Allocator>& other);
  ~List() {
    clear();
    basenode_traits::deallocate(base_alloc_, head_, 1);
  }
  Allocator& get_allocator() { return alloc_; }

  using alloc_traits = std::allocator_traits<Allocator>;
  using node_allocator = typename alloc_traits::template rebind_alloc<Node>;
  using basenode_allocator = typename std::allocator_traits<
      Allocator>::template rebind_alloc<BaseNode>;
  using node_traits = std::allocator_traits<node_allocator>;
  using basenode_traits = std::allocator_traits<basenode_allocator>;

  template <bool IsConst>
  class CommonIterator {
   public:
    using iterator_category = std::bidirectional_iterator_tag;
    using pointer = std::conditional_t<IsConst, const T*, T*>;
    using reference = std::conditional_t<IsConst, const T&, T&>;
    using const_reference = const T&;
    using difference_type = std::ptrdiff_t;
    using value_type = std::conditional_t<IsConst, const T, T>;

    CommonIterator() : current_(nullptr) {}
    CommonIterator(const CommonIterator* other)
        : current_(const_cast<BaseNode*>(other->current_)) {}
    CommonIterator(Node* node) : current_(node) {}
    CommonIterator& operator++();
    CommonIterator& operator--();
    CommonIterator operator++(int);
    CommonIterator operator--(int);
    std::conditional_t<IsConst, const T&, T&> operator*() {
      return static_cast<std::conditional_t<IsConst, const Node*, Node*>>(
          current_)
          ->value;
    }
    std::conditional_t<IsConst, const T*, T*> operator->() {
      return &(*(*this));
    }
    bool operator!=(const CommonIterator<IsConst>& argument) const {
      return !(*this == argument);
    }
    bool operator==(const CommonIterator<IsConst>& argument) const {
      return current_ == argument.current_;
    }

   private:
    std::conditional_t<IsConst, const Node*, Node*> current_;
    CommonIterator<true> cast() { return {const_cast<BaseNode*>(current_)}; }
  };

  bool empty() const { return size_ == 0; }
  size_t size() const { return size_; }
  void push_back(const T& elem);
  void push_front(const T& elem);
  void push_back(T&& elem);
  void push_front(T&& elem);
  void pop_back();
  void pop_front();
  T& front() { return *(head_->prev); }
  const T& front() const { return *(head_->prev); }
  T& back() { return *(head_->next); }

  const T& back() const { return *(head_->next); }

  using iterator = CommonIterator<false>;
  using const_iterator = CommonIterator<true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  iterator begin() const { return iterator(static_cast<Node*>(head_->next)); }
  const_iterator cbegin() {
    return const_iterator(static_cast<Node*>(head_->next));
  }
  iterator end() const { return iterator(static_cast<Node*>(head_)); }
  const_iterator cend() const {
    return const_iterator(static_cast<Node*>(head_));
  }
  reverse_iterator rbegin() const { return reverse_iterator(--end()); }
  const_reverse_iterator crbegin() const {
    return const_reverse_iterator(--end());
  }
  reverse_iterator rend() const { return reverse_iterator(--begin()); }
  const_reverse_iterator crend() const {
    return const_reverse_iterator(--begin());
  }

 private:
  void clear();
  BaseNode* head_;
  size_t size_ = 0;
  Allocator alloc_;
  basenode_allocator base_alloc_;
  node_allocator node_alloc_;
};