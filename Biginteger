#include<iostream>
#include<string>
#include<vector>

class BigInteger {
private:
    static const int BASE = 1000;
    std::vector<int> digits_;
    bool is_negative_;

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
    BigInteger& operator=(BigInteger copy);
    void Swap(BigInteger& swapped);


    //comparison
    friend bool operator==(const BigInteger& left, const BigInteger& right);
    friend bool operator!=(const BigInteger& left, const BigInteger& right);
    friend bool operator>(const BigInteger& left, const BigInteger& right);
    friend bool operator>=(const BigInteger& left, const BigInteger& right);
    friend bool operator<(const BigInteger& left, const BigInteger& right);
    friend bool operator<=(const BigInteger& left, const BigInteger& right);

    //ariphmetical operators
    friend const BigInteger operator-(const BigInteger& value);
    BigInteger& operator+=(const BigInteger&);
    BigInteger& operator*=(const BigInteger&);
    BigInteger& operator/=(const BigInteger&);
    BigInteger& operator-=(const BigInteger&);
    BigInteger& operator%=(const BigInteger&);
    friend BigInteger operator+(const BigInteger& left, const BigInteger& right);
    friend BigInteger operator-(const BigInteger& left, const BigInteger& right);
    friend BigInteger operator*(const BigInteger& left, const BigInteger& right);
    friend BigInteger operator/(const BigInteger& left, const BigInteger& right);
    friend BigInteger operator%(const BigInteger& left, const BigInteger& right);
    //increment, decrement
    BigInteger& operator++();
    BigInteger operator++(int);
    BigInteger& operator--();
    BigInteger operator--(int);
    //other
    size_t size() const;
    void ToCorrectForm();
    void Clear();
    std::string toString() const;
    void ChangeSign();
    void Increase(int x);
    void Decrease();
};


class Rational {
private:
    BigInteger numerator_;
    BigInteger denominator_;
    BigInteger Gcd(BigInteger x, BigInteger y);
    void Normalize();
public:
    //constructors
    Rational();
    Rational(int x);
    Rational(const BigInteger& x);
    Rational(const BigInteger& numerator, const BigInteger& denominator);
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
    BigInteger Exponentiation(BigInteger x, size_t deg) const;
    explicit operator double() const;
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
    size_t length = 3;
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
//CONSTRUCTORS
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
BigInteger& BigInteger::operator=(BigInteger copy) {
    Swap(copy);
    return *this;
}

void BigInteger::Swap(BigInteger& swapped) {
    std::swap(is_negative_, swapped.is_negative_);
    std::swap(digits_, swapped.digits_);
}
//COMPARISON
bool operator==(const BigInteger& left, const BigInteger& right) {
    BigInteger x = left - right;
    return (x.digits_[0] == 0 && x.size() <= 1);
}

bool operator!=(const BigInteger& left, const BigInteger& right) {
    return !(left == right);
}

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

bool operator>=(const BigInteger& left, const BigInteger& right) {
    return !(left < right);
}

bool operator<=(const BigInteger& left, const BigInteger& right) {
    return !(left > right);
}

//ARIPMETICAL OPERATORS

const BigInteger operator-(const BigInteger& value) {
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
            if (!(*this < divisor_copy * mid)) {
                left = mid;
            }
            else {
                right = mid;
            }
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
        ToCorrectForm();
        return *this;
    }
    --digits_[0];
    ToCorrectForm();
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
        ToCorrectForm();
        return *this;
    }
    --digits_[0];
    ToCorrectForm();
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

BigInteger Rational::Gcd(BigInteger x, BigInteger y) {
    x.is_negative_ = false;
    y.is_negative_ = false;
    if (x == 0) {
        return y;
    }
    if (y == 0) {
        return x;
    }
    if (x < y) {
        return Gcd(x, y % x);
    }
    return Gcd(y, x % y);
}
void Rational::Normalize() {
    if (denominator_.is_negative_) {
        denominator_.is_negative_ = false;
        numerator_.is_negative_ = !numerator_.is_negative_;
    }
    BigInteger t = Gcd(numerator_, denominator_);
    numerator_ /= t;
    denominator_ /= t;
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
//Comparison
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
//Other
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
