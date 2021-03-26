#include <iostream>
#include <cstring>

using std::ostream;
using std::istream;

class String
{
private:
    size_t size;
    size_t capacity;
    char* str = nullptr;
public:

    String(const char* s);
    String();
    String(size_t n, char c);
    String(const String& s);
    ~String();
    String& operator=(String s);
    const char& operator[](const size_t index) const;
    char& operator[](const size_t index);
    String& operator+=(const char c);
    String& operator+=(const String& s);

    String substr(const size_t start, const size_t count) const;
    size_t find(const String& substring) const;
    size_t rfind(const String& substring) const;
    void swap(String& s);
    bool empty() const;
    const size_t& length() const;
    char& front();
    const char& front() const;
    char& back();
    const char& back() const;
    void push_back(char);
    void pop_back();
    void clear();
    bool operator==(const String& s) const;
};

void String::push_back(char c) {
    if (capacity <= size + 2) {
        char* copy = new char[capacity * 2 + 1];
        memcpy(copy, str, size);
        delete[] str;
        capacity = capacity * 2 + 1;
        str = copy;
    }
    str[size] = c;
    ++size;
}
void String::pop_back() {
    if (capacity > 4) {
        if (capacity > size * 4) {
            char* copy = new char[capacity / 2];
            memcpy(copy, str, size);
            delete[] str;
            capacity /= 2;
            str = copy;
        }
    }
    --size;
}

bool String::operator==(const String& s) const {
    if (size != s.size)
        return false;
    for (size_t i = 0; i < size; i++) {
        if (str[i] != s[i])
            return false;
    }
    return true;
}

const size_t& String::length() const {
    return size;
}

const char& String::operator[](const size_t index) const {
    return str[index];
}

char& String::operator[](const size_t index) {
    return str[index];
}

String& String::operator+=(const char c) {
    push_back(c);
    return *this;
}

String& String::operator+=(const String& s) {
    capacity += s.capacity;
    size += s.size;
    for (size_t i = 0; i < s.size; ++i)
        str[i+size] = s[i];
    return *this;
}

String operator+(const String& s, const String& c) {
    String copy = s;
    copy += c;
    return copy;
}

String operator+(const String& s, const char& c) {
    String copy = s;
    copy += c;
    return copy;
}

String operator+(const char& s, const  String& c) {
    String copy(1, s);
    copy += c;
    return copy;
}

char& String::front() {
    return str[0];
}
const char& String::front() const {
    return str[0];
}
char& String::back() {
    return str[size - 1];
}
const char& String::back() const {
    return str[size - 1];
}
String& String::operator=(String s) {
    swap(s);
    return *this;
}
void String::swap(String& s) {
    std::swap(size, s.size);
    std::swap(str, s.str);
    std::swap(capacity, s.capacity);
}

String::String(const char* s) : size(strlen(s)), capacity(size + 1) {
    str = new char[capacity];
    memcpy(str, s, size);
}

String::String(size_t n, char c) : size(n), capacity(n + 1) {
    str = new char[capacity];
    memset(str, c, size);
}

String::String() : size(0), capacity (4) {
    str = new char[capacity];
}

String::String(const String& s) : size(s.size), capacity(s.capacity){
    str = new char[capacity];
    memcpy(str, s.str, size);
}

String::~String() {
    delete[] str;
}

size_t String::find(const String& substring) const {
    size_t i = 0;
    while (true) {
        bool f = 1;
        for (size_t j = i; j < i + substring.size; ++j) {
            if (str[j] != substring[j - i]) {
                f = 0;
                break;
            }
        }
        if (f == 1) return i;
        ++i;
        if (i >= size - substring.size) return length();
    }
}
size_t String::rfind(const String& substring) const {
    size_t i = size - 1;
    
    while (true) {
        if (i < substring.size - 1) return length();
        bool f = 1;
        size_t j = i;
        while (true) {
            if (j <= i - substring.size) break;
            if (str[j] != substring[j - i + substring.size - 1]) {
                f = 0;
                break;
            }
            if (j == 0) break;
            --j;
        }
        if (f == 1) return i - substring.size + 1;
        --i;
    }
}

String String::substr(const size_t start, const size_t count) const {
    String copy(count, '\0');
    memcpy(copy.str, str + start, count);
    return copy;
}

bool String::empty() const {
    if (size == 0)
        return true;
    else
        return false;
}

void String::clear() {
    capacity = 4;
    size = 0;
    delete[] str;
    str = new char[capacity];
}

std::ostream& operator<<(std::ostream& out, const String& s) {
    for (size_t i = 0; i < s.length(); ++i)
        out << s[i];
    return out;
}

std::istream& operator>>(std::istream& in, String& s) {
    in >> std::noskipws;
    s.clear();
    char c;
    while (in >> c && c != '\n' && c != ' ') {
        s.push_back(c);
    }
    return in;
}
