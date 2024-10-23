#include "gui.h"
#include "utils.h"
#include <sstream>
#include <iomanip>

OpticsBox *LuaUI::box_ = nullptr;
Field *LuaUI::field_ = nullptr;

OpticsBox::OpticsBox(int x, int y, int w, int h, const char *label)
    : Fl_Widget(x, y, w, h, label), axis_(Point(x + w / 2, y + h / 2), w / 10.0)
{
    LuaUI::Bind(this, &field_);
    interpreter_.RegisterLuaFunction<LuaUI::AddMirrorFunctor>("add_mirror");
    interpreter_.RegisterLuaFunction<LuaUI::AddLensFunctor>("add_lens");
    interpreter_.RegisterLuaFunction<LuaUI::AddRefractiveFunctor>("add_refractive");
    interpreter_.RegisterLuaFunction<LuaUI::AddLightRayFunctor>("add_lightray");
    RunLuaScript();
    RunSimulation();
    redraw();
}

OpticsBox::~OpticsBox() { LuaUI::UnBind(); }

void MirrorElement::Draw(const Axis &axis) const
{
    Point end = mirror_.seg_.GetEnd();
    Point start = mirror_.seg_.GetStart();
    Point w_end = axis.ToWindowCoord(end);
    Point w_start = axis.ToWindowCoord(start);
    fl_color(FL_BLUE);
    fl_line(w_start.x, w_start.y, w_end.x, w_end.y);
}

void LensElement::Draw(const Axis &axis) const
{
    Point end = lens_.seg_.GetEnd();
    Point start = lens_.seg_.GetStart();
    Point w_end = axis.ToWindowCoord(end);
    Point w_start = axis.ToWindowCoord(start);
    fl_color(FL_BLACK);
    DrawArrow(w_start.x, w_start.y, w_end.x, w_end.y);

    // std::stringstream ss;
    // ss << "f=" << std::setprecision(2) << lens_.focal_length_;
    // fl_color(FL_BLACK);
    // fl_font(FL_COURIER, 8);
    // fl_draw(ss.str().c_str(), w_end.x + 4, w_end.y + 4);
}

void RefractiveElement::Draw(const Axis &axis) const
{
    Point end = refractive_.seg_.GetEnd();
    Point start = refractive_.seg_.GetStart();
    Point w_end = axis.ToWindowCoord(end);
    Point w_start = axis.ToWindowCoord(start);
    fl_color(FL_GREEN);
    fl_line(w_start.x, w_start.y, w_end.x, w_end.y);

    // std::stringstream ss;
    // ss << "nl,nr=" << std::setprecision(2) << refractive_.n_left_ << "," << refractive_.n_right_;
    // fl_color(FL_BLACK);
    // fl_font(FL_COURIER, 8);
    // fl_draw(ss.str().c_str(), w_end.x + 4, w_end.y + 4);
}

void LightRayElement::Draw(const Axis &axis) const
{
    for (auto seg : path_)
    {
        Point end = seg.GetEnd();
        Point start = seg.GetStart();
        Point w_end = axis.ToWindowCoord(end);
        Point w_start = axis.ToWindowCoord(start);
        fl_color(FL_DARK_YELLOW);
        fl_line(w_start.x, w_start.y, w_end.x, w_end.y);
    }
    Point start = ray_.GetStart();
    Point end = start + ray_.GetDirection().Normalize().Scale(20);
    Point w_end = axis.ToWindowCoord(end);
    Point w_start = axis.ToWindowCoord(start);
    fl_color(FL_DARK_YELLOW);
    fl_line(w_start.x, w_start.y, w_end.x, w_end.y);
}

void LuaUI::AddMirrorFunctor::operator()(std::vector<double> ds) const
{
    if (ds.size() != 4 || LuaUI::field_ == nullptr || LuaUI::box_ == nullptr)
        throw LuaExecutionException();
    Mirror mirror(Segment(Point(ds[0], ds[1]), Point(ds[2] - ds[0], ds[3] - ds[1])));
    auto pmirror = std::make_shared<MirrorElement>(mirror);
    LuaUI::field_->AddDeflector(pmirror);
    LuaUI::box_->AddElement(pmirror);
}

void LuaUI::AddLensFunctor::operator()(std::vector<double> ds) const
{
    if (ds.size() != 5 || LuaUI::field_ == nullptr || LuaUI::box_ == nullptr)
        throw LuaExecutionException();
    Lens lens(Segment(Point(ds[0], ds[1]), Point(ds[2] - ds[0], ds[3] - ds[1])), ds[4]);
    auto plens = std::make_shared<LensElement>(lens);
    LuaUI::field_->AddDeflector(plens);
    LuaUI::box_->AddElement(plens);
}

void LuaUI::AddRefractiveFunctor::operator()(std::vector<double> ds) const
{
    if (ds.size() != 6 || LuaUI::field_ == nullptr || LuaUI::box_ == nullptr)
        throw LuaExecutionException();
    RefractiveSurface ref(Segment(Point(ds[0], ds[1]), Point(ds[2] - ds[0], ds[3] - ds[1])), ds[4], ds[5]);
    auto pref = std::make_shared<RefractiveElement>(ref);
    LuaUI::field_->AddDeflector(pref);
    LuaUI::box_->AddElement(pref);
}

void LuaUI::AddLightRayFunctor::operator()(std::vector<double> ds) const
{
    if (ds.size() != 4 || LuaUI::field_ == nullptr || LuaUI::box_ == nullptr)
        throw LuaExecutionException();
    Ray ray(Point(ds[0], ds[1]), Point(ds[2], ds[3]));
    auto pray = std::make_shared<LightRayElement>(ray);
    LuaUI::field_->AddLightRay(pray);
    LuaUI::box_->AddElement(pray);
}

int OpticsBox::handle(int event)
{
    static int last_x = 0, last_y = 0;
    int dy = Fl::event_dy(), curx = Fl::event_x(), cury = Fl::event_y();
    if (event == FL_MOUSEWHEEL)
    {
        if (dy > 0)
            axis_.Scale(0.97, Point(x() + w() / 2, y() + h() / 2));
        else
            axis_.Scale(1.03, Point(x() + w() / 2, y() + h() / 2));
        redraw();
    }
    else if (event == FL_PUSH)
    {
    }
    else if (event == FL_RELEASE)
    {
        last_x = 0;
        last_y = 0;
    }
    else if (event == FL_DRAG)
    {
        if (last_x > 0 && last_y > 0)
            axis_.Move(curx - last_x, cury - last_y);
        last_x = curx;
        last_y = cury;
        redraw();
    }
    else if (event == FL_KEYBOARD)
    {
        if(Fl::event_key() == 'c')
        {
            Clear();
            RunLuaScript();
            RunSimulation();
        }
        else
        {
            return 1;
        }

    }
    return 1;
}