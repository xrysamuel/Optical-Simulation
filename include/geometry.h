#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <exception>
#include <cmath>

// Division by zero error
class ZeroDivisionException : public std::exception
{
};

// Two-dimensional vector, supports operations such as vector addition, subtraction, scalar multiplication, dot product, cross product, and equality check
struct Vec
{
    double x;
    double y;
    Vec operator+(const Vec &other) const { return {x + other.x, y + other.y}; }
    Vec operator-(const Vec &other) const { return {x - other.x, y - other.y}; }
    Vec operator-() const { return {-x, -y}; }
    Vec Scale(double d) const { return {x * d, y * d}; }
    double Dot(const Vec &other) const { return x * other.x + y * other.y; }
    bool operator==(const Vec &other) const { return (x == other.x) && (y == other.y); }
    Vec Rotate90Anticlockwise() const { return {-y, x}; }
    Vec Rotate90Clockwise() const { return {y, -x}; }
    Vec SlipY() const { return {x, -y}; }
    Vec SlipX() const { return {-x, y}; }
    double Norm() const { return std::sqrt(x * x + y * y); }
    double NormSquare() const { return x * x + y * y; }
    Vec Normalize() const
    {
        double n = Norm();
        if (n == 0.0)
            throw ZeroDivisionException();
        return {x / n, y / n};
    }
    Vec Projection(const Vec &other) const
    {
        double ns = other.NormSquare();
        if (ns == 0.0)
            throw ZeroDivisionException();
        return other.Scale(Dot(other) / other.NormSquare());
    }
};

// Two-dimensional point
using Point = Vec;

// Zero vector
const Vec kZeroVec = Vec{0.0, 0.0};

// 2D directed line
class Line
{
protected:
    Point start_;
    Vec direction_;

public:
    // Construct a directed line using a specified starting point s and direction vector d; an exception will be thrown if the direction vector is zero
    Line(const Point &s, const Vec &d) : start_(s), direction_(d)
    {
        if (d == kZeroVec)
            throw ZeroDivisionException{};
    }
    Vec GetStart() const { return start_; }
    Vec GetDirection() const { return direction_; }
    // Get point s + d * parameter on the directed line
    Vec GetPoint(double parameter) const { return start_ + direction_.Scale(parameter); }
    virtual bool ParameterWithinBounds(double parameter) const { return true; }
};

// 2D ray
class Ray : public Line
{
public:
    Ray(const Point &s, const Vec &d) : Line(s, d){};
    virtual bool ParameterWithinBounds(double parameter) const override
    {
        return (parameter >= 0);
    }
};

// 2D directed line segment
class Segment : public Line
{
public:
    // Construct a directed line segment, of which the endpoint is s + d
    Segment(const Point &s, const Vec &d) : Line(s, d){};
    Vec GetEnd() const { return start_ + direction_; }
    virtual bool ParameterWithinBounds(double parameter) const override
    {
        return (parameter >= 0 && parameter <= 1);
    }
};

// Relationships between two directed lines
struct Intersection
{
    enum NumIntersects
    {
        ZeroIntersection, // Zero intersection points, or coincident
        OneIntersection,  // One intersection point
    } num_intersects;     // Number of intersection points
    double parameter1;    // Parameter of the intersection point with respect to the first line
    double parameter2;    // Parameter of the intersection point with respect to the second line
};

// Get the relationships between directed lines
Intersection GetLineIntersection(const Line &l1, const Line &l2);

#endif