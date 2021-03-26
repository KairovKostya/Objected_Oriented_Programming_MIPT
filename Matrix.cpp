#include<iostream>
#include<vector>
#include<cmath>
#include<cassert>
#include<algorithm>
#include<string>
#include <fstream> 

using namespace std;

class BigInteger {
private:
    static const long long BASE = 1000000000;
    std::vector<long long> digits_;
    bool is_negative_;
    void Increase(int x);
    void Decrease();
    size_t size() const;
    void ToCorrectForm();

    friend class Rational;
public:
    //input-output
    friend std::ostream& operator<<(std::ostream& out, const BigInteger& value);
    friend std::istream& operator>>(std::istream& in, BigInteger& value);

    //constructors and other
    BigInteger(int);
    BigInteger();
    ~BigInteger() = default;
    BigInteger(const BigInteger& x);
    explicit operator bool() const;
    explicit operator double() const;
    BigInteger& operator=(const BigInteger& copy);


    //comparison
    friend bool operator==(const BigInteger& left, const BigInteger& right);
    friend bool operator!=(const BigInteger& left, const BigInteger& right);
    friend bool operator>(const BigInteger& left, const BigInteger& right);
    friend bool operator>=(const BigInteger& left, const BigInteger& right);
    friend bool operator<(const BigInteger& left, const BigInteger& right);
    friend bool operator<=(const BigInteger& left, const BigInteger& right);

    //ariphmetical operators
    friend BigInteger operator-(BigInteger& value);
    BigInteger& operator+=(const BigInteger&);
    BigInteger& operator*=(const BigInteger&);
    BigInteger& operator/=(const BigInteger&);
    BigInteger& operator-=(const BigInteger&);
    BigInteger& operator%=(const BigInteger&);

    //increment, decrement
    BigInteger& operator++();
    BigInteger operator++(int);
    BigInteger& operator--();
    BigInteger operator--(int);
    //other

    std::string toString() const;
    void ChangeSign();
    void Clear();
};


class Rational {
private:
    BigInteger numerator_;
    BigInteger denominator_;
    BigInteger Exponentiation(BigInteger x, size_t deg) const;
    void Normalize();
public:
    //constructors
    Rational();
    Rational(int x);
    Rational(const BigInteger& x);
    Rational(const BigInteger& numerator, const BigInteger& denominator);
    ~Rational() = default;
    //ariphmetical operators
    Rational& operator+=(const Rational& value);
    friend Rational operator+(const Rational& left, const Rational& right);
    Rational& operator-=(const Rational& value);
    friend Rational operator-(const Rational& left, const Rational& right);
    friend Rational operator-(const Rational& value);
    Rational& operator*=(const Rational& value);
    friend Rational operator*(const Rational& left, const Rational& right);
    Rational& operator/=(const Rational& value);
    friend Rational operator/(const Rational& left, const Rational& right);
    //comparison
    friend bool operator<(const Rational& left, const Rational& right);
    friend bool operator>(const Rational& left, const Rational& right);
    friend bool operator>=(const Rational& left, const Rational& right);
    friend bool operator<=(const Rational& left, const Rational& right);
    friend bool operator==(const Rational& left, const Rational& right);
    friend bool operator!=(const Rational& left, const Rational& right);
    //other
    std::string toString() const;
    std::string asDecimal(size_t precision = 0) const;

    explicit operator double() const;
    friend std::istream& operator>>(std::istream& in, Rational& value);
    friend std::ostream& operator<<(std::ostream& out, const Rational& value);
};
//////////////////////////////////////////////////////////////BIG INTEGER/////////////////////////////////////////////////////////////////////////

//INPUT-OUTPUT

std::ostream& operator<<(std::ostream& out, const BigInteger& value) {
    if (value == 0) {
        out << '0';
        return out;
    }
    if (value.is_negative_) {
        out << '-';
    }
    size_t i = value.size() - 1;

    while (true) {
        if (i < value.size() - 1) {
            size_t  t = BigInteger::BASE / 10;
            while (t > static_cast<size_t>(value.digits_[i])) {
                out << '0';
                t /= 10;
            }
        }
        if (value.digits_[i] != 0)
            out << value.digits_[i];

        if (i == 0) break;
        --i;
    }

    return out;
}
std::istream& operator>>(std::istream& in, BigInteger& value) {
    std::string str;
    in >> str;
    size_t length = 9;
    value.Clear();
    size_t  pos = 0;
    if (str[0] == '-') {
        value.is_negative_ = true;
        str.erase(0, 1);
    }
    else value.is_negative_ = false;
    size_t i = str.size() - 1;
    while (true) {
        value.digits_.push_back(0);
        if (i + 1 > length) pos = i + 1 - length;
        else pos = 0;
        while (pos <= i) {
            value.digits_[value.size() - 1] = value.digits_[value.size() - 1] * 10 + str[pos] - 48;
            ++pos;
        }
        if (i < length) break;
        i -= length;
    }
    return in;
}
//CONSTRUCTORS and explicits
BigInteger::BigInteger() : is_negative_(false) {}

