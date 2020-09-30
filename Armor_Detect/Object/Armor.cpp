#include "include/Armor.h"

//获取装甲板的长度函数
float Armor::get_height()
{
    return this->height;
}

//获取装甲板的宽度函数
float Armor::get_width()
{
    return this->width;
}

//获取装甲板灯条高度差值
float Armor::get_H_diff()
{
    return this->H_diff;
}

//获取装甲板灯条角度差值
float Armor::get_Angle_diff()
{
    return this->Angle_diff;
}

//获取装甲板灯条中心点距离差值
float Armor::get_center_dis_diff()
{
    return this->center_dis_diff;
}

Light Armor::get_male_light()
{
    return this->lights[0];
}

Light Armor::get_female_light()
{
    return this->lights[1];
}