#include <iostream>
#include <vector>
#include "geometry.h"
#include "luaapi.h"
#include "gui.h"
#include "test.hpp"

int main()
{
    // Test();
    Fl_Window window(1280, 800, "optics simulator");
    OpticsBox optics_box(10, 10, 1260, 780, "optics box");
    window.add(optics_box);
    window.end();
    window.show();
    return Fl::run();
}



