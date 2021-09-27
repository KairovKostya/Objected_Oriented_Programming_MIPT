#include<iostream>
#include<vector>
#include <type_traits>
#include <list>
#include <new>
#include <memory>

template<size_t chunkSize>
class FixedAllocator {
private:
    size_t free_place = 0;
    size_t BlockSize = 10;
    size_t MaxBlockSize = 500000;

    struct Block {
        size_t sz;
        char *start;
        char *now;
        ~Block() {
            ::operator delete(start, sz * chunkSize);
        }
        Block(size_t sz) : sz(sz) {
            start = reinterpret_cast<char *>(::operator new(sz * chunkSize));
            now = start;
        }
    };
    std::vector<Block *> places;
public:
    FixedAllocator() {}

    void *allocate();

    void deallocate(void *) {
        //do nothing
    }

    ~FixedAllocator() {
        for (auto memory: places)
            memory->~Block();
        //::operator delete(memory);
    }
};

template<size_t chunkSize>
void* FixedAllocator<chunkSize>::allocate() {
    if (free_place <= 0) BlockSize *= 2;
        if(BlockSize*2 <= MaxBlockSize)
        places.push_back(new Block(BlockSize));
        char *place = places.back()->now;
        places.back()->now = reinterpret_cast<char *>(places.back()->now + chunkSize);
        free_place = BlockSize - 1;
        return place;
    }
    --free_place;
    void *new_place = reinterpret_cast<void *>(places.back()->now);
    places.back()->now = reinterpret_cast<char *>(places.back()->now + chunkSize);
    return new_place;
}


template<typename T>
class FastAllocator {
private:
    std::shared_ptr<FixedAllocator<sizeof(T)>> my_fixed_alloc;
public:
    using value_type = T;
    using pointer = T *;
    template<typename U>
    struct rebind {
        using other = FastAllocator<U>;
    };

    FastAllocator() {
        my_fixed_alloc = std::make_shared<FixedAllocator<sizeof(T)>>();
    }

    FastAllocator(const FastAllocator<T> &) : FastAllocator() {
        my_fixed_alloc = std::make_shared<FixedAllocator<sizeof(T)>>();
    };

    template<typename U>
    FastAllocator(const FastAllocator<U> &):FastAllocator() {
        my_fixed_alloc = std::make_shared<FixedAllocator<sizeof(T)>>();
    }

    pointer allocate(size_t n);

    void deallocate(T *ptr, size_t n);

};

template<typename T, typename U>
bool operator==(const FastAllocator<T> &, const FastAllocator<U> &) { return true; }

template<typename T, typename U>
bool operator!=(const FastAllocator<T> &, const FastAllocator<U> &) { return false; }

template<typename T>
typename FastAllocator<T>::pointer FastAllocator<T>::allocate(size_t n) {
    if (n == 1) {
        return static_cast<T *>(my_fixed_alloc->allocate());
    }
    return static_cast<T *>(::operator new(n * sizeof(T)));
}

template<typename T>
void FastAllocator<T>::deallocate(T *ptr, size_t n) {
    if (n == 1) {
        my_fixed_alloc->deallocate(ptr);
    } else {
        ::operator delete(ptr, n * sizeof(T));
    }
}


//allocator

template<typename T, typename Allocator = std::allocator<T>>
class List {
public:
    using AllocTraits = std::allocator_traits<Allocator>;
private:

    size_t sz;
    //Allocator alloc;


    struct Node {
        T data;
        Node *prev;
        Node *next;

        Node() : data(T()) {}

        Node(const T &data_) : data(data_) {}

        Node(const T &data_, Node *prev, Node *next) : data(data_), prev(prev), next(next) {}

        Node(Node *prev, Node *next) : data(T()), prev(prev), next(next) {}
    };

    typename AllocTraits::template rebind_alloc<Node> alloc_node;

    //pakost'
    template<bool IsConst>
    struct common_iterator {
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = std::conditional_t<IsConst, const T *, T *>;
        using reference = std::conditional_t<IsConst, const T &, T &>;

        Node *ptr;

        common_iterator &operator++() {
            ptr = ptr->next;
            return *this;
        }

        common_iterator operator++(int) {
            auto copy = *this;
            ++*this;
            return copy;
        }

        common_iterator &operator--() {
            ptr = ptr->prev;
            return *this;
        }

        common_iterator operator--(int) {
            auto copy = *this;
            --*this;
            return copy;
        }

        common_iterator(Node *node) {
            ptr = node;
        }


        reference operator*() {
            return ptr->data;
        }

        pointer operator->() {
            return &(ptr->data);
        }

        bool operator==(const common_iterator &other) {
            return ptr == other.ptr;
        }

        bool operator!=(const common_iterator &other) {
            return ptr != other.ptr;
        }

        template<bool IsConstOther>
        explicit operator common_iterator<IsConstOther>() const {
            static_assert(IsConstOther == IsConst || IsConstOther);
            return common_iterator<IsConstOther>(this->ptr);
        }

    };
    //pakost'


    Node *first_empty;
    Node *last_empty;

public:
    using iterator = common_iterator<false>;
    using const_iterator = common_iterator<true>;
    using reverse_iterator = std::reverse_iterator<common_iterator<false>>;
    using const_reverse_iterator = std::reverse_iterator<common_iterator<true>>;

