#ifndef  __LIGHT_
#define  __LIGHT_

#include </usr/local/include/opencv4/opencv2/opencv.hpp>

using namespace cv;

class Light{
    public:
        Light();
        Light(double lenght, double width);

        double get_length();
        double get_width();
        
        void set_Area(double area);

        Point2f get_center();                           //获取中心点
        Point2f get_top_point();                    //获取顶点
        Point2f get_botton_point();             //获取底点

        void set_center(float x, float y);                           //获取中心点
        void set_top_point(float x, float y);                    //获取顶点
        void set_botton_point(float x, float y);             //获取底点

    private:
        double length,width;        //定义灯条的长和宽
        double Area;                          //定义灯条的面积

        Point2f center;                     //定义中心点
        Point2f top_point, botton_point;        //定义顶点和底点
};

#endif // ! __LIGHT_
