#include "include/Light.h"

//灯条构造函数
Light::Light(){}

Light::Light(double lenght, double width){
    this->length = length;
    this->width = width;
}

//获取灯条长宽
double Light::get_length(){
    return this->length;
}
double Light::get_width(){
    return this->width;
}

//设置灯条的面积
void Light::set_Area(double area){
    this->Area = area;
}

//获取中心点
Point2f Light::get_center(){
    return this->center;
}                         

//获取顶点
Point2f Light::get_top_point(){
    return this->top_point;
}                  

//获取底点
Point2f Light::get_botton_point(){
    return this->botton_point;
}        

 //获取中心点
void Light::set_center(float x, float y){
    this->center.x = x;
    this->center.y = y;
}                     

//获取顶点
void Light::set_top_point(float x, float y){
    this->top_point.x = x;
    this->top_point.y = y;
}                

//获取底点
void Light::set_botton_point(float x, float y){
    this->botton_point.x = x;
    this->botton_point.y = y;
}           