    template<typename Iterator>
    void insert(Iterator it, const T &value) {
        ++sz;
        Node *new_node = AllocTraitsNode::allocate(alloc_node, 1);
        AllocTraitsNode::construct(alloc_node, new_node, value, it.ptr->prev, it.ptr);
        it.ptr->prev->next = new_node;
        it.ptr->prev = new_node;
    }

    template<typename Iterator>
    void erase(Iterator it) {
        --sz;
        it.ptr->prev->next = it.ptr->next;
        it.ptr->next->prev = it.ptr->prev;
        AllocTraitsNode::destroy(alloc_node, it.ptr);
        AllocTraitsNode::deallocate(alloc_node, it.ptr, 1);
    }

    iterator begin() {
        return iterator(first_empty->next);
    }

    iterator end() {
        return iterator(last_empty);
    }

    const_iterator cbegin() const {
        return const_iterator(first_empty->next);
    }

    const_iterator cend() const {
        return const_iterator(last_empty);
    }

    const_iterator begin() const {
        return const_iterator(first_empty->next);
    }

    const_iterator end() const {
        return const_iterator(last_empty);
    }

    reverse_iterator rbegin() {
        return std::make_reverse_iterator(end());
    }

    reverse_iterator rend() {
        return std::make_reverse_iterator(begin());
    }

    const_reverse_iterator rbegin() const {
        return std::make_reverse_iterator(cend());
    }

    const_reverse_iterator rend() const {
        return std::make_reverse_iterator(cbegin());
    }

    const_reverse_iterator crbegin() const {
        return std::make_reverse_iterator(cend());
    }

    const_reverse_iterator crend() const {
        return std::make_reverse_iterator(cbegin());
    }

    using AllocTraitsNode = std::allocator_traits<typename AllocTraits::template rebind_alloc<Node>>;

    explicit List(const Allocator &alloc = Allocator()) : sz(0), alloc_node(alloc) {
        first_empty = AllocTraitsNode::allocate(alloc_node, 1);
        last_empty = AllocTraitsNode::allocate(alloc_node, 1);
        last_empty->prev = first_empty;
        first_empty->next = last_empty;
        first_empty->prev = nullptr;
        last_empty->next = nullptr;
    }

    List(const List &another);

    List(size_t count, const T &value, const Allocator &alloc = Allocator());

    List(size_t count, const Allocator &alloc = Allocator());

    List &operator=(const List &another);

    ~List() {
        while (sz > 0) {
            pop_front();
        }

        AllocTraitsNode::deallocate(alloc_node, first_empty, 1);
        AllocTraitsNode::deallocate(alloc_node, last_empty, 1);
    }

    Allocator get_allocator() const { return alloc_node; }

    size_t size() const { return sz; }

    void push_back(const T &value);

    void pop_back();

    void pop_front();

    void push_front(const T &value);
};

template<typename T, typename Allocator>
List<T, Allocator> &List<T, Allocator>::operator=(const List<T, Allocator> &another) {
    while (sz > 0) {
        pop_back();
    }
    if (std::allocator_traits<Allocator>::propagate_on_container_copy_assignment::value) {
        alloc_node = another.alloc_node;
    }
    if (another.size() == 0) return *this;
    sz = 0;
    for (auto it = another.cbegin(); it != another.cend(); ++it) {
        push_back(*it);
    }
    return *this;
}

template<typename T, typename Allocator>
List<T, Allocator>::List(const List<T, Allocator> &another) :
        List(std::allocator_traits<Allocator>::select_on_container_copy_construction(another.alloc_node)) {
    if (another.size() == 0) return;
    for (auto it = another.cbegin(); it != another.cend(); ++it) {
        push_back(*it);
    }
}

template<typename T, typename Allocator>
void List<T, Allocator>::pop_back() {
    if (sz > 0) {
        erase(--end());
    }
}

template<typename T, typename Allocator>
void List<T, Allocator>::pop_front() {
    if (sz > 0) {
        erase(begin());
    }
}


template<typename T, typename Allocator>
void List<T, Allocator>::push_back(const T &value) {
    insert(end(), value);
}

template<typename T, typename Allocator>
void List<T, Allocator>::push_front(const T &value) {
    insert(begin(), value);
}

template<typename T, typename Allocator>
List<T, Allocator>::List(size_t count, const T &value, const Allocator &alloc): sz(0), alloc_node(alloc) {
    first_empty = AllocTraitsNode::allocate(alloc_node, 1);
    last_empty = AllocTraitsNode::allocate(alloc_node, 1);
    first_empty->next = last_empty;
    last_empty->prev = first_empty;
    first_empty->prev = nullptr;
    last_empty->next = nullptr;
    for (size_t i = 0; i < count; ++i) {
        push_back(value);
    }
}

template<typename T, typename Allocator>
List<T, Allocator>::List(size_t count, const Allocator &alloc): sz(count), alloc_node(alloc) {
    first_empty = AllocTraitsNode::allocate(alloc_node, 1);
    last_empty = AllocTraitsNode::allocate(alloc_node, 1);
    first_empty->next = last_empty;
    last_empty->prev = first_empty;
    first_empty->prev = nullptr;
    last_empty->next = nullptr;

    for (size_t i = 0; i < count; ++i) {
        Node *new_node = AllocTraitsNode::allocate(alloc_node, 1);
        AllocTraitsNode::construct(alloc_node, new_node, last_empty->prev, last_empty);
        last_empty->prev->next = (new_node);
        last_empty->prev = (new_node);
    }
}
