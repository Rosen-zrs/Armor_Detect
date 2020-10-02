#ifndef __ARMOR_
#define __ARMOR_

#include "Light.h"
#include </usr/local/include/opencv4/opencv2/opencv.hpp>
#include "math.h"

class Armor
{
public:
    Armor(Light male_light, Light female_light, float H_diff, float A_diff, float C_diff)
    {
        Point2f male_light_vertices[4];   //定义公灯条矩形的4个顶点
        Point2f female_light_vertices[4]; //定义母灯条的4个顶点
        male_light.rect.points(male_light_vertices);
        female_light.rect.points(female_light_vertices);

        this->height = (male_light.get_height() + female_light.get_height()) / 2;
        this->width = abs(male_light.get_center().x - female_light.get_center().x) + male_light.get_width();
        this->lights[0] = male_light;
        this->lights[1] = female_light;
        this->H_diff = H_diff;
        this->Angle_diff = A_diff;
        this->center_dis_diff = C_diff;
        this->p_bl = male_light_vertices[0];
        this->p_tl = male_light_vertices[1];
        this->p_tr = female_light_vertices[2];
        this->p_br = female_light_vertices[3];
    }

    float get_height();
    float get_width();
    float get_H_diff();
    float get_Angle_diff();
    float get_center_dis_diff();
    Light get_male_light();
    Light get_female_light();

    Point2f bl();
    Point2f tl();
    Point2f tr();
    Point2f br();


private:
    float height, width; //定义装甲板的长和宽
    Light lights[2];
    float H_diff;          //灯条高度差
    float Angle_diff;      //灯条角度差
    float center_dis_diff; //灯条中心距离差
    Point2f p_tl, p_tr, p_bl, p_br;//装甲板矩形角点
};

#endif // ! __ARMOR_
