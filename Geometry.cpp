#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <math.h>
#include <cassert>

const double pi = 3.141592653589793;

struct Point {
	double x;
	double y;

	void rotate(Point center, double angle);
	void reflex(Point center);
	void reflex(class Line axis);
	void scale(Point center, double coefficient);

	Point() = default;
	Point(class Line first, class Line second);
	Point(double x, double y);

	Point& operator=(const Point& point);
	bool operator==(const Point& point) const;
	bool operator!=(const Point& point) const;
	Point operator+(const Point& another) const;
	Point operator-(const Point& another) const;
	Point operator*(double scalar) const;
	Point operator/(double scalar) const;
};
//Point
class Line {
private:
	//Ax + By + C = 0
	double A;
	double B;
	double C;

	friend struct Point;
public:

	Line(double scalar1, double scalar2);
	Line(const struct Point& point, double scalar);
	Line(double scalar, const struct Point& point);
	Line(const struct Point& point1, const struct Point& point2);
	Line(const Line& another, const struct Point& point);

	bool operator==(const Line& line) const;
	bool operator!=(const Line& line) const;


};
//Line
class Shape {
public:
	std::string shape_name;

	Shape() = default;
	virtual ~Shape() {};

	virtual void rotate(Point center, double angle) = 0;
	virtual void reflex(Point center) = 0;
	virtual void reflex(Line axis) = 0;
	virtual void scale(Point center, double coefficient) = 0;

	virtual double perimeter() const = 0;
	virtual double area() const = 0;

	virtual bool operator==(const Shape& another) const = 0;
	virtual bool operator!=(const Shape& another) const = 0;
	virtual bool isCongruentTo(const Shape& another) const = 0;
	virtual bool isSimilarTo(const Shape& another)  const = 0;
	virtual bool containsPoint(Point point)  const = 0;
};
//Shape
class Polygon : public Shape {
protected:
	std::vector <struct Point> vertices;
public:
	Polygon() = default;
	Polygon(std::vector <Point> Vertex_);
	Polygon(std::initializer_list<Point> points);

	Polygon& operator=(const Polygon& polygon);

	void rotate(Point center, double angle);
	void reflex(Point center);
	void reflex(class Line axis);
	void scale(Point center, double coefficient);

	double perimeter() const;
	double area() const;
	int verticesCount();

	bool operator==(const Shape& another) const;
	bool operator!=(const Shape& another) const;
	bool isCongruentTo(const Shape& another) const;
	bool isSimilarTo(const Shape& another)  const;
	bool containsPoint(Point point)  const;
	bool isConvex() const;

	const std::vector<struct Point> getVertices() const;

};
//Polygon
class Ellipse : public Shape {
protected:
	Point focus1;
	Point focus2;
	double rad;
public:
	Ellipse() = default;
	Ellipse(Point focus1_, Point focus2_, double radius_);

	Ellipse& operator=(const Ellipse& ellipse);

	double perimeter() const;
	double area() const;
	bool operator==(const Shape& another) const;
	bool operator!=(const Shape& another) const;
	bool isCongruentTo(const Shape& another) const;
	bool isSimilarTo(const Shape& another)  const;
	bool containsPoint(Point point)  const;

	void rotate(Point center, double angle);
	void reflex(Point center);
	void reflex(Line axis);
	void scale(Point center, double coefficient);

	std::pair<Point, Point> focuses() const;
	std::pair<Line, Line> directrices() const;
	double eccentricity() const;
	Point center() const;

	double a() const;
	double b() const;
};
//Ellipse

class Circle : public Ellipse {
public:
	Circle() = default;
	Circle(Point focus_, double radius_);
	Circle(Point first, Point second, Point third);

	double radius() const;
};
//Circle
class Rectangle : public Polygon {

public:
	Rectangle() = default;
	Rectangle(Point first, Point second, double coefficient);

	Point center();
	std::pair<Line, Line> diagonals();
};
//Rectangle
class Square : public Rectangle {
protected:

public:
	Square() = default;
	Square(Point first, Point second);

	class Circle circumscribedCircle();
	class Circle inscribedCircle();
};
//Square

class Triangle : public Polygon {
protected:

public:
	Triangle() = default;
	Triangle(Point first, Point second, Point third);

	class Circle circumscribedCircle();
	class Circle inscribedCircle();
	Point centroid();
	Point orthocenter();
	Line EulerLine();
	class Circle ninePointsCircle();
};
//Triangle