BigInteger::BigInteger(int value) {
    if (value < 0) {
        value *= -1;
        is_negative_ = true;
    }
    else is_negative_ = false;
    while (value > 0) {
        digits_.push_back(value % BASE);
        value /= BASE;
    }
    if (digits_.empty()) {
        digits_.push_back(0);
    }
    ToCorrectForm();
}
BigInteger::BigInteger(const BigInteger& x) : digits_(x.digits_), is_negative_(x.is_negative_) {}

BigInteger::operator bool() const {
    return *this != BigInteger(0);
}

BigInteger::operator double() const {
    double temp = 1;
    double result = 0;
    for (size_t i = 0; i < digits_.size(); ++i) {
        result += temp * digits_[i];
        temp *= BASE;
    }
    return result;
}
BigInteger& BigInteger::operator=(const BigInteger& number) { //new++
    digits_ = number.digits_;
    is_negative_ = number.is_negative_;
    return *this;
}
//COMPARISON


bool operator>(const BigInteger& left, const BigInteger& right) {
    if (left.digits_[0] == 0 && right.digits_[0] == 0 && left.size() <= 1 && right.size() <= 1) return false;
    if (left.is_negative_) {
        if (!right.is_negative_) return false;
    }
    else if (right.is_negative_) return true;
    if (left.size() > right.size()) return !left.is_negative_;
    if (left.size() < right.size()) return left.is_negative_;
    size_t i = left.size() - 1;
    while (true) {
        if (right.digits_[i] < left.digits_[i]) return !left.is_negative_;
        if (right.digits_[i] > left.digits_[i]) return left.is_negative_;
        if (i == 0) break;
        --i;
    }
    return false;
}
bool operator<(const BigInteger& right, const BigInteger& left) {
    if (left.digits_[0] == 0 && right.digits_[0] == 0 && left.size() <= 1 && right.size() <= 1) return false;
    if (left.is_negative_) {
        if (!right.is_negative_) return false;
    }
    else if (right.is_negative_) return true;
    if (left.size() > right.size()) return !left.is_negative_;
    if (left.size() < right.size()) return left.is_negative_;
    size_t i = left.size() - 1;
    while (true) {
        if (right.digits_[i] < left.digits_[i]) return !left.is_negative_;
        if (right.digits_[i] > left.digits_[i]) return left.is_negative_;
        if (i == 0) break;
        --i;
    }
    return false;
}

bool operator==(const BigInteger& left, const BigInteger& right) {
    return !(left > right) && !(left < right);
}

bool operator!=(const BigInteger& left, const BigInteger& right) {
    return !(left == right);
}

bool operator>=(const BigInteger& left, const BigInteger& right) {
    return !(left < right);
}

bool operator<=(const BigInteger& left, const BigInteger& right) {
    return !(left > right);
}

//ARIPMETICAL OPERATORS

BigInteger operator-(BigInteger& value) {
    BigInteger copy = value;
    copy.is_negative_ = !copy.is_negative_;
    return copy;
}


BigInteger& BigInteger::operator+=(const BigInteger& value) {
    if (digits_.size() < value.digits_.size()) {
        digits_.resize(value.digits_.size());
    }
    for (size_t i = 0; i < value.digits_.size(); ++i) {
        if (value.is_negative_ == is_negative_) {
            digits_[i] += value.digits_[i];
        }
        else {
            digits_[i] -= value.digits_[i];
        }
    }
    ToCorrectForm();
    return *this;
}

BigInteger& BigInteger::operator-=(const BigInteger& value) {
    if (digits_.size() < value.digits_.size()) {
        digits_.resize(value.digits_.size());
    }
    for (size_t i = 0; i < value.digits_.size(); ++i) {
        if (value.is_negative_ == is_negative_) {
            digits_[i] -= value.digits_[i];
        }
        else {
            digits_[i] += value.digits_[i];
        }
    }
    ToCorrectForm();
    return *this;
}

