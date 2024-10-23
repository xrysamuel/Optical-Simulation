#include "geometry.h"
#include "luaapi.h"
#include "gui.h"

#include <iostream>

void PrintAdd(std::vector<double> ds)
{
    double sum = 0.0;
    for (double d : ds)
    {
        sum += d;
    }
    std::cout << sum << "\n";
}

struct PrintAddFunctor
{
    void operator()(std::vector<double> ds) const
    {
        PrintAdd(std::move(ds));
    }
};

void TestLuaInterface()
{
    std::cout << "==== Test Lua Interface ====\n";
    LuaInterpreter interpreter;
    interpreter.RegisterLuaFunction<PrintAddFunctor>("print_add");

    std::cout << "test.lua Output:\n";
    try
    {
        interpreter.Run("test/test.lua");
    }
    catch (LuaExecutionException &e)
    {
        std::cout << "Lua Execution Exception\n";
    }

    std::cout << "error.lua Output:\n";
    try
    {
        interpreter.Run("test/error.lua");
    }
    catch (LuaExecutionException &e)
    {
        std::cout << "Lua Execution Exception\n";
    }
}

void TestGeometry()
{
    std::cout << "==== Test Geometry ====\n";

    std::vector<Intersection> lrs =
        {
            GetLineIntersection(Line({0, 0}, {2, 1}), Line({3, 3}, {2, -2})),
            GetLineIntersection(Ray({0, 0}, {2, 1}), Segment({3, 3}, {2, -2})),
            GetLineIntersection(Segment({0, 0}, {2, 1}), Segment({3, 3}, {2, -2})),
            GetLineIntersection(Line({0, 0}, {2, 1}), Line({0, 1}, {2, 1})),
            GetLineIntersection(Ray({0, 0}, {2, 1}), Segment({0, 1}, {2, 1})),
        };
    for (auto lr : lrs)
    {
        if (lr.num_intersects == Intersection::ZeroIntersection)
        {
            std::cout << "No Intersection\n";
        }
        else
        {
            std::cout << "The parameter of intersection is " << lr.parameter1 << " and " << lr.parameter2 << "\n";
        }
    }
    try
    {
        GetLineIntersection(Ray({0, 0}, {0, 0}), Segment({0, 1}, {2, 1}));
    }
    catch (ZeroDivisionException &e)
    {
        std::cout << "Zero Direction Exception"
                  << "\n";
    }
}

void Test()
{
    TestGeometry();
    TestLuaInterface();
}