////////////////////////////////////////////////////////////////////////////////////////////

bool IsDoubleEqual(double x, double y) {
	return (std::abs(x - y) < 0.000001);
}

Point::Point(double x, double y) : x(x), y(y) {}

Point& Point::operator=(const Point& point) {
	x = point.x;
	y = point.y;
	return *this;
}

bool Point::operator==(const Point& point) const {
	return (IsDoubleEqual(x, point.x) && IsDoubleEqual(y, point.y));
}

bool Point::operator!=(const Point& point) const {
	return !(*this == point);
}

Point Point::operator+(const Point& another) const {
	Point point(x + another.x, y + another.y);
	return point;
}

Point Point::operator-(const Point& another) const {
	Point point(x - another.x, y - another.y);
	return point;
}

Point Point::operator*(double scalar) const {
	Point point(x * scalar, y * scalar);
	return point;
}

Point Point::operator/(double scalar) const {
	Point point(x / scalar, y / scalar);
	return point;
}

void Point::rotate(Point center, double angle) {
	double x2 = x;
	double y2 = y;
	double x1 = center.x;
	double y1 = center.y;
	x = x1 + (x2 - x1) * cos(angle * pi / 180) - (y2 - y1) * sin(angle * pi / 180);
	y = y1 + (x2 - x1) * sin(angle * pi / 180) + (y2 - y1) * cos(angle * pi / 180);
}

void Point::reflex(Point point) {
	*this = point + point - *this;
}

void Point::scale(Point point, double scalar) {
	*this = point + ((*this - point) * scalar);
}

double Length(const Point& first, const Point& second) {
	return sqrt((first.x - second.x) * (first.x - second.x) + (first.y - second.y) * (first.y - second.y));
}

Point::Point(class Line first, class Line second) {
	double det = first.A * second.B - first.B * second.A;
	y = (first.C * second.A - first.A * second.C) / det;
	x = (first.B * second.C - first.C * second.B) / det;
}

void Point::reflex(class Line axis) {
	Line line(axis, *this);
	Point center(axis, line);
	reflex(center);
}

//Point

Line::Line(double scalar1, double scalar2) : A(scalar1), B(-1), C(scalar2) {}

Line::Line(const struct Point& point, double scalar) : A(scalar), B(-1), C(point.y - (point.x * scalar)) {}

Line::Line(double scalar, const struct Point& point) : A(scalar), B(-1), C(point.y - (point.x * scalar)) {}

Line::Line(const Line& line, const struct Point& point) : A(-line.B), B(line.A), C(-(A * point.x + B * point.y)) {}

Line::Line(const struct Point& first, const struct Point& second) {
	if (first.x == second.x) {
		B = 0;
		A = 1;
		C = -first.x;
		return;
	}
	if (first.y == second.y) {
		B = 1;
		A = 0;
		C = -first.y;
		return;
	}
	B = -1;
	C = (second.y * first.x - second.x * first.y) / (first.x - second.x);
	if (first.x != 0) {
		A = (first.y - C) / first.x;
		return;
	}
	A = (second.y - C) / second.x;
}

bool Line::operator==(const Line& line) const {
	return (IsDoubleEqual(A * line.B, B * line.A) && IsDoubleEqual(A * line.C, C * line.A) && IsDoubleEqual(C * line.B, B * line.C));
}

bool Line::operator!=(const Line& line) const {
	return !(*this == line);
}

double Length(const class Line& line, const struct Point& first) {
	return Length(first, Point(line, Line(line, first)));
}

//Line

Polygon::Polygon(std::vector <struct Point> Vertex_) : vertices(std::move(Vertex_)) {}

Polygon::Polygon(std::initializer_list<Point> points) : vertices(points) {}

Polygon& Polygon::operator=(const Polygon& polygon) {
	vertices = polygon.vertices;
	return *this;
}

int Polygon::verticesCount() {
	return vertices.size();
}

const std::vector<struct Point> Polygon::getVertices() const{
	return vertices;
}

double Polygon::perimeter() const {
	double ans = 0;
	for (size_t i = 1; i < vertices.size(); ++i) {
		ans += Length(vertices[i], vertices[i - 1]);
	}
	ans += Length(vertices[0], vertices[vertices.size() - 1]);
	return ans;
}

