#ifndef GUI_H
#define GUI_H

#include "geometry.h"
#include "optics.h"
#include "luaapi.h"
#include "utils.h"
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Browser.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/fl_draw.H>
#include <FL/fl_ask.H>
#include <FL/Fl_File_Chooser.H>

class Axis
{
private:
    Point origin_;
    double scale_;

public:
    Axis(Point origin, double scale) : origin_(origin), scale_(scale)
    {
        if (scale == 0)
            throw ZeroDivisionException();
    }
    Point ToWindowCoord(Point p) const { return origin_ + p.SlipY().Scale(scale_); }
    Point ToFieldCoord(Point p) const { return (p - origin_).Scale(1 / scale_).SlipY(); }
    void Scale(double s, Point center)
    {
        scale_ *= s;
        origin_ = (origin_ - center).Scale(s) + center;
    }
    void Move(double dx, double dy) { origin_ = origin_ + Vec(dx, dy); }
};

class Element
{
public:
    virtual void Draw(const Axis &axis) const = 0;
};

class MirrorElement : public Element, public MirrorDeflector
{
public:
    MirrorElement(const Mirror &mirror) : MirrorDeflector(mirror) {}
    virtual void Draw(const Axis &axis) const override;
};

class LensElement : public Element, public LensDeflector
{
public:
    LensElement(const Lens &lens) : LensDeflector(lens) {}
    virtual void Draw(const Axis &axis) const override;
};

class RefractiveElement : public Element, public RefractiveDeflector
{
public:
    RefractiveElement(const RefractiveSurface &refractive) : RefractiveDeflector(refractive) {}
    virtual void Draw(const Axis &axis) const override;
};

class LightRayElement : public Element, public LightRay
{
public:
    LightRayElement(Ray ray) : LightRay(ray) {}
    virtual void Draw(const Axis &axis) const override;
};

class OpticsBox : public Fl_Widget
{
public:
    OpticsBox(int x, int y, int w, int h, const char *label);
    ~OpticsBox();
    void draw() override
    {
        Fl_Widget::redraw();
        draw_box(FL_BORDER_BOX, FL_LIGHT3);
        fl_push_clip(x(), y(), w(), h());
        for (auto element : elements_)
        {
            element->Draw(axis_);
        }
        fl_pop_clip();
    }
    int handle(int event) override;
    void AddElement(std::shared_ptr<Element> e)
    {
        elements_.push_back(e);
    }
    void RunLuaScript()
    {
        try
        {
            interpreter_.Run(SelectFile());
        }
        catch (LuaExecutionException)
        {
            fl_alert("Please check your Lua script!");
        }
        catch (ZeroDivisionException)
        {
            fl_alert("Please check your Lua script!");
        }
    }
    void RunSimulation()
    {
        try
        {
            field_.Simulation();
        }
        catch (ZeroDivisionException)
        {
            fl_alert("Error occurred during simulation! Please check your Lua script!");
        }
    }
    void Clear()
    {
        elements_.clear();
        field_.Clear();
    }

private:
    std::vector<std::shared_ptr<Element>> elements_;
    LuaInterpreter interpreter_;
    Field field_;
    Axis axis_;
};

class LuaUI
{
public:
    static OpticsBox *box_;
    static Field *field_;
    static void Bind(OpticsBox *box, Field *field)
    {
        LuaUI::box_ = box;
        LuaUI::field_ = field;
    }

    static void UnBind()
    {
        LuaUI::box_ = nullptr;
        LuaUI::field_ = nullptr;
    }
    struct AddMirrorFunctor
    {
        void operator()(std::vector<double> ds) const;
    };
    struct AddLensFunctor
    {
        void operator()(std::vector<double> ds) const;
    };
    struct AddRefractiveFunctor
    {
        void operator()(std::vector<double> ds) const;
    };
    struct AddLightRayFunctor
    {
        void operator()(std::vector<double> ds) const;
    };
};

#endif