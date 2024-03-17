#pragma once

#include <iterator>
#include <utility>

namespace csc {

template <typename T>
class List {
public:
    template <typename U>
    class Iterator;

    template <typename U>
    class ReverseIterator;

    using value_type = T;
    using reference = T&;
    using const_reference = const T&;
    using pointer = T*;
    using const_pointer = const T*;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    using iterator = Iterator<T>;
    using const_iterator = Iterator<const T>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    struct Node {
    public:
        Node(const T& data, Node* next, Node* prev)
            : next_(next)
            , prev_(prev)
            , data_(data)
        {
        }
        ~Node() = default;

        Node* next_;
        Node* prev_;
        T data_;
    };

    template <typename U>
    class Iterator {
    public:
        friend class List;
        using self = Iterator<U>;
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = U;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type*;
        using reference = value_type&;

        explicit Iterator(Node* node)
            : node_(node)
        {
        }

        auto& operator++() noexcept
        {
            node_ = node_->next_;
            return *this;
        }

        auto operator++(int) noexcept
        {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        auto& operator--() noexcept
        {
            node_ = node_->prev_;
            return *this;
        }

        auto operator--(int) noexcept
        {
            auto tmp = *this;
            --(*this);
            return tmp;
        }

        reference operator*() const noexcept { return node_->data_; }

        pointer operator->() const noexcept { return &node_->data_; }

        friend bool operator==(const self& lhs, const self& rhs)
        {
            return lhs.node_ == rhs.node_;
        }

    private:
        Node* node_;
    };

    List() = default;

    List(iterator begin, iterator end)
    {
        for (auto it = begin; it != end; ++it) {
            push_back(*it);
        }
    }

    List(const_iterator cbegin, const_iterator cend)
    {
        for (auto it = cbegin; it != cend; ++it) {
            push_back(*it);
        }
    }

    List(std::initializer_list<T> list)
    {
        for (const auto& item : list) {
            push_back(item);
        }
    }

    List(const List& other)
    {
        for (const auto& item : other) {
            push_back(item);
        }
    }

    List(List&& other) noexcept
    {
        std::swap(head_, other.head_);
        std::swap(tail_, other.tail_);
        std::swap(size_, other.size_);
        other.head_ = nullptr;
        other.tail_ = nullptr;
    }

    List& operator=(const List& other)
    {
        if (this != std::addressof(other)) {
            List tmp(other);
            std::swap(head_, tmp.head_);
            std::swap(tail_, tmp.tail_);
            std::swap(size_, tmp.size_);
        }
        return *this;
    }

    List& operator=(List&& other) noexcept
    {
        if (this != std::addressof(other)) {
            std::swap(head_, other.head_);
            std::swap(tail_, other.tail_);
            std::swap(size_, other.size_);
        }
        return *this;
    }

    ~List() { clear(); }

    iterator begin() const noexcept { return iterator(head_); }
    iterator end() const noexcept
    {
        return iterator(tail_ ? tail_->next_ : nullptr);
    }

    const_iterator cbegin() const noexcept { return const_iterator(head_); }
    const_iterator cend() const noexcept
    {
        return const_iterator(tail_ ? tail_->next_ : nullptr);
    }

    reverse_iterator rbegin() const noexcept { return reverse_iterator(end()); }
    reverse_iterator rend() const noexcept { return reverse_iterator(begin()); }

    const_reverse_iterator crbegin() const noexcept
    {
        return const_reverse_iterator(cend());
    }
    const_reverse_iterator crend() const noexcept
    {
        return const_reverse_iterator(cbegin());
    }

    void push_back(T data)
    {
        if (head_ == nullptr) {
            head_ = tail_ = new Node(std::move(data), nullptr, nullptr);
            tail_->next_ = new Node(T(), nullptr, tail_);
        } else {
            tail_ = tail_->next_;
            tail_->data_ = std::move(data);
            tail_->next_ = new Node(T(), nullptr, tail_);
        }
        ++size_;
    }

    void push_front(T data)
    {
        if (head_ == nullptr) {
            head_ = tail_ = new Node(std::move(data), nullptr, nullptr);
            tail_->next_ = new Node(T(), nullptr, tail_);
        } else {
            auto node = new Node(std::move(data), head_, nullptr);
            head_->prev_ = node;
            head_ = node;
        }
        ++size_;
    }

    iterator erase(iterator pos)
    {
        if (size() == 1) {
            clear();
            return iterator(head_);
        }

        auto node = pos.node_;
        node->prev_->next_ = node->next_;
        node->next_->prev_ = node->prev_;
        auto next = node->next_;
        delete node;
        --size_;
        return iterator(next);
    }

    reference front() const noexcept { return head_->data_; }

    reference back() const noexcept { return tail_->data_; }

    bool empty() const noexcept { return head_ == nullptr; }

    size_type size() const noexcept { return size_; }

    void clear() noexcept
    {
        while (head_ != nullptr) {
            delete std::exchange(head_, head_->next_);
        }
        head_ = tail_ = nullptr;
        size_ = 0;
    }

private:
    Node* head_ = nullptr;
    Node* tail_ = nullptr;
    size_type size_ = 0;
};

} // namespace csc
