#include<iostream>
#include<vector>
#include<algorithm>

template<typename T>
class Bucket {
private:
    size_t sz;
    size_t capacity = 100;
    T *arr;
public:
    size_t start = 0;

    Bucket(size_t n);
    Bucket(size_t n, const T &value);
    Bucket(Bucket<T>& another);

    T &operator[](size_t number);
    const T &operator[](size_t number) const;
    Bucket<T>& operator=(const Bucket<T>& another);
    T &at(size_t number);
    const T &at(size_t number) const;

    void push_back(const T &value);
    void pop_back();
    void push_front(const T &value);
    void pop_front();

    size_t size();
};

template<typename T>
Bucket<T>::Bucket(Bucket<T> &another) {
    sz = another.sz;
    capacity = another.capacity;
    start = another.start;
    arr =  reinterpret_cast<T *>(new int8_t[capacity * sizeof(T)]);
    for (size_t i = 0; i < sz; ++i) {
        new(arr + i) T(*(another.arr + i));
    }
}

template<typename T>
Bucket<T>::Bucket(size_t n, const T &value) : sz(n) {
    arr = reinterpret_cast<T *>(new int8_t[capacity * sizeof(T)]);
    for (size_t i = 0; i < sz; ++i) {
        new(arr + i) T(value);
    }
}

template<typename T>
Bucket<T>::Bucket(size_t n) : sz(n) {
    arr = reinterpret_cast<T *>(new int8_t[capacity * sizeof(T)]);
    if(n == 0) start = capacity;
}

template<typename T>
Bucket<T>& Bucket<T>::operator=(const Bucket<T> &another) {
    sz = another.sz;
    capacity = another.capacity;
    start = another.start;
    arr =  reinterpret_cast<T *>(new int8_t[capacity * sizeof(T)]);
    for (size_t i = 0; i < sz; ++i) {
        new(arr + i) T(*(another.arr + i));
    }
}

template<typename T>
T &Bucket<T>::operator[](size_t number) {
    return arr[start + number];
}

template<typename T>
T &Bucket<T>::at(size_t number) {
    if (start + number >= sz || sz == 0) throw std::out_of_range("out of range");
    return arr[start + number];
}

template<typename T>
const T &Bucket<T>::operator[](size_t number) const {
    return arr[start + number];
}


template<typename T>
const T &Bucket<T>::at(size_t number) const {
    if (start + number >= sz || sz == 0) throw std::out_of_range("out of range");
    return arr[start + number];
}

template<typename T>
void Bucket<T>::push_back(const T &value) {
    new(arr + sz) T(value);
    ++sz;
}

template<typename T>
void Bucket<T>::push_front(const T &value) {
    ++sz;
    --start;
    new(arr + start) T(value);
}

template<typename T>
void Bucket<T>::pop_back() {
    --sz;
    (arr + sz)->~T();
}

template<typename T>
void Bucket<T>::pop_front() {
    --sz;
    (arr + start)->~T();
    ++start;
}

/// Bucket

template<bool B, typename U, typename V>
struct conditional {
    using type = V;
};
template<typename U, typename V>
struct conditional<true, U, V> {
    using type = U;
};
template<bool B, typename U, typename V>
using conditional_t = typename conditional<B, U, V>::type;

template<typename T>
class Deque {
    size_t sz = 0;
    size_t head_size = 0;
    size_t tail_size = 0;
    size_t bucket_sz = 100;
    size_t first_cell = 0;
    size_t last_cell = 0;
    size_t start = 0;
    size_t added_to_front = 0;
    std::vector<Bucket<T>*> main_arr;


    template <bool IsConst>
    struct common_iterator{
        using Conditional_Ptr = std::conditional_t<IsConst,const T*, T*>;
        using Conditional_Reference = std::conditional_t<IsConst,const T&, T&>;
    private:
        const Deque* deque_pointer;
        Conditional_Ptr ptr;
        size_t added_to_front;

        void it_from_num(){
            if (number < deque_pointer->head_size){
                ptr = &(*(deque_pointer->main_arr[deque_pointer->first_cell]))[number];
            }
            else{
                size_t x = (number - deque_pointer->head_size) / deque_pointer->bucket_sz + 1;
                size_t y = (number - deque_pointer->head_size) % deque_pointer->bucket_sz;
                ptr = &(*(deque_pointer->main_arr[x + deque_pointer->first_cell]))[y];
            }
        }
        void Normalize(){
            number = real_number();
            added_to_front = deque_pointer->added_to_front;
        }
    public:
        size_t number = 0;
        common_iterator(size_t number, const Deque<T>*  deque_pointer1) : deque_pointer(deque_pointer1), number(number){
            added_to_front = deque_pointer1->added_to_front;
            ptr = &(*(deque_pointer->main_arr[deque_pointer->first_cell]))[0];
            ptr += number*sizeof(T);
        }

        Conditional_Reference& operator*();
        Conditional_Ptr operator->();