BigInteger& BigInteger::operator*=(const BigInteger& value) {
    size_t max = digits_.size();
    if (digits_.size() < value.digits_.size() + digits_.size()) {
        digits_.resize(value.digits_.size() + digits_.size());
    }
    if (is_negative_ != value.is_negative_) is_negative_ = true;
    else is_negative_ = false;

    for (size_t i = max - 1; true; --i) {
        for (size_t j = value.size() - 1; true; --j) {
            if (j != 0) digits_[i + j] += digits_[i] * value.digits_[j];
            else digits_[i + j] = digits_[i] * value.digits_[j];
            if (j == 0) break;
        }
        if (i == 0) break;
    }
    ToCorrectForm();
    return *this;
}

BigInteger operator+(const BigInteger& left, const BigInteger& right) {
    BigInteger copy = left;
    copy += right;
    return copy;
}
BigInteger operator-(const BigInteger& left, const BigInteger& right) {
    BigInteger copy = left;
    copy -= right;
    return copy;
}
BigInteger operator*(const BigInteger& left, const BigInteger& right) {
    BigInteger copy = left;
    copy *= right;
    return copy;
}
BigInteger operator/(const BigInteger& left, const BigInteger& right) {
    BigInteger copy = left;
    copy /= right;
    return copy;
}
BigInteger operator%(const BigInteger& left, const BigInteger& right) {
    BigInteger copy = left;
    copy %= right;
    return copy;
}

BigInteger& BigInteger::operator/=(const BigInteger& value) {
    if (size() < value.size()) {
        *this = 0;
        return *this;
    }
    bool neg = !(is_negative_ == value.is_negative_);
    BigInteger result;
    is_negative_ = false;
    BigInteger copy = value;
    copy.is_negative_ = false;
    result.digits_.resize(size() - copy.size() + 1);
    BigInteger deg_of_base = 1;
    size_t x = result.digits_.size() - 1;
    copy.Increase(result.digits_.size() - 1);
    size_t i = result.digits_.size() - 1;
    while (true) {
        if (*this == 0) break;
        size_t  right = BASE;
        size_t  left = 0;
        while (right - left > 1) {
            long long mid = (left + right) / 2;
            if (!(*this < copy * mid)) {
                left = mid;
            }
            else {
                right = mid;
            }
        }
        result.digits_[i] = left;
        *this -= left * copy;

        if (x > 0) {
            copy.Decrease();
            --x;
        }
        else {
            copy = 0;
        }
        if (i == 0) break;
        --i;
    }

    *this = result;
    is_negative_ = neg;
    ToCorrectForm();
    return *this;
}
BigInteger& BigInteger::operator%=(const BigInteger& value) {
    bool neg = is_negative_;
    BigInteger divisor_copy = value;
    divisor_copy.is_negative_ = false;
    is_negative_ = false;

    if (digits_.size() < value.digits_.size()) {
        is_negative_ = neg;
        return *this;
    }
    divisor_copy.Increase(digits_.size() - value.digits_.size());
    for (int i = size() - value.size(); i >= 0; --i) {
        long long left = 0;
        long long right = BASE;
        while (right - left > 1) {
            long long mid = (left + right) / 2;
            if (!(*this < divisor_copy * mid)) left = mid;
            else right = mid;
        }
        *this -= left * divisor_copy;
        divisor_copy.Decrease();
    }
    is_negative_ = neg;
    ToCorrectForm();
    return *this;
}

//INCREMENT-DECREMENT

BigInteger& BigInteger::operator++() {
    if (!is_negative_) {
        ++digits_[0];
        if (digits_[0] >= BASE) ToCorrectForm();
        return *this;
    }
    --digits_[0];
    if (digits_[0] < 0) ToCorrectForm();
    return *this;
}

BigInteger BigInteger::operator++(int) {
    BigInteger copy = *this;
    ++* this;
    return copy;
}

BigInteger& BigInteger::operator--() {
    if (is_negative_) {
        ++digits_[0];
        if (digits_[0] >= BASE) ToCorrectForm();
        return *this;
    }
    --digits_[0];
    if (digits_[0] < 0) ToCorrectForm();
    return *this;
}


BigInteger BigInteger::operator--(int) {
    BigInteger copy = *this;
    --* this;
    return copy;
}

//OTHER

size_t BigInteger::size() const {
    return digits_.size();
}

