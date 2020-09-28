#ifndef __LIGHT_
#define __LIGHT_

#include </usr/local/include/opencv4/opencv2/opencv.hpp>

using namespace cv;

class Light
{
public:
    Light();
    Light(RotatedRect rect, float area, float perimeter);

    RotatedRect rect;

    float get_height();
    float get_width();
    float get_angle();
    float get_area();
    float get_perimeter();

    void set_Area(float area);   //设置面积
    void set_angle(float angle); //设置角度

    Point2f get_center();       //获取中心点
    Point2f get_top_point();    //获取顶点
    Point2f get_botton_point(); //获取底点

    void set_center(float x, float y);       //获取中心点
    void set_top_point(float x, float y);    //获取顶点
    void set_botton_point(float x, float y); //获取底点

    //重载！= 符号
    friend bool operator!=(const Light &t1, const Light &t2);

private:
    float height, width; //定义灯条的长和宽
    float area;          //定义灯条的面积
    float perimeter;     //定义灯条的周长
    float angle;         //定义灯条的角度

    Point2f center;                  //定义中心点
    Point2f top_point, botton_point; //定义顶点和底点
};

#endif // ! __LIGHT_
