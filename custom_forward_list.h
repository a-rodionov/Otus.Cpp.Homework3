#pragma once

#include <type_traits>

namespace homework3 {

struct c_fwd_list_node_base
{
  c_fwd_list_node_base() = default;
  c_fwd_list_node_base* next{nullptr};
};

template<typename T>
struct c_fwd_list_node : c_fwd_list_node_base
{
  template<typename ... Args>
  c_fwd_list_node(Args&& ... args)
    : c_fwd_list_node_base{}, value{std::forward<Args>(args)...} {}

  T value;
};

template<typename T>
struct c_fwd_list_iterator
{
  using Self = c_fwd_list_iterator<T>;
  using Node = c_fwd_list_node<T>;

  using value_type = T;
  using pointer = T*;
  using reference = T&;
  using difference_type = ptrdiff_t;
  using iterator_category = std::forward_iterator_tag;

  explicit c_fwd_list_iterator(c_fwd_list_node_base* _node)
    : node{_node} {}

  reference operator*() const
  {
    return static_cast<Node*>(node)->value;
  }

  pointer operator->() const
  {
    return &static_cast<Node*>(node)->value;
  }

  Self& operator++()
  {
    node = node->next;
    return *this;
  }

  Self operator++(int)
  {
    Self tmp{node};
    node = node->next;
    return tmp;
  }

  c_fwd_list_node_base* node{nullptr};
};

template<typename T>
bool operator==(const c_fwd_list_iterator<T>& lhs, const c_fwd_list_iterator<T>& rhs)
{
  return lhs.node == rhs.node;
}

template<typename T>
bool operator!=(const c_fwd_list_iterator<T>& lhs, const c_fwd_list_iterator<T>& rhs)
{
  return lhs.node != rhs.node;
}

template<typename T>
struct c_fwd_list_const_iterator
{
  using Self = c_fwd_list_const_iterator<T>;
  using Node = const c_fwd_list_node<T>;

  using value_type = T;
  using pointer = const T*;
  using reference = const T&;
  using difference_type = ptrdiff_t;
  using iterator_category = std::forward_iterator_tag;

  explicit c_fwd_list_const_iterator(const c_fwd_list_node_base* _node)
    : node{_node} {}

  reference operator*() const
  {
    return static_cast<Node*>(node)->value;
  }

  pointer operator->() const
  {
    return &static_cast<Node*>(node)->value;
  }

  Self& operator++()
  {
    node = node->next;
    return *this;
  }

  Self operator++(int)
  {
    Self tmp{node};
    node = node->next;
    return tmp;
  }

  const c_fwd_list_node_base* node{nullptr};
};

template<typename T>
bool operator==(const c_fwd_list_const_iterator<T>& lhs, const c_fwd_list_const_iterator<T>& rhs)
{
  return lhs.node == rhs.node;
}

template<typename T>
bool operator!=(const c_fwd_list_const_iterator<T>& lhs, const c_fwd_list_const_iterator<T>& rhs)
{
  return lhs.node != rhs.node;
}

template<typename T, typename Allocator = std::allocator<T>>
class custom_forward_list
{
  static_assert(std::is_same<T, typename Allocator::value_type>::value, "Mismatch of custom_forward_list and Allocator template parameter type.");

  using Node = c_fwd_list_node<T>;
  using Allocator_Node = typename Allocator::template rebind<Node>::other;

public:

  using value_type = T;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;

  using iterator = c_fwd_list_iterator<T>;
  using const_iterator = c_fwd_list_const_iterator<T>;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using allocator_type = Allocator;

  custom_forward_list() = default;

  custom_forward_list(const custom_forward_list& other)
  {
    copy(std::cbegin(other), std::cend(other));
  }

  custom_forward_list(custom_forward_list&& other)
    : allocator{std::forward<Allocator_Node>(other.allocator)}
  {
    head.next = other.head.next;
    other.head.next = nullptr;
  }

  ~custom_forward_list()
  {
    clear();
  };

  custom_forward_list& operator=(const custom_forward_list& other)
  {
    clear();
    copy(std::cbegin(other), std::cend(other));
  }

  custom_forward_list& operator=(custom_forward_list&& other)
  {
    clear();
    head.next = other.head.next;
    other.head.next = nullptr;
    allocator = std::move(other.allocator);
  }

  bool operator==(const custom_forward_list& other) const
  {
    return std::equal(std::cbegin(*this), std::cend(*this), std::cbegin(other), std::cend(other));
  }

  bool operator!=(const custom_forward_list& other) const
  {
    return !std::equal(std::cbegin(*this), std::cend(*this), std::cbegin(other), std::cend(other));
  }

  void swap(custom_forward_list& other)
  {
    custom_forward_list tmp{std::move(other)};
    other = std::move(*this);
    *this = std::move(tmp);
  }

  void push_front(const T& value)
  {
    Node* node = allocator.allocate(1);
    allocator.construct(node, value);
    node->next = head.next;
    head.next = node;
  }

  void push_front(T&& value)
  {
    Node* node = allocator.allocate(1);
    allocator.construct(node, std::forward<T>(value));
    node->next = head.next;
    head.next = node;
  }

  void pop_front()
  {
    Node* node = static_cast<Node*>(head.next);
    head.next = node->next;
    allocator.destroy(node);
    allocator.deallocate(node, 1);
  }

  reference front()
  {
    return static_cast<Node*>(head.next)->value;
  }

  const_reference front() const
  {
    return static_cast<Node*>(head.next)->value;
  }

  bool empty() const
  {
    if(nullptr == head.next)
      return true;
    return false;
  }

  size_type size() const noexcept
  {
    return std::distance(std::cbegin(*this), std::cend(*this));
  }

  void clear() noexcept
  {
    while(nullptr != head.next)
      pop_front();
  }

  iterator begin() noexcept
  {
    return iterator{head.next};
  }

  const_iterator begin() const noexcept
  {
    return const_iterator{head.next};
  }

  iterator end() noexcept
  {
    return iterator{nullptr};
  }

  const_iterator end() const noexcept
  {
    return const_iterator{nullptr};
  }

  const_iterator cbegin() const noexcept
  {
    return const_iterator{head.next};
  }

  const_iterator cend() const noexcept
  {
    return const_iterator{nullptr};
  }

private:

  void copy(const_iterator first, const_iterator last)
  {
    auto size = std::distance(first, last);
    while(0 < size)
    {
      auto first_copy = first;
      for(decltype(size) i = 0; i < size - 1; ++i)
        ++first_copy;
      push_front(*first_copy);
      --size;
    }
  }

  c_fwd_list_node_base  head;
  Allocator_Node        allocator;

};

template<typename T, typename Allocator>
void swap(custom_forward_list<T, Allocator>& lhs, custom_forward_list<T, Allocator>& rhs)
{
  lhs.swap(rhs);
}

}