void BigInteger::ToCorrectForm() {
    size_t max = size() - 1;
    while (max > 0 && digits_[max] == 0) {
        digits_.pop_back();
        --max;
    }
    if (digits_[size() - 1] < 0) {
        is_negative_ = !is_negative_;
        for (size_t i = 0; i < size(); ++i)
            digits_[i] *= (-1);
    }
    max = size();
    for (size_t i = 0; i < max; ++i) {
        if (digits_[i] >= BASE) {
            if (size() < i + 2) {
                digits_.push_back(digits_[i] / BASE);
            }
            else digits_[i + 1] += (digits_[i] / BASE);
            digits_[i] %= BASE;
        }
        if (digits_[i] < 0) {
            --digits_[i + 1];
            digits_[i] += BASE;
        }
    }
    max = size() - 1;
    while (max > 0 && digits_[max] == 0) {
        digits_.pop_back();
        --max;
    }
}


void BigInteger::Clear() {
    is_negative_ = true;
    digits_.clear();
}

std::string BigInteger::toString() const {
    std::string result;
    if (*this == 0) {
        result = '0';
        return result;
    }
    if (is_negative_) {
        result += '-';
    }
    size_t i = size() - 1;
    while (true) {
        if (i < size() - 1) {
            size_t  t = BigInteger::BASE / 10;
            while (t > static_cast<size_t>(digits_[i])) {
                result += '0';
                t /= 10;
            }
        }
        if (digits_[i] != 0)
            result += std::to_string(digits_[i]);
        if (i == 0) break;
        --i;
    }

    return result;
}
void BigInteger::ChangeSign() {
    is_negative_ = !is_negative_;
    if (digits_.size() == 1 && digits_[0] == 0) is_negative_ = true;
}
void BigInteger::Increase(int x) {
    digits_.resize(digits_.size() + x);
    for (int i = digits_.size() - 1; i >= 0; --i) {
        if (i >= x) {
            digits_[i] = digits_[i - x];
        }
        else {
            digits_[i] = 0;
        }
    }
    ToCorrectForm();
}
void BigInteger::Decrease() {
    for (size_t i = 0; i < digits_.size() - 1; ++i) {
        digits_[i] = digits_[i + 1];
    }
    digits_[digits_.size() - 1] = 0;
    ToCorrectForm();
}
/////////////////////////////RATIONAL//////////////////////////////////////////
Rational::Rational() : numerator_(0), denominator_(1) {}

Rational::Rational(int x) : numerator_(x), denominator_(1) {}

Rational::Rational(const BigInteger& x) : numerator_(x), denominator_(1) {}

Rational::Rational(const BigInteger& numerator, const BigInteger& denominator) : numerator_(numerator), denominator_(denominator) {}

void Rational::Normalize() {
    if (denominator_.is_negative_) {
        denominator_.is_negative_ = false;
        numerator_.is_negative_ = !numerator_.is_negative_;
    }
    BigInteger num = numerator_;
    BigInteger den = denominator_;
    BigInteger gcd = 1;
    num.is_negative_ = false;
    den.is_negative_ = false;
    while (den != 0) {
        gcd = num % den;
        num = den;
        den = gcd;
    }
    numerator_ /= num;
    denominator_ /= num;
}

//ARIPHMETICAL OPERATIONS
Rational operator-(const Rational& value) {
    Rational copy = value;
    copy.numerator_.ChangeSign();
    return copy;
}

Rational& Rational::operator+=(const Rational& value) {
    numerator_ *= value.denominator_;
    numerator_ += denominator_ * value.numerator_;
    denominator_ *= value.denominator_;
    Normalize();
    return *this;
}
Rational operator+(const Rational& left, const Rational& right) {
    Rational copy = left;
    copy += right;
    return copy;
}
Rational& Rational::operator-=(const Rational& value) {
    numerator_ *= value.denominator_;
    numerator_ -= denominator_ * value.numerator_;
    denominator_ *= value.denominator_;
    Normalize();
    return *this;
}
Rational operator-(const Rational& left, const Rational& right) {
    Rational copy = left;
    copy -= right;
    return copy;
}
Rational& Rational::operator*=(const Rational& value) {
    numerator_ *= value.numerator_;
    denominator_ *= value.denominator_;
    Normalize();
    return *this;
}

Rational operator*(const Rational& left, const Rational& right) {
    Rational copy = left;
    copy *= right;
    return copy;
}
Rational& Rational::operator/=(const Rational& value) {
    numerator_ *= value.denominator_;
    denominator_ *= value.numerator_;
    Normalize();
    return *this;
}

Rational operator/(const Rational& left, const Rational& right) {
    Rational copy = left;
    copy /= right;
    return copy;
}
////////////////////////////////Comparison////////////////////////////////////////
bool operator<(const Rational& left, const Rational& right) {
    return left.numerator_ * right.denominator_ < left.denominator_ * right.numerator_;
}

bool operator>(const Rational& left, const Rational& right) {
    return right < left;
}