        ///ariphmetical operators
        common_iterator& operator+=(size_t n){
            Normalize();
            number += n;
            if(number < 0){
                size_t x = number;
                number = 0;
                it_from_num();
                number = x;
                ptr += number*sizeof(T);
            }
            else if(number >= deque_pointer->sz){
                size_t x = number - deque_pointer->sz + 1;
                number -= x;
                it_from_num();
                ptr += x*sizeof(T);
                number += x;
            }
            else{
                it_from_num();
            }
            return *this;
        }

        int real_number() const{
            if(number - added_to_front + deque_pointer->added_to_front < 0) return -10000000;
            if(number - added_to_front + deque_pointer->added_to_front > deque_pointer->sz+1) return - 10000000;
            return number - added_to_front + deque_pointer->added_to_front;
        }
        common_iterator& operator++(){
            return *this+=1;
        }
        common_iterator operator++(int) {
            auto copy = *this;
            ++copy;
            return copy;
        }common_iterator& operator--(){
            return *this+=(-1);
        }
        common_iterator operator--(int) {
            auto copy = *this;
            --copy;
            return copy;
        }
        common_iterator operator+(size_t n) const{
            auto copy = *this;
            copy += n;
            return copy;
        }
        common_iterator operator-(size_t n) const{
            auto copy = *this;
            copy -= n;
            return copy;
        }
        common_iterator& operator-=(size_t n){
            return *this+=(-n);
        }
        /// comparison operators

        bool operator>(const common_iterator& other){
            return real_number() > other.real_number();
        }
        bool operator<(const common_iterator& other){
            return real_number() < other.real_number();
        }
        bool operator>=(const common_iterator& other){
            int x = real_number();
            int y = other.real_number();
            return x >= y;
        }
        bool operator<=(const common_iterator& other){
            return real_number() <= other.real_number();
        }
        bool operator==(const common_iterator& other){
            size_t x = real_number();
            size_t y = other.real_number();
            return x == y;
        }
        bool operator!=(const common_iterator& other){
            return real_number() != other.real_number();
        }
        size_t operator-(const common_iterator& other){
            return real_number() - other.real_number();
        }
        operator common_iterator<true>(){
            return common_iterator<true>(number, deque_pointer);
        };

    };

public:
    Deque();
    Deque(size_t amount);
    Deque(size_t amount, const T &value);
    Deque(const Deque<T> &another);

    Deque<T> &operator=(const Deque<T> &another);
    T &operator[](size_t number);
    const T &operator[](size_t number) const;
    T &at(size_t number);

    void push_back(const T &value);
    void push_front(const T &value);
    void pop_back();
    void pop_front();
    void resize();

    size_t size() const;

    using iterator = common_iterator<false>;
    using const_iterator = common_iterator<true>;

    using reverse_iterator = std::reverse_iterator<common_iterator<false>>;

    void insert(iterator it, const T& value){
        iterator last = end()-1;
        push_back(*last);
        for(iterator iter = last; iter != it; --iter){
            *iter = *(iter-1);
        }
        *it = value;
    }
    void erase(iterator it){
        for(iterator iter = it; iter != end()-1; ++iter){
            *iter = *(iter+1);
        }
        pop_back();
    }

    iterator begin() {
        return iterator (0, this);
    }
    const_iterator begin() const {
        return const_iterator (0, this);
    }
    const_iterator cbegin() const{
        return const_iterator (0, this);
    }
    iterator end() {
        return iterator (sz, this);
    }
    const_iterator end() const {
        return const_iterator (sz, this);
    }
    const_iterator cend() const{
        return const_iterator (sz, this);
    }
};

template<typename T>
template < bool IsConst>
typename Deque<T>::template common_iterator<IsConst>::Conditional_Reference& Deque<T>::common_iterator<IsConst>::operator*(){
    return *ptr;
}
template<typename T>
template < bool IsConst>
typename Deque<T>::template common_iterator<IsConst>::Conditional_Ptr Deque<T>::common_iterator<IsConst>::operator->() {
    return ptr;
}

/// common_iterator

template<typename T>
Deque<T>::Deque() {
    sz = 0;
    start = 0;
    main_arr.resize(1);
    head_size = 0;
    tail_size = 0;
    main_arr[0] = new Bucket<T>(0);
}


template<typename T>
Deque<T>::Deque(size_t amount) {
    sz = amount;
    size_t new_sz = 2 * ((sz + bucket_sz - 1) / bucket_sz);
    main_arr.resize(new_sz);
    head_size = std::min(amount, static_cast<size_t>(bucket_sz));
    tail_size = amount % bucket_sz;

    first_cell = new_sz / 4;
    last_cell = new_sz / 4 + (amount) / bucket_sz;
    for (size_t i = first_cell; i < last_cell; ++i) {
        main_arr[i] = new Bucket<T>(bucket_sz);
    }

    if (tail_size != 0) {
        main_arr[last_cell] = new Bucket<T>(tail_size);
    }
}


