#include "utils.h"

void DrawArrow(int x1, int y1, int x2, int y2, int arrow_size, double arrow_angle)
{
    fl_line(x1, y1, x2, y2);

    double angle = atan2(y2 - y1, x2 - x1);

    double x3 = x1 + arrow_size * cos(angle + arrow_angle);
    double y3 = y1 + arrow_size * sin(angle + arrow_angle);
    double x4 = x1 + arrow_size * cos(angle - arrow_angle);
    double y4 = y1 + arrow_size * sin(angle - arrow_angle);

    fl_line(x1, y1, x3, y3);
    fl_line(x1, y1, x4, y4);

    double x5 = x2 - arrow_size * cos(angle - arrow_angle);
    double y5 = y2 - arrow_size * sin(angle - arrow_angle);
    double x6 = x2 - arrow_size * cos(angle + arrow_angle);
    double y6 = y2 - arrow_size * sin(angle + arrow_angle);

    fl_line(x2, y2, x5, y5);
    fl_line(x2, y2, x6, y6);
}

std::string SelectFile()
{
    Fl_File_Chooser file_chooser(".", "*", Fl_File_Chooser::SINGLE, "Select a file.");
    file_chooser.show();
    while (file_chooser.shown())
    {
        Fl::wait();
    }
    if (file_chooser.value() != nullptr)
    {
        return std::string(file_chooser.value());
    }
    return std::string();
}