bool operator>=(const Rational& left, const Rational& right) {
    return !(left < right);
}

bool operator<=(const Rational& left, const Rational& right) {
    return !(right < left);
}

bool operator==(const Rational& left, const Rational& right) {
    return !(left < right || right < left);
}

bool operator!=(const Rational& left, const Rational& right) {
    return (left < right || right < left);
}
/////////////////////////////Other//////////////////////////////////////////
std::string Rational::toString() const {
    std::string returned;
    returned += numerator_.toString();
    if (denominator_ != 1) {
        returned += '/';
        returned += denominator_.toString();
    }
    return returned;
}
BigInteger Rational::Exponentiation(BigInteger x, size_t deg) const {
    if (deg == 0) {
        return BigInteger(1);
    }
    BigInteger result = 1;
    for (size_t i = 0; i < deg; ++i) {
        result *= x;
    }
    return result;
}

std::string Rational::asDecimal(size_t precision) const {
    std::string result_string;
    BigInteger st = 1;
    st = Exponentiation(10, precision);
    BigInteger result = numerator_ * st / denominator_;
    if (numerator_ >= 0) {
        result_string = result.toString();
    }
    else {
        result_string = (-result).toString();
    }
    if (precision == 0) {
        if (numerator_ < 0) {
            result_string = "-" + result_string;
        }
        return result_string;
    }
    while (result_string.size() <= precision) {
        result_string = "0" + result_string;
    }
    result_string = "0" + result_string;
    for (size_t i = 0; i < result_string.size() - precision - 1; ++i) {
        result_string[i] = result_string[i + 1];
    }
    result_string[result_string.size() - precision - 1] = '.';

    if (numerator_ < 0) {
        result_string = "-" + result_string;
    }
    return result_string;
}

Rational::operator double() const {
    double num = static_cast<double>(numerator_);
    double den = static_cast<double>(denominator_);
    return num / den;
}

std::istream& operator>>(std::istream& in, Rational& value) {
    in >> value.numerator_;
    value.denominator_ = 1;
    return in;
}
std::ostream& operator<<(std::ostream& out, const Rational& value) {
    out << value.numerator_;
    return out;
}
template<size_t N>
class Finite {
private:
    long long value;
    bool IfSimple();

public:
    //constructors and smth like this
    Finite(int x);
    Finite() = default;
    ~Finite() = default;
    Finite(const Finite<N>& input);
    Finite<N>& operator=(const Finite<N>& number);

    //ariphmetical operators
    Finite<N>& operator+=(const Finite<N>& x);
    Finite<N>& operator-=(const Finite<N>& x);
    Finite<N>& operator*=(const Finite<N>& x);
    Finite<N>& operator/=(const Finite<N>& x);
    Finite<N>& operator++();
    Finite<N>& operator--();

    //comparison
    bool operator==(const Finite<N>& another) const;
    bool operator!=(const Finite<N>& another) const;

    //output
    friend std::ostream& operator<<(std::ostream& out, const Finite<N>& value) {
        out << value.value;
        return out;
    }
};
//Finite

template<size_t N>
bool Finite<N>::IfSimple() {
    for (size_t i = 2; i <= std::sqrt(N); ++i) {
        if (N % i == 0) return false;
    }
    return true;
}

long long Exponentiation(const long long& x, const long long& N, size_t p) {
    if (p == 0) return 1;

    if (p % 2 == 0) {
        long long  y = Exponentiation(x, N, p / 2);
        return (y * y) % N;
    }
    if (p % 2 == 1) {
        long long  y = Exponentiation(x, N, p - 1);
        return (y * x) % N;
    }
    return 1;
}
//helpful functions

template <size_t N>
bool Finite<N>::operator==(const Finite<N>& another) const {
    return value == another.value;
}

template <size_t N>
bool Finite<N>::operator!=(const Finite<N>& another) const {
    return value != another.value;
}

template <size_t N>
Finite<N>& Finite<N>::operator=(const Finite<N>& another) {
    value = another.value;
    return *this;
}
//comparison



template<size_t N>
Finite<N>::Finite(int x) : value(x){
    int t = static_cast<long long>(N);
    value%= t;
    if(value<0) value += t;
}

template <size_t N>
Finite<N>::Finite(const Finite<N>& another) : value(another.value) {}
//constructors

template<size_t N>
Finite<N>& Finite<N>::operator+=(const Finite<N>& x) {
    value += x.value;
    if (value >= static_cast<long long>(N))
        value %= N;
    return *this;
}
template<size_t N>
Finite<N>& Finite<N>::operator++() {
    return (*this += 1);
}

