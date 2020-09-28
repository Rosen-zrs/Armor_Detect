#include "include/Light.h"

//灯条构造函数
Light::Light() {}

Light::Light(RotatedRect rect, float area, float perimeter)
{
    this->width = rect.size.width;
    this->height = rect.size.height;
    this->angle = rect.angle;
    this->center = rect.center;
    this->top_point = Point2f(rect.center.x + this->width * cos(this->angle), rect.center.y + this->height * sin(this->angle));
    this->botton_point = Point2f(rect.center.x - this->width * cos(this->angle), rect.center.y - this->height * sin(this->angle));
    this->area = area;
    this->perimeter = perimeter;
}

//获取灯条长宽
float Light::get_height()
{
    return this->height;
}

float Light::get_width()
{
    return this->width;
}

float Light::get_angle()
{
    return this->angle;
}

float Light::get_area()
{
    return this->area;
}

float Light::get_perimeter()
{
    return this->perimeter;
}

//设置灯条的面积
void Light::set_Area(float area)
{
    this->area = area;
}

void Light::set_angle(float angle)
{
    this->angle = angle;
}

//获取中心点
Point2f Light::get_center()
{
    return this->center;
}

//获取顶点
Point2f Light::get_top_point()
{
    return this->top_point;
}

//获取底点
Point2f Light::get_botton_point()
{
    return this->botton_point;
}

//获取中心点
void Light::set_center(float x, float y)
{
    this->center.x = x;
    this->center.y = y;
}

//获取顶点
void Light::set_top_point(float x, float y)
{
    this->top_point.x = x;
    this->top_point.y = y;
}

//获取底点
void Light::set_botton_point(float x, float y)
{
    this->botton_point.x = x;
    this->botton_point.y = y;
}

bool operator!=(const Light &t1, const Light &t2)
{
    if (t1.width != t2.width && t1.height != t2.height && t1.angle != t2.angle)
        return true;
    else
        return false;
}