double Polygon::area() const {
	double ans = 0;
	for (size_t i = 1; i < vertices.size(); ++i) {
		ans += (vertices[i].x - vertices[i - 1].x) * (vertices[i].y + vertices[i - 1].y);
	}
	ans += (vertices[0].x - vertices[vertices.size() - 1].x) * (vertices[0].y + vertices[vertices.size() - 1].y);
	return std::abs(ans / 2);
}

bool Polygon::operator==(const Shape& another) const {
	auto pol1 = dynamic_cast<const Polygon*>(&another);
	if (!pol1) return false;
	const Polygon& pol = dynamic_cast<const Polygon&> (another);
	if (vertices.size() != pol.vertices.size()) {
		return false;
	}
	size_t i = 0;
	while (i < vertices.size()) {
		if (vertices[i] == pol.vertices[0])
			break;
		++i;
	}
	if (i == vertices.size()) {
		return false;
	}
	size_t amount1 = 0;
	size_t amount2 = 0;
	for (size_t j = 0; j < vertices.size(); ++j) {
		if (vertices[(i + j) % vertices.size()] == pol.vertices[j])
			++amount1;
		if (vertices[(vertices.size() + i - j) % vertices.size()] == pol.vertices[j])
			++amount2;
	}
	if (amount1 == vertices.size() || amount2 == vertices.size()) {
		return true;
	}
	return false;

}

bool Polygon::operator!=(const Shape& another) const {
	return !(*this == another);
}

bool Polygon::isCongruentTo(const Shape& another) const {
	if (!isSimilarTo(another))
		return false;
	const Polygon& poly = dynamic_cast<const Polygon&> (another);
	if (IsDoubleEqual(poly.perimeter(), perimeter())) {
		return true;
	}
	return false;
}

bool Polygon::isSimilarTo(const Shape& another)  const { //not working
	if (dynamic_cast<const Polygon*>(&another) == nullptr) {
		return false;
	}
	const Polygon& pol = dynamic_cast<const Polygon&> (another);
	if (vertices.size() != pol.vertices.size()) {
		return false;
	}
	size_t amount = 0;
	size_t size = vertices.size();
	for (size_t i = 0; i < size; ++i) {
		for (size_t j = 0; j < size; ++j) {
			size_t now = (i + j);
			double first = Length(vertices[now % size], vertices[(now + 1) % size]);
			double second = Length(vertices[(now + 1) % size], vertices[(now + 2) % size]);
			double pfirst = Length(pol.vertices[j % size], pol.vertices[(j + 1) % size]);
			double psecond = Length(pol.vertices[(j + 1) % size], pol.vertices[(j + 2) % size]);
			double height = Length(Line(vertices[now % size], vertices[(now + 1) % size]), vertices[(now + 2) % size]);
			double pheight = Length(Line(pol.vertices[j % size], pol.vertices[(j + 1) % size]), pol.vertices[(j + 2) % size]);
			if (IsDoubleEqual(first * psecond, pfirst * second) && IsDoubleEqual(first * pheight, height * pfirst))
				++amount;
		}
		if (amount == size) {
			return true;
		}
		amount = 0;
	}
	for (size_t i = 0; i < size; ++i) {
		for (size_t j = 0; j < size; ++j) {
			size_t now = (size + i - j);
			double second = Length(vertices[now % size], vertices[(now + 1) % size]);
			double first = Length(vertices[(now + 1) % size], vertices[(now + 2) % size]);
			double pfirst = Length(pol.vertices[j % size], pol.vertices[(j + 1) % size]);
			double psecond = Length(pol.vertices[(j + 1) % size], pol.vertices[(j + 2) % size]);
			double height = Length(Line(vertices[(now + 2) % size], vertices[(now + 1) % size]), vertices[(now) % size]);
			double pheight = Length(Line(pol.vertices[j % size], pol.vertices[(j + 1) % size]), pol.vertices[(j + 2) % size]);
			if (IsDoubleEqual(first * psecond, pfirst * second) && IsDoubleEqual(first * pheight, height * pfirst))
				++amount;
		}
		if (amount == size) {
			return true;
		}
		amount = 0;
	}
	return false;
}

bool Polygon::containsPoint(Point point)  const {
	bool amount = false;
	size_t n = vertices.size();
	for (size_t i = 0; i < vertices.size(); ++i) {
		bool between = (vertices[i].y <= point.y && vertices[(i + n - 1) % n].y >= point.y) || (vertices[(i + n - 1) % n].y <= point.y && vertices[i].y >= point.y);
		bool right = Point(Line(vertices[i], vertices[(i + n - 1) % n]), Line(0, point)).x < point.x;
		if (between && right)
			amount = !amount;
	}
	return amount;
}