template<size_t N>
Finite<N>& Finite<N>::operator-=(const Finite<N>& x) {
    value += N;
    value -= x.value;
    value += N;
    value %= N;
    return *this;
}


template<size_t N>
Finite<N>& Finite<N>::operator--() {
    return (*this -= 1);
}

template<size_t N>
Finite<N>& Finite<N>::operator*=(const Finite<N>& x) {
    value *= x.value;
    value %= N;
    return *this;
}



template<size_t N>
Finite<N> operator+(const Finite<N>& left, const Finite<N>& right) {
    Finite<N> copy = left;
    copy += right;
    return copy;
}
template<size_t N>
Finite<N> operator-(const Finite<N>& left, const Finite<N>& right) {
    Finite<N> copy = left;
    copy -= right;
    return copy;
}
template<size_t N>
Finite<N> operator*(const Finite<N>& left, const Finite<N>& right) {
    Finite<N> copy = left;
    copy *= right;
    return copy;
}

template<size_t N>
Finite<N>& Finite<N>::operator/=(const Finite<N>& x) {
    assert(IfSimple());
    assert(x.value != 0);

    value = (value * Exponentiation(x.value, N, N - 2)) % N;
    return *this;
}


template<size_t N>
Finite<N> operator/(const Finite<N>& left, const Finite<N>& right) {
    Finite<N> copy = left;
    copy /= right;
    return copy;
}
//ariphmetical operations


template <size_t N, size_t M, typename Field = Rational>
class Matrix {
private:
    std::vector<std::vector<Field>> container_;
    class Column; //for common matrix
    class ConstColumn; //for const matrix
    int gaus(char x);

public:
    ~Matrix() = default;
    Matrix();
    Matrix(const std::vector<std::vector<Field>>& x);
    Matrix(const std::vector<std::vector<int>>& x);
    //contructors
    template <size_t N1, size_t M1>
    bool operator==(const Matrix<N1, M1, Field>&) const;
    template <size_t N1, size_t M1>
    bool operator!=(const Matrix<N1, M1, Field>&) const;
    //comparisons
    Column operator[](int index);
    ConstColumn operator[](int index) const;
    //for columns class
    Matrix<N, M, Field>& operator+=(const Matrix<N, M, Field>& other);
    Matrix<N, M, Field>& operator-=(const Matrix<N, M, Field>& other);
    Matrix<N, M, Field>& operator*=(const Field& scalar);
    Matrix<N, M, Field>& operator*=(const Matrix<M, M, Field>& other);
    //ariphmetical things
    Matrix<M, N, Field> transposed() const;
    Field det() const;
    size_t rank() const;
    Field trace() const;
    Matrix<N, N, Field> inverted() const;
    void invert();
    std::vector<Field> getRow(size_t index);
    std::vector<Field> getColumn(size_t index);
    //algebraic methods
};

template <size_t N, size_t M, typename Field>
class Matrix<N, M, Field>::Column {
private:
    std::vector<Field>& column_;
public:
    ~Column() = default;
    Column(std::vector<Field>& column);
    Field& operator[](size_t index);
};

template <size_t N, size_t M, typename Field>
Matrix<N, M, Field>::Column::Column(std::vector<Field>& column) : column_(column) {}

template <size_t N, size_t M, typename Field>
Field& Matrix<N, M, Field>::Column::operator[](size_t index) {
    return column_[index];
}
//Columns

template <size_t N, size_t M, typename Field>
class Matrix<N, M, Field>::ConstColumn {
private:
    const std::vector<Field>& column_;
public:
    ~ConstColumn() = default;
    ConstColumn(const  std::vector<Field>& column);
    const Field& operator[](size_t index) const;
};
template <size_t N, size_t M, typename Field>
Matrix<N, M, Field>::ConstColumn::ConstColumn(const std::vector<Field>& column) : column_(column) {}

template <size_t N, size_t M, typename Field>
const Field& Matrix<N, M, Field>::ConstColumn::operator[](size_t index) const {
    return column_[index];
}
//ConstColumns
template <size_t N, size_t M, typename Field>
typename Matrix<N, M, Field>::Column Matrix<N, M, Field>::operator[](int index) {
    return Column(container_[index]);
}

template <size_t N, size_t M, typename Field>
typename Matrix<N, M, Field>::ConstColumn Matrix<N, M, Field>::operator[](int index) const {
    return ConstColumn(container_[index]);
}

