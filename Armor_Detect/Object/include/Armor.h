#ifndef __ARMOR_
#define __ARMOR_

#include "Light.h"
#include "math.h"

class Armor
{
public:
    Armor(Light male_light, Light female_light, float H_diff, float A_diff, float C_diff)
    {
        this->height = (male_light.get_height() + female_light.get_height()) / 2;
        this->width = abs(male_light.get_center().x - female_light.get_center().x) + male_light.get_width();
        this->lights[0] = male_light;
        this->lights[1] = female_light;
        this->H_diff = H_diff;
        this->Angle_diff = A_diff;
        this->center_dis_diff = C_diff;
    }
    float get_height();
    float get_width();
    float get_H_diff();
    float get_Angle_diff();
    float get_center_dis_diff();
    Light get_male_light();
    Light get_female_light();

private:
    float height, width; //定义装甲板的长和宽
    Light lights[2];
    float H_diff;          //灯条高度差
    float Angle_diff;      //灯条角度差
    float center_dis_diff; //灯条中心距离差
};

#endif // ! __ARMOR_
