#include "geometry.h"
#include <iostream>
#include <vector>

// Get the relationships between directed lines
Intersection GetLineIntersection(const Line &l1, const Line &l2)
{
    Vec d1 = l1.GetDirection();
    Vec d2 = l2.GetDirection();
    Vec d1_o = d1.Rotate90Clockwise();
    Vec d2_o = d2.Rotate90Clockwise();
    double den1 = d1.Dot(d2_o);
    double den2 = d2.Dot(d1_o);
    if (den1 == 0 || den2 == 0)
        return Intersection{Intersection::ZeroIntersection, 0., 0.};
    Vec ds21 = l2.GetStart() - l1.GetStart();
    double t1 = ds21.Dot(d2_o) / den1;
    double t2 = -ds21.Dot(d1_o) / den2;
    if (!l1.ParameterWithinBounds(t1) || !l2.ParameterWithinBounds(t2))
        return Intersection{Intersection::ZeroIntersection, 0., 0.};
    return Intersection{Intersection::OneIntersection, t1, t2};
}