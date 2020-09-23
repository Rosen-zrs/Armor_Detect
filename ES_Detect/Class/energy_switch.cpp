#include "include/energy_switch.h"

//get the distance between two Points
double get_distance(Point2f p1, Point2f p2)
{
    return sqrt(pow((p1.x - p2.x), 2) + pow((p1.y - p2.y), 2));
}

//constration
Energy_Switch::Energy_Switch(Point2f *p_rect)
{
    this->height = get_distance(p_rect[0], p_rect[1]);
    this->width = get_distance(p_rect[1], p_rect[2]);
    if (this->height > this->width)
    {
        swap_height_width();
        this->srcQuad[0] = p_rect[0];
        this->srcQuad[1] = p_rect[1];
        this->srcQuad[2] = p_rect[2];
        this->srcQuad[3] = p_rect[3];
    }
    else
    {
        this->srcQuad[0] = p_rect[1];
        this->srcQuad[1] = p_rect[2];
        this->srcQuad[2] = p_rect[3];
        this->srcQuad[3] = p_rect[0];
    }
    dstQuad[0] = Point2f(0, 0);
    dstQuad[1] = Point2f(120, 0);
    dstQuad[2] = Point2f(120, 60);
    dstQuad[3] = Point2f(0, 60);
}

// get height and width
double Energy_Switch::get_height()
{
    return this->height;
}

double Energy_Switch::get_width()
{
    return this->width;
}

//set height and width
void Energy_Switch::set_height(double height)
{
    this->height = height;
}

void Energy_Switch::set_width(double width)
{
    this->width = width;
}

void Energy_Switch::swap_height_width()
{
    double n = this->height;
    this->height = this->width;
    this->width = n;
}

//get area and set area
double Energy_Switch::get_area()
{
    return this->area;
}

void Energy_Switch::set_area(double area)
{
    this->area = area;
}