template <size_t N, size_t M, typename Field>
std::vector<Field> Matrix<N, M, Field>::getRow(size_t index) {
    return container_[index];


}
template <size_t N, size_t M, typename Field>
std::vector<Field> Matrix<N, M, Field>::getColumn(size_t index) {
    std::vector<Field> result(N);
    for (size_t i = 0; i < N; ++i) {
        result[i] = container_[i][index];
    }
    return result;
}
//Row-Column methods
template<size_t N, size_t M, typename Field>
std::ostream& operator<<(std::ostream& out, const Matrix < N, M, Field>& value) {
    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < M; ++j) {
            out << value[i][j] << " ";
        }
        out << '\n';
    }
    return out;
}
//Output
template<size_t N, size_t M, typename Field>
int Matrix < N, M, Field>::gaus(char x) {
    size_t start_column_number = 0;
    for (size_t row_number = 0; row_number < M; ++row_number) {
        size_t column_number = start_column_number;
        for (; column_number < N; ++column_number) {
            if (container_[column_number][row_number] != 0) break;
        }
        if (column_number == N) {
            if (x == 'd') return 0;
        }
        else {
            if (column_number != start_column_number) std::swap(container_[start_column_number], container_[column_number]);
            for (size_t j = start_column_number + 1; j < N; ++j) {
                Field coef = container_[j][row_number] / container_[start_column_number][row_number];
                for (size_t t = row_number; t < M; ++t) {
                    container_[j][t] -= container_[start_column_number][t] * coef;
                }
            }
            ++start_column_number;
        }
    }
    return 1;
}
template<size_t N, size_t M, typename Field>
size_t Matrix<N, M, Field>::rank() const {
    Matrix<N, M, Field> copy = *this;
    copy.gaus('r');
    size_t ans = N;
    for (size_t i = 0; i < N; ++i) {
        size_t j = 0;
        while (j < M) {
            if (copy[N - i - 1][j] != 0) break;
            ++j;
        }
        if (j == M) --ans;
    }
    return ans;
}

template<size_t N, size_t M, typename Field>
Matrix<M, N, Field> Matrix<N, M, Field>::transposed() const {
    Matrix<M, N, Field> result;
    for (size_t i = 0; i < M; ++i) {
        for (size_t j = 0; j < N; ++j) {
            result[i][j] = container_[j][i];
        }
    }
    return result;
}

template <size_t N, size_t M, typename Field>
Field Matrix<N, M, Field>::trace() const {
    assert(N == M);
    Field result = 0;
    for (size_t i = 0; i < N; ++i) {
        result += container_[i][i];
    }
    return result;
}

template <size_t N, size_t M, typename Field>
void Matrix<N, M, Field>::invert() {
    assert(N == M);
    Matrix<N, N * 2, Field> double_matrix;
    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < M; ++j) {            
            double_matrix[i][j] = container_[i][j]; 
        }                                           // there are our matrix 
    }                                               // and identity matrix 
    for (size_t i = 0; i < N; ++i) {                // in double_matrix
        double_matrix[i][i + N] = 1;
    }
    size_t start_column_number = 0;
    for (size_t row_number = start_column_number; row_number < M; ++row_number) {
        size_t column_number = start_column_number;
        for (; column_number < N; ++column_number) {
            if (double_matrix[column_number][row_number] != 0) break;
        }
        if (column_number != start_column_number) {
            for (size_t i = row_number; i < 2 * N; ++i) {
                Field swap = double_matrix[start_column_number][i];
                double_matrix[start_column_number][i] = double_matrix[column_number][i];
                double_matrix[column_number][i] = swap;
            }
        }
        Field coef = double_matrix[start_column_number][row_number];
        for (size_t i = row_number; i < 2 * N; ++i) {
            double_matrix[start_column_number][i] /= coef;
        }
        for (size_t j = 0; j < N; ++j) {
            if (j != start_column_number) {
                coef = double_matrix[j][row_number] / double_matrix[start_column_number][row_number];
                for (size_t t = row_number; t < 2 * N; ++t) {
                    double_matrix[j][t] -= double_matrix[start_column_number][t] * coef;
                }
            }
        }
        ++start_column_number;
    }
    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < M; ++j) {
            container_[i][j] = double_matrix[i][j + N];
        }
    }
}

template <size_t N, size_t M, typename Field>
Matrix<N, N, Field> Matrix<N, M, Field>::inverted() const {
    assert(M == N);
    Matrix<N, M, Field> copy = *this;
    copy.invert();
    return copy;
}

template <size_t N, size_t M, typename Field>
Field  Matrix<N, M, Field>::det() const {
    assert(M == N);
    Matrix<N, M, Field> copy = *this;
    if (copy.gaus('d') == 0) return 0;
    Field ans = 1;
    for (size_t i = 0; i < M; ++i) {
        ans *= copy.container_[i][i];
    }
    return ans;
}