void Polygon::rotate(Point center, double angle) {
	for (size_t i = 0; i < vertices.size(); ++i)
		vertices[i].rotate(center, angle);
}

void Polygon::reflex(Point center) {
	for (size_t i = 0; i < vertices.size(); ++i)
		vertices[i].reflex(center);
}

void Polygon::reflex(Line axis) {
	for (size_t i = 0; i < vertices.size(); ++i)
		vertices[i].reflex(axis);
}

void Polygon::scale(Point center, double coefficient) {
	for (size_t i = 0; i < vertices.size(); ++i)
		vertices[i].scale(center, coefficient);
}

bool Polygon::isConvex() const{
	std::vector <struct Point> copy = vertices;
	size_t size = vertices.size();
	if (size < 4) return true;
	for (size_t i = 0; i < size; ++i) {
		double area1 = area();
		copy[i].reflex(Line(copy[(i + size - 1) % size], copy[(i + 1) % size]));
		double area2 = area();
		copy[i].reflex(Line(copy[(i + size - 1) % size], copy[(i + 1) % size]));
		if (area1 < area2) return false;
	}
	return true;
}
//Poly

Ellipse::Ellipse(Point focus1_, Point focus2_, double radius_) : focus1(focus1_), focus2(focus2_), rad(radius_) {}

Ellipse& Ellipse::operator=(const Ellipse& ellipse) {
	focus1 = ellipse.focus1;
	focus2 = ellipse.focus2;
	rad = ellipse.rad;
	return *this;
}

std::pair<Point, Point> Ellipse::focuses() const {
	return { focus1, focus2 };
}

double Ellipse::a() const {
	return rad / 2;
}

double Ellipse::b() const {
	return sqrt(a() * a() - Length(focus1, center()) * Length(focus1, center()));
}

double Ellipse::perimeter() const {
	double perimeter = pi * (3 * (a() + b()) - sqrt((3 * a() + b()) * (a() + 3 * b())));
	return perimeter;
}

double Ellipse::area() const {
	return pi * a() * b();
}

bool Ellipse::operator==(const Shape& another) const {
	if (dynamic_cast<const Polygon*>(&another) == nullptr) return false;
	const Ellipse& el = dynamic_cast<const Ellipse&> (another);
	if (Length(focus1, el.focuses().first) < 0.000001 && Length(focus2, el.focuses().second) < 0.000001 && IsDoubleEqual(a(), el.a())) return true;
	if (Length(focus2, el.focuses().first) < 0.000001 && Length(focus1, el.focuses().second) < 0.000001 && IsDoubleEqual(a(), el.a())) return true;
	return false;
}

bool Ellipse::operator!=(const Shape& another) const {
	return !(*this == another);
}

bool Ellipse::isCongruentTo(const Shape& another) const {
	if (dynamic_cast<const Polygon*>(&another) == nullptr) return false;
	const Ellipse& el = dynamic_cast<const Ellipse&> (another);

	if (IsDoubleEqual(a(), el.a()) && IsDoubleEqual(b(), el.b())) return true;
	if (IsDoubleEqual(a(), el.b()) && IsDoubleEqual(b(), el.a())) return true;
	return false;
}

bool Ellipse::isSimilarTo(const Shape& another)  const {
	if (dynamic_cast<const Polygon*>(&another) == nullptr) return false;
	const Ellipse& el = dynamic_cast<const Ellipse&> (another);
	if (IsDoubleEqual(a() * el.a(), b() * el.b())) return true;
	if (IsDoubleEqual(a() * el.b(), b() * el.a())) return true;

	return false;
}

bool Ellipse::containsPoint(Point point)  const {
	if ((Length(point, focus1) + Length(point, focus2)) <= rad)
		return true;
	return false;
}

std::pair<Line, Line> Ellipse::directrices() const {
	Line line(focus1, focus2);
	Point p1 = center() + ((focus1 - center()) / eccentricity());
	Point p2 = center() + ((focus2 - center()) / eccentricity());
	Line first(line, p1);
	Line second(line, p2);
	return { first, second };
}

double Ellipse::eccentricity() const {
	return Length(focus1, focus2) / rad;
}

Point Ellipse::center() const {
	return (focus1 + focus2) / 2;
}


void Ellipse::rotate(Point center, double angle) {
	focus1.rotate(center, angle);
	focus2.rotate(center, angle);
}

