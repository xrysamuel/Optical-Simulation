#ifndef UTILS_H
#define UTILS_H

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_File_Chooser.H>
#include <string>
#include <cmath>

void DrawArrow(int x1, int y1, int x2, int y2, int arrow_size = 10, double arrow_angle = 2 * acos(0.0) / 6);

std::string SelectFile();

#endif