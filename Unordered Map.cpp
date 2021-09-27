#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <type_traits>
#include <new>
#include <memory>

template<size_t chunkSize>
class FixedAllocator {
private:
    size_t free_place = 0;
    size_t BlockSize = 400000;

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

    void deallocate(void *);

    ~FixedAllocator() {
        for (auto memory: places)
            delete memory;
    }
};

template<size_t chunkSize>
void* FixedAllocator<chunkSize>::allocate() {
    if (free_place <= 0) {
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

template<size_t chunkSize>
void FixedAllocator<chunkSize>::deallocate(void *) {
    //do nothing
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

    struct Node {
        T data;
        Node *prev;
        Node *next;

        Node() : data(T()) {}
        Node(Node&& another) : data(std::move(another.data)), prev(std::move(another.prev)), next(std::move(another.next)) {}
        Node(const T &data_, Node *prev, Node *next) : data(data_), prev(prev), next(next) {}
        Node(Node *prev, Node *next) : data(T()), prev(prev), next(next) {}
        template<typename... Args>
        Node(Args&&... args): data(std::forward<Args>(args)...), prev(nullptr), next(nullptr){}
    };

    typename AllocTraits::template rebind_alloc<Node> alloc_node;

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
    iterator insert(iterator it, Node&&);

    template<typename Iterator>
    void erase(Iterator it) {
        --sz;

        it.ptr->prev->next = it.ptr->next;
        it.ptr->next->prev = it.ptr->prev;

        //AllocTraitsNode::destroy(alloc_node, it.ptr);
        //AllocTraitsNode::deallocate(alloc_node, it.ptr, 1);
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

    List(List&&);
    List& operator=(List&&);

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

    template<typename... Args>
    void emplace(iterator it, Args&&... args);
};

template<typename T, typename Allocator>
typename List<T, Allocator>::iterator List<T, Allocator>::insert(typename List<T, Allocator>::iterator  it,  Node&& node) {
    ++sz;
    Node *new_node = AllocTraitsNode::allocate(alloc_node, 1);
    AllocTraitsNode::construct(alloc_node, new_node, node.data, it.ptr->prev, it.ptr);
    it.ptr->prev->next = new_node;
    it.ptr->prev = new_node;
    return it;
}

template<typename T, typename Allocator>
template<typename... Args>
void List<T, Allocator>::emplace(typename List<T, Allocator>::iterator it, Args &&... args) {
    ++sz;
    Node *new_node = AllocTraitsNode::allocate(alloc_node, 1);
    Allocator alloc;
    AllocTraitsNode::construct(alloc_node, new_node, std::forward<Args>(args)...);
    new_node->prev = it.ptr->prev;
    new_node->next = it.ptr;
    it.ptr->prev->next = new_node;
    it.ptr->prev = new_node;
}

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
List<T, Allocator>::List(List&& another) {
    sz = 0;
    first_empty = AllocTraitsNode::allocate(alloc_node, 1);
    last_empty = AllocTraitsNode::allocate(alloc_node, 1);
    last_empty->prev = first_empty;
    first_empty->next = last_empty;
    first_empty->prev = nullptr;
    last_empty->next = nullptr;
    std::swap(sz, another.sz);
    std::swap(first_empty, another.first_empty);
    std::swap(last_empty, another.last_empty);
}

template<typename T, typename Allocator>
List<T, Allocator>& List<T, Allocator>::operator=(List&& another){
    if(&another == this){
        return *this;
    }
    while(sz > 0){
        pop_front();
    }
    std::swap(sz, another.sz);
    std::swap(first_empty, another.first_empty);
    std::swap(last_empty, another.last_empty);
    return *this;
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





///UnorderedMap





template<typename Key, typename Value, typename Hash = std::hash<Key>,
        typename Equal = std::equal_to<Key>, typename Allocator = std::allocator<std::pair<const Key, Value>>>
class UnorderedMap {
public:
    using AllocTraits = std::allocator_traits<Allocator>;
    using NodeType = std::pair<const Key, Value>;
private:
    const double max_load_factor_ = 1;
    const size_t max_sz = 10000000;
    List<NodeType, Allocator> table;
    std::vector<typename List<NodeType, Allocator>::iterator> ptrs;
    Allocator alloc_node;
    size_t my_sz = 1000;
    Hash hash;
    Equal equal;

public:
    UnorderedMap(const Allocator &alloc = Allocator());
    UnorderedMap(const UnorderedMap&);
    UnorderedMap(UnorderedMap&&);
    UnorderedMap& operator=(UnorderedMap&&);
    UnorderedMap& operator=(const UnorderedMap&);
    ~UnorderedMap() = default;

    Value& operator[](const Key& key);
    Value& at(const Key& key);

    size_t size() const;
    size_t max_size() const;
    double max_load_factor() const;
    double load_factor() const;

    struct Iterator : public List<NodeType, Allocator>::iterator{
        Iterator(const typename List<NodeType, Allocator>::iterator& it): List<NodeType, Allocator>::iterator(it){}
    };
    struct ConstIterator : public List<NodeType, Allocator>::const_iterator {
        ConstIterator(const typename List<NodeType, Allocator>::const_iterator& it):
                List<NodeType, Allocator>::const_iterator(it){}
        ConstIterator(const Iterator& it):
                List<NodeType, Allocator>::const_iterator(it){}
    };
    Iterator begin();
    Iterator end();
    ConstIterator begin() const;
    ConstIterator end() const;
    ConstIterator cbegin() const;
    ConstIterator cend() const;
    Iterator find(const Key&);

    std::pair<Iterator, bool> insert(NodeType&&);
    std::pair<Iterator, bool> insert(const NodeType&);
    template<typename InputIterator>
    void insert(InputIterator first, InputIterator second);
    void erase(Iterator);
    void erase(Iterator first, Iterator second);
    template<typename... Args>
    std::pair<Iterator, bool> emplace(Args&&... args);
    void reserve(size_t);
    void rehash(size_t);
};

template<typename Key, typename Val, typename Hash,typename Equal, typename Alloc>
size_t UnorderedMap<Key,Val,Hash,Equal,Alloc>::size() const{
    return table.size();
}
template<typename Key, typename Val, typename Hash,typename Equal, typename Alloc>
size_t UnorderedMap<Key,Val,Hash,Equal,Alloc>::max_size() const{
    return max_sz;
}
template<typename Key, typename Val, typename Hash,typename Equal, typename Alloc>
double UnorderedMap<Key,Val,Hash,Equal,Alloc>::max_load_factor() const {
    return max_load_factor_;
}
template<typename Key, typename Val, typename Hash,typename Equal, typename Alloc>
double UnorderedMap<Key,Val,Hash,Equal,Alloc>::load_factor() const {
    if(ptrs.size() == 0) return 0;
    return static_cast<double>(table.size())/ptrs.size();
}
template<typename Key, typename Val, typename Hash,typename Equal, typename Alloc>
std::pair<typename UnorderedMap<Key,Val,Hash,Equal,Alloc>::Iterator, bool>
UnorderedMap<Key,Val,Hash,Equal,Alloc>::insert(NodeType&& node) {

    auto it = find(node.first);
    if(it != end()){
        return std::make_pair(it, false);
    }
    size_t place = (hash(node.first))%ptrs.size();
    table.emplace(ptrs[place], std::move(const_cast<Key&>(node.first)), std::move(node.second));
    return std::make_pair(--ptrs[place], true);
}
template<typename Key, typename Val, typename Hash,typename Equal, typename Alloc>
std::pair<typename UnorderedMap<Key,Val,Hash,Equal,Alloc>::Iterator, bool>
UnorderedMap<Key,Val,Hash,Equal,Alloc>::insert(const UnorderedMap<Key,Val,Hash,Equal,Alloc>::NodeType& node) {
    auto it = find(node.first);
    if(it != end()){
        return std::make_pair(it, false);
    }
    size_t place = (hash(node.first))%ptrs.size();
    table.insert(ptrs[place], node);
    return std::make_pair(--ptrs[place], true);
}
template<typename Key, typename Val, typename Hash,typename Equal, typename Alloc>
template<typename InputIterator>
void UnorderedMap<Key,Val,Hash,Equal,Alloc>::insert(InputIterator first, InputIterator second) {
    for(auto it = first; it != second; ++it){
        insert(*it);
    }
}

template<typename Key, typename Val, typename Hash,typename Equal, typename Alloc>
template<typename... Args>
std::pair<typename UnorderedMap<Key,Val,Hash,Equal,Alloc>::Iterator, bool>
UnorderedMap<Key,Val,Hash,Equal,Alloc>::emplace(Args &&... args) {
    NodeType *new_node = AllocTraits::allocate(alloc_node, 1);
    AllocTraits::construct(alloc_node, new_node, std::forward<Args>(args)...);

    auto it = find(new_node->first);
    if(it != end()){
        return std::make_pair(it, false);
    }
    size_t place = (hash(new_node->first))%ptrs.size();
    table.emplace(ptrs[place], std::move(const_cast<Key&>(new_node->first)), std::move(new_node->second));
    return std::make_pair(--ptrs[place], true);
}

template<typename Key, typename Val, typename Hash,typename Equal, typename Alloc>
void UnorderedMap<Key,Val,Hash,Equal,Alloc>::erase(Iterator first, Iterator second) {
    while (first != second) {
        Iterator copy = first;
        ++first;
        this->erase(copy);
    }
}
template<typename Key, typename Val, typename Hash,typename Equal, typename Alloc>
void UnorderedMap<Key,Val,Hash,Equal,Alloc>::erase(Iterator it_){
    size_t place = (hash(it_->first))%ptrs.size();
    if(equal(ptrs[place]->first,it_->first)){
        ++ptrs[place];
    }

    table.erase(it_);
    auto it = ptrs[place];
    if(it == end() || (hash(it->first))%ptrs.size() != (hash(it_->first))%ptrs.size()){
        ptrs[place] = end();
    }
}
template<typename Key, typename Val, typename Hash,typename Equal, typename Alloc>
typename UnorderedMap<Key,Val,Hash,Equal,Alloc>::Iterator UnorderedMap<Key,Val,Hash,Equal,Alloc>::begin() {
    return Iterator(table.begin());
}
template<typename Key, typename Val, typename Hash,typename Equal, typename Alloc>
typename UnorderedMap<Key,Val,Hash,Equal,Alloc>::Iterator UnorderedMap<Key,Val,Hash,Equal,Alloc>::end() {
    return Iterator(table.end());
}
template<typename Key, typename Val, typename Hash,typename Equal, typename Alloc>
typename UnorderedMap<Key,Val,Hash,Equal,Alloc>::ConstIterator UnorderedMap<Key,Val,Hash,Equal,Alloc>::begin() const{
    return Iterator(table.begin());
}
template<typename Key, typename Val, typename Hash,typename Equal, typename Alloc>
typename UnorderedMap<Key,Val,Hash,Equal,Alloc>::ConstIterator UnorderedMap<Key,Val,Hash,Equal,Alloc>::end() const{
    return Iterator(table.end());
}
template<typename Key, typename Val, typename Hash,typename Equal, typename Alloc>
typename UnorderedMap<Key,Val,Hash,Equal,Alloc>::ConstIterator UnorderedMap<Key,Val,Hash,Equal,Alloc>::cbegin() const{
    return ConstIterator(table.begin());
}
template<typename Key, typename Val, typename Hash,typename Equal, typename Alloc>
typename UnorderedMap<Key,Val,Hash,Equal,Alloc>::ConstIterator UnorderedMap<Key,Val,Hash,Equal,Alloc>::cend() const{
    return ConstIterator(table.end());
}
template<typename Key, typename Val, typename Hash,typename Equal, typename Alloc>
typename UnorderedMap<Key,Val,Hash,Equal,Alloc>::Iterator UnorderedMap<Key,Val,Hash,Equal,Alloc>::find(const Key & key) {
    size_t place = (hash(key))%ptrs.size();
    auto it = ptrs[place];
    if(it == end()) return end();

    while(it!= end() && (hash(it->first))%ptrs.size() == (hash(key))%ptrs.size()){
        if(equal(it->first,key)){
            return it;
        }
        ++it;
    }
    return end();
}

template<typename Key, typename Val, typename Hash,typename Equal, typename Alloc>
UnorderedMap<Key,Val,Hash,Equal,Alloc>::UnorderedMap(const UnorderedMap& another): table(another.table),
                                                                                   ptrs(another.ptrs), alloc_node(another.alloc_node){}

template<typename Key, typename Val, typename Hash,typename Equal, typename Alloc>
UnorderedMap<Key,Val,Hash,Equal,Alloc>::UnorderedMap(const Alloc &alloc): alloc_node(alloc) {
    ptrs.assign(my_sz, end());
}

template<typename Key, typename Val, typename Hash,typename Equal, typename Alloc>
UnorderedMap<Key,Val,Hash,Equal,Alloc>::UnorderedMap(UnorderedMap&& another):
        table(std::move(another.table)), ptrs(std::move(another.ptrs)), alloc_node(std::move(another.alloc_node)){
    for(auto& it : ptrs){
        if(it == another.table.end()){
            it = table.end();
        }
    }
}

template<typename Key, typename Val, typename Hash,typename Equal, typename Alloc>
UnorderedMap<Key,Val,Hash,Equal,Alloc>& UnorderedMap<Key,Val,Hash,Equal,Alloc>::operator=(UnorderedMap&& another){
    if(&another == this){
        return *this;
    }
    table = std::move(another.table);
    ptrs = std::move(another.ptrs);
    alloc_node = std::move(another.alloc_node);
    for(auto& it : ptrs){
        if(it == another.table.end()){
            it = table.end();
        }
    }
    return *this;
}

template<typename Key, typename Val, typename Hash,typename Equal, typename Alloc>
UnorderedMap<Key,Val,Hash,Equal,Alloc>& UnorderedMap<Key,Val,Hash,Equal,Alloc>::operator=(const UnorderedMap& another){
    if(&another == this){
        return *this;
    }
    table.assign(my_sz, end());
    ptrs.clear();
    for(auto it = another.table.begin(); it != another.table.end(); ++it){
        emplace(it->first, it->second);
    }
}

template<typename Key, typename Val, typename Hash,typename Equal, typename Alloc>
Val& UnorderedMap<Key,Val,Hash,Equal,Alloc>::operator[](const Key& key){
    return emplace(key, Val()).first->second;
}

template<typename Key, typename Val, typename Hash,typename Equal, typename Alloc>
Val& UnorderedMap<Key,Val,Hash,Equal,Alloc>::at(const Key& key){
    auto it = find(key);
    if(it!= end()) return it->second;
    throw std::out_of_range("out of range");
}

template<typename Key, typename Val, typename Hash,typename Equal, typename Alloc>
void UnorderedMap<Key,Val,Hash,Equal,Alloc>::rehash(size_t new_size) {
    std::vector<typename List<UnorderedMap<Key,Val,Hash,Equal,Alloc>::NodeType, Alloc>::iterator> new_ptrs(new_size, end());
    List<UnorderedMap<Key,Val,Hash,Equal,Alloc>::NodeType, Alloc> new_table;
    std::swap(ptrs, new_ptrs);
    std::swap(table, new_table);
    for(auto it = new_table.begin(); it != new_table.end(); ++it){
        emplace(std::move(const_cast<Key&>(it->first)), std::move(it->second));
    }
}

template<typename Key, typename Val, typename Hash,typename Equal, typename Alloc>
void UnorderedMap<Key,Val,Hash,Equal,Alloc>::reserve(size_t sz) {
    if(sz > max_sz) return;
    if(sz > ptrs.size()) rehash(sz);
}