void Ellipse::reflex(Point center) {
	focus1.reflex(center);
	focus2.reflex(center);
}

void Ellipse::reflex(Line axis) {
	focus1.reflex(axis);
	focus2.reflex(axis);
}

void Ellipse::scale(Point center, double coefficient) {
	focus1.scale(center, coefficient);
	focus2.scale(center, coefficient);
	rad *= coefficient;
}

//Ellipse

Circle::Circle(Point focus_, double radius_) {
	focus1 = focus_;
	focus2 = focus_;
	rad = 2 * radius_;
}

Circle::Circle(Point first, Point second, Point third) {
	Line AB(first, second);
	Line BC(third, second);
	Line f(AB, (first + second) / 2);
	Line s(BC, (third + second) / 2);
	Point focus_(f, s);
	focus1 = focus_;
	focus2 = focus_;
	rad = 2 * Length(focus_, first);
}

double Circle::radius() const {
	return rad / 2;
}

//Circle

Rectangle::Rectangle(Point first, Point second, double scalar) {
	if (scalar > 1) scalar = 1 / scalar;
	vertices.resize(4);
	vertices[0] = first;
	vertices[2] = second;
	double coeffitient = 1 / (1 + scalar * scalar);
	Point new2 = second;
	Point new1 = first;
	new2.scale(first, coeffitient);
	new1.rotate(new2, 270);
	new1.scale(new2, 1 / scalar);
	new2 = (vertices[0] + vertices[2]) - new1;
	vertices[1] = new1;
	vertices[3] = new2;
}

Point Rectangle::center() {
	return (vertices[0] + vertices[2]) / 2;
}

std::pair<Line, Line> Rectangle::diagonals() {
	Line first(vertices[0], vertices[2]);
	Line second(vertices[1], vertices[3]);
	return { first, second };
}
//Rectangle

Square::Square(Point first, Point second) {
	vertices.resize(4);
	vertices[0] = first;
	vertices[2] = second;
	double coeffitient = (double)1 / (double)2;
	Point new2 = second;
	Point new1 = first;
	new2.scale(first, coeffitient);
	new1.rotate(new2, 270);
	new2 = (vertices[0] + vertices[2]) - new1;
	vertices[1] = new1;
	vertices[3] = new2;
}

//Square

class Circle Square::circumscribedCircle() {
	Circle circle(vertices[0], vertices[1], vertices[2]);
	return circle;
}

class Circle Square::inscribedCircle() {
	Circle circle((vertices[0] + vertices[1]) / 2, (vertices[1] + vertices[2]) / 2, (vertices[2] + vertices[3]) / 2);
	return circle;
}

//Circle

Triangle::Triangle(Point first, Point second, Point third) {
	vertices.resize(3);
	vertices[0] = first;
	vertices[1] = second;
	vertices[2] = third;
}

class Circle Triangle::circumscribedCircle() {
	Circle circle(vertices[0], vertices[1], vertices[2]);
	return circle;
}

class Circle Triangle::inscribedCircle() {
	Line AB(vertices[0], vertices[1]);
	Line BC(vertices[2], vertices[1]);
	double AB_length = Length(vertices[0], vertices[1]);
	double AC_length = Length(vertices[0], vertices[2]);
	double BC_length = Length(vertices[2], vertices[1]);
	Line bisector1(vertices[2], ((vertices[0] * BC_length) + (vertices[1] * AC_length)) / (BC_length + AC_length));
	Line bisector2(vertices[0], ((vertices[1] * AC_length) + (vertices[2] * AB_length)) / (AB_length + AC_length));
	Point focus_(bisector1, bisector2);
	double radius_ = Length(AB, focus_);
	Circle circle(focus_, radius_);
	return circle;
}

Point Triangle::centroid() {
	return Point(Line((vertices[0] + vertices[1]) / (double)2, vertices[2]), Line((vertices[1] + vertices[2]) / (double)2, vertices[0]));
}

Point Triangle::orthocenter() {
	return Point(Line(Line(vertices[0], vertices[1]), vertices[2]), Line(Line(vertices[0], vertices[2]), vertices[1]));
}

Line Triangle::EulerLine() {
	return Line(centroid(), orthocenter());
}

class Circle Triangle::ninePointsCircle() {
	return Circle((vertices[0] + vertices[1]) / 2, (vertices[1] + vertices[2]) / 2, (vertices[2] + vertices[0]) / 2);
}

//Triangle