//Matrix Algebra

template <size_t N, size_t M, typename Field>
Matrix<N, M, Field>& Matrix<N, M, Field>::operator*=(const Matrix<M, M, Field>& other) {
    for (size_t i = 0; i < N; ++i) {
        std::vector<Field> ans(M);
        for (size_t j = 0; j < M; ++j) {
            ans[j] = 0;
            for (size_t k = 0; k < M; ++k) {
                ans[j] += container_[i][k] * other.container_[k][j];
            }
        }
        container_[i] = ans;
    }
    return *this;
}

template <size_t N, size_t M, size_t K, typename Field>
Matrix<N, K, Field> operator*(const Matrix<N, M, Field>& first, const Matrix<M, K, Field>& second) {
    Matrix<N, K, Field> ans;
    if (N == K) {
        for (size_t j = 0; j < N; ++j)
            ans[j][j] = 0;
    }
    for (size_t j = 0; j < N; ++j) {
        for (size_t t = 0; t < M; ++t) {
            for (size_t i = 0; i < K; ++i) {
                ans[j][i] += first[j][t] * second[t][i];
            }
        }
    }
    return ans;
}

template <size_t N, size_t M, typename Field>
Matrix<N, M, Field> operator*(Matrix<N, M, Field> matr, Field scalar) {
    Matrix<N, M, Field> copy = matr;
    copy *= scalar;
    return copy;
}

template <size_t N, size_t M, typename Field>
Matrix<N, M, Field> operator*(Field scalar, Matrix<N, M, Field> matr) {
    Matrix<N, M, Field> copy = matr;
    copy *= scalar;
    return copy;
}

template <size_t N, size_t M, typename Field>
Matrix<N, M, Field>& Matrix<N, M, Field>::operator+=(const Matrix<N, M, Field>& another) {
    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < M; ++j) {
            container_[i][j] += another.container_[i][j];
        }
    }
    return *this;
}

template <size_t N, size_t M, typename Field>
Matrix<N, M, Field>& Matrix<N, M, Field>::operator-=(const Matrix<N, M, Field>& another) {
    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < M; ++j) {
            container_[i][j] -= another.container_[i][j];
        }
    }
    return *this;
}

template <size_t N, size_t M, typename Field>
Matrix<N, M, Field>& Matrix<N, M, Field>::operator*=(const Field& scalar) {
    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < M; ++j) {
            container_[i][j] *= scalar;
        }
    }
    return *this;
}

template<size_t N, size_t M, typename Field>
Matrix<N, M, Field> operator-(const Matrix<N, M, Field>& first, const Matrix<N, M, Field>& second) {
    Matrix<N, M, Field> copy = first;
    copy -= second;
    return copy;
}
template<size_t N, size_t M, typename Field>
Matrix<N, M, Field> operator+(const Matrix<N, M, Field>& first, const Matrix<N, M, Field>& second) {
    Matrix<N, M, Field> copy = first;
    copy -= second;
    return copy;
}
//Ariphmetical operators
template <size_t N, size_t M, typename Field>
Matrix<N, M, Field>::Matrix() {
    container_ = std::vector<std::vector<Field>>(N, std::vector<Field>(M));
    for (size_t i = 0; i < std::min(N, M); ++i) {
        container_[i][i] = Field(1);
    }
}

template <size_t M, size_t N, typename Field>
Matrix<M, N, Field>::Matrix(const std::vector<std::vector<Field>>& x) {
    container_ = x;
}

template <size_t N, size_t M, typename Field>
Matrix<N, M, Field>::Matrix(const std::vector<std::vector<int>>& source) {
    container_ = std::vector<std::vector<Field>>(N, std::vector<Field>(M));
    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < M; ++j) {
            container_[i][j] = Field(source[i][j]);
        }
    }
}
//Constructors
template <size_t N, size_t M, typename Field>
template<size_t N1, size_t M1>
bool Matrix<N, M, Field>::operator==(const Matrix<N1, M1, Field>& another) const {
    if (M != M1)
        return false;
    if (N != N1)
        return false;
    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < M; ++j) {
            if (container_[i][j] != another.container_[i][j])
                return false;
        }
    }
    return true;
}

template <size_t N, size_t M, typename Field>
template<size_t N1, size_t M1>
bool Matrix<N, M, Field>::operator!=(const Matrix<N1, M1, Field>& another) const {
    return !(*this == another);
}
//Comparison

template <size_t N, typename Field = Rational>
using SquareMatrix = Matrix<N, N, Field>;