template<typename T>
Deque<T>::Deque(size_t amount, const T &value) {
    sz = amount;
    size_t new_sz = 2 * ((sz + bucket_sz - 1) / bucket_sz);
    main_arr.resize(new_sz);
    head_size = std::min(amount, static_cast<size_t>(bucket_sz));
    tail_size = amount % bucket_sz;
    first_cell = new_sz / 4;
    last_cell = new_sz / 4 + (amount) / bucket_sz;
    for (size_t i = first_cell; i < last_cell; ++i) {
        main_arr[i] = new Bucket<T>(bucket_sz, value);
    }
    if (tail_size != 0) {
        main_arr[last_cell] = new Bucket<T>(tail_size);
        for (size_t i = 0; i < tail_size; ++i) {
            (*main_arr[last_cell])[i] = value;
        }
    }
}

template<typename T>
Deque<T>::Deque(const Deque<T> &another) {
    bucket_sz = another.bucket_sz;
    start = 0;
    auto it = another.begin();
    while (it != another.end()) {
        push_back(*it);
        ++it;
    }
}
///constructors

template<typename T>
Deque<T> &Deque<T>::operator=(const Deque<T> &another) {
    for (size_t i = 0; i < main_arr.size(); ++i) {
        delete[] main_arr[i];
    }
    main_arr.resize(0);
    head_size = 0;
    tail_size = 0;
    first_cell = 0;
    last_cell = 0;
    sz = 0;
    bucket_sz = another.bucket_sz;
    start = 0;
    auto it = another.begin();
    while (it != another.end()) {
        push_back(*it);
        ++it;
    }
    return *this;
}

template<typename T>
T &Deque<T>::operator[](size_t number) {
    if (number < head_size) return (*main_arr[first_cell])[number];
    size_t x = (number - head_size) / bucket_sz + 1;
    size_t y = (number - head_size) % bucket_sz;
    return (*main_arr[x + first_cell])[y];
}

template<typename T>
const T &Deque<T>::operator[](size_t number) const{
    if (number < head_size) return (*main_arr[first_cell])[number];
    size_t x = (number - head_size) / bucket_sz + 1;
    size_t y = (number - head_size) % bucket_sz;
    return (*main_arr[x + first_cell])[y];
}

template<typename T>
T &Deque<T>::at(size_t number) {
    if (number > sz - 1 || number < 0 || sz == 0) throw std::out_of_range("Pomogite");
    return (*this)[number];
}

///operators

template<typename T>
void Deque<T>::push_back(const T &value) {
    if (first_cell == last_cell && tail_size + start == bucket_sz) {
        tail_size = 0;
        ++last_cell;
    }
    if(first_cell != last_cell && tail_size == bucket_sz){
        tail_size = 0;
        ++last_cell;
    }
    resize();
    ++sz;
    if (tail_size != 0) {
        (*main_arr[last_cell]).push_back(value);
    } else {
        main_arr[last_cell] = new Bucket<T>(1, value);
    }
    if (first_cell == last_cell) head_size++;
    tail_size++;
}

template<typename T>
void Deque<T>::push_front(const T &value) {
    ++added_to_front;
    resize();
    ++sz;
    if (start!=0) {
        (*main_arr[first_cell]).push_front(value);
        head_size++;
        start--;
    } else {
        first_cell--;
        main_arr[first_cell] = new Bucket<T>(0);
        (*main_arr[first_cell]).push_front(value);
        head_size = 1;
        start = bucket_sz - 1;
    }
}

template<typename T>
void Deque<T>::pop_back() {

    --sz;
    if (!main_arr[last_cell] || (*main_arr[last_cell]).size() == 0) {
        tail_size = bucket_sz;
        delete main_arr[last_cell];
        main_arr.pop_back();
        --last_cell;
    }
    if (first_cell == last_cell) head_size--;
    --tail_size;
    (*main_arr[last_cell]).pop_back();
    //resize();
    if(head_size == 0){
        head_size = std::min(bucket_sz,sz);
        start = 0;
        ++first_cell;
    }
}

template<typename T>
void Deque<T>::pop_front() {
    --added_to_front;
    --sz;
    if(start == bucket_sz){
        delete main_arr[first_cell];
        ++first_cell;
        head_size = std::min(bucket_sz,sz+1);
        start = 0;
    }
    (*main_arr[first_cell]).pop_front();
    ++start;
    --head_size;
    if(head_size == 0){
        head_size = std::min(bucket_sz,sz);
        start = 0;
        ++first_cell;
    }
    resize();
}

template<typename T>
void Deque<T>::resize() {
    size_t n = main_arr.size();
    if(last_cell == n || first_cell == 0){size_t x = last_cell - first_cell;
        std::vector<Bucket<T>*> new_main_arr(2*n +5);
        size_t old = first_cell;
        first_cell = n / 2 + 1;
        last_cell = first_cell + x;
        size_t tmp = first_cell;
        for(size_t i = old; i < n; ++i){
            if(main_arr[i]){
                new_main_arr[tmp] = main_arr[i];
                ++tmp;
            }
        }
        main_arr = new_main_arr;
    }
}

template<typename T>
size_t Bucket<T>::size() {
    return sz;
}

template<typename T>
size_t Deque<T>::size() const {
    return sz;
}

///operations
