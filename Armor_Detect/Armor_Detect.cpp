#include "Object/include/Armor.h"
#include "Object/include/Light.h"
#include "Object/include/Match_Condition.h"
#include <iostream>
#include <math.h>
#include <time.h>

#define DEBUG
#define NORMAL
// #define SOLVEPNP
// #define PREDICT
// #define OUTPUT

using namespace cv;
using namespace std;

// void swap(float width, float height);
RotatedRect tranform_rect(RotatedRect &rect);
float distance(Point2f p1, Point2f p2);

int main()
{
    //导入视频
    VideoCapture capture(0);
    capture.open("/home/rosen/桌面/Rosen/RM/Armor_Detect/Video/Armor1.mp4");

    if (!capture.isOpened())
    {
        printf("could not find video data file...\n");
        return -1;
    }

#ifdef DEBUG
    namedWindow("Origin", WINDOW_NORMAL);
    namedWindow("Video", WINDOW_NORMAL);
    resizeWindow("Origin", Size(640, 480));
    resizeWindow("Video", Size(640, 480));
    moveWindow("Origin", 800, 90);
#endif // DEBUG

    //定义Mat变量
    Mat frame, src, dst;
    Mat pre_frame;
    Mat c_frame[3];

    Point2f center;

    //定义灯条指针
    Light *p_light;

    //定义世界坐标和图像坐标
    vector<Point3d> World_Coor = {Point3f(0, 0, 0), Point3f(0, 26.5, 0), Point3f(67.5, 26.5, 0), Point3f(67.5, 0, 0)};

    //读取yml文件
    FileStorage fs2("/home/rosen/桌面/Rosen/RM/Armor_Detect/cam.yml", FileStorage::READ);
    Mat cameraMatrix2, distCoeffs2;
    fs2["camera_matrix"] >> cameraMatrix2;
    fs2["distortion_coefficients"] >> distCoeffs2;

#ifdef PREDICT
    //储存中心点和上一帧中心点信息
    Point2f pre_center, predict_center;
    float dx, dy, Derivative, modulus;

    //计算时间变量
    double start, end, dt;

    //定义相乘系数
    double factor = 500.0;
#endif // PREDICT
    while (capture.read(frame))
    {

#ifdef PREDICT
        //计算程序运行时间
        start = static_cast<double>(getTickCount()); //获取开始执行时间
#endif                                               // PREDICT

        //分离图像RGB通道
        split(frame, c_frame);

        //红蓝通道相减
        src = c_frame[0] - c_frame[2];

        //设定阈值二值化
        threshold(src, src, 60, 255, THRESH_BINARY);

        //膨胀
        Mat element = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
        dilate(src, src, element);

        //提取轮廓
        vector<vector<Point>> contours;
        findContours(src, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point());

        //定义外接拟合矩形
        RotatedRect rect;
        Point2f vertices[4]; //定义矩形的4个顶点

        //定义动态灯条数组
        vector<Light> lights;

        //定义匹配条件结构体
        Match_Condition MATCH_COND;

        bool flag = false;

        for (size_t i = 0; i < contours.size(); i++)
        {
            //筛除小轮廓(斑点)
            if (contourArea(contours[i]) < 80)
            {
                continue;
            }

            if (contours[i].size() > 5)
            {
                //椭圆拟合
                rect = fitEllipse(contours[i]);

                //调整矩形角度和宽高
                tranform_rect(rect);

                float contour_area = contourArea(contours[i]);

                if (rect.size.width / rect.size.height > MATCH_COND.MAX_WH_RATIO || rect.size.height / rect.size.width > 4.5 || contour_area / rect.size.area() < MATCH_COND.MIN_AREA_FULL)
                    continue;

                // //太远的装甲板不选择打击
                // if ( rect.size.height / rect.size.width < 1.8)
                //     continue;

                //配置灯条成员信息
                Light light = Light(rect, contourArea(contours[i]), arcLength(contours[i], true));
                light.rect = rect;
                lights.push_back(light);
            }
        }

        vector<Armor> Matching_Armor;

        Point2f male_light_vertices[4];   //定义公灯条矩形的4个顶点
        Point2f female_light_vertices[4]; //定义母灯条的4个顶点

        //匹配灯条
        for (auto male_light : lights)
        {
            for (auto female_light : lights)
            {
                if (male_light.get_center().x < female_light.get_center().x)
                {
                    //计算灯条特征逐步筛选

                    //面积差值
                    float area_ratio_diff = abs(male_light.get_area() - female_light.get_area());
                    if (area_ratio_diff > male_light.get_area() * MATCH_COND.MAX_AREA_RATIO_DIFF && area_ratio_diff > female_light.get_area() * MATCH_COND.MAX_AREA_RATIO_DIFF)
                    continue;

                    //宽度和高度差值
                    float height_diff = abs(male_light.get_height() - female_light.get_height());
                    if (height_diff > male_light.get_height() * MATCH_COND.MAX_HEIGHT_RATIO_DIFF && height_diff > female_light.get_height() * MATCH_COND.MAX_HEIGHT_RATIO_DIFF)
                    continue;

                    //角度差值
                    float angle_diff = abs(male_light.get_angle() - female_light.get_angle());
                    if (angle_diff > MATCH_COND.MAX_ANGLE_DIFF)
                    continue;

                    //中心距离差值
                    float center_dis_diff = distance(male_light.get_center(), female_light.get_center());
                    float center_y_diff = abs(male_light.get_center().y - female_light.get_center().y);
                    float center_x_diff = abs(male_light.get_center().x - female_light.get_center().x);

                    //根据中心距筛除
                    if (center_dis_diff < 3.5 * male_light.get_height() && center_y_diff < 0.8 * male_light.get_height() && center_x_diff > male_light.get_width())
                    {
                        Matching_Armor.push_back(Armor(male_light, female_light, height_diff, angle_diff, center_dis_diff));
                    }
                }
            }
        }

        for (auto M : Matching_Armor)
        {
            Point2f vertices1[4], vertices2[4]; //定义矩形的4个顶点
            RotatedRect r1 = M.get_male_light().rect;
            RotatedRect r2 = M.get_female_light().rect;

            r1.points(vertices1);
            r2.points(vertices2);

            //绘制矩形
            for (int i = 0; i < 4; i++)
            {
                line(frame, vertices1[i], vertices1[(i + 1) % 4], Scalar(0, 255, 0), 1);
                line(frame, vertices2[i], vertices2[(i + 1) % 4], Scalar(0, 255, 0), 1);
            }
        }

        int min_index = 0;
        float min_dis, max_area;

        if (Matching_Armor.size() >= 1)
        {
            for (int i = 0; i < Matching_Armor.size(); i++)
            {
#ifdef SOLVEPNP
                Mat rvec, tvec, R;

                //传入图像坐标
                vector<Point2d> Img_Coor;
                Img_Coor.push_back(Matching_Armor[i].bl());
                Img_Coor.push_back(Matching_Armor[i].tl());
                Img_Coor.push_back(Matching_Armor[i].tr());
                Img_Coor.push_back(Matching_Armor[i].br());

                //slovepnp姿态解算
                solvePnP(World_Coor, Img_Coor, cameraMatrix2, distCoeffs2, rvec, tvec);
                // Rodrigues(rvec, R);

                //挑选计算出距离最近的装甲板
                double Z = tvec.at<double>(2);

                //改变标志位
                flag = true;

                //记录最近装甲板下标
                if (i == 0)
                {
                    min_dis = Z;
                    min_index = i;
                }
                if (Z < min_dis)
                {
                    min_dis = Z;
                    min_index = i;
                }
#endif // SOLVEPNP

#ifdef NORMAL
                //改变标志位
                flag = true;

                float mean_area = (Matching_Armor[i].get_male_light().get_area() + Matching_Armor[i].get_female_light().get_area()) / 2;

                //记录灯条对最大面积
                if (i == 0)
                {
                    max_area = mean_area;
                }
                else
                {
                    if (mean_area > max_area)
                    {
                        max_area = mean_area;
                    }
                }

#endif // NORMAL

#ifdef OUTPUT
                cout << i << ":   height: " << Matching_Armor[i].get_height() << "    "
                     << "Z:" << Z << "   ";
#endif // OUTPUT
            }

#ifdef NORMAL
            //当面积与最大面积相近时，以中心矩离优先匹配
            if (flag)
            {
                for (int i = 0; i < Matching_Armor.size(); i++)
                {
                    if (i == 0)
                    {
                        min_dis = Matching_Armor[i].get_center_dis_diff();
                    }
                    //计算灯条平均面积
                    float mean_area = (Matching_Armor[i].get_male_light().get_area() + Matching_Armor[i].get_female_light().get_area()) / 2;

                    if (abs(mean_area - max_area) / max_area < 0.1)
                    {
                        //匹配最近灯条对
                        if (Matching_Armor[i].get_center_dis_diff() < min_dis)
                        {
                            min_dis = Matching_Armor[i].get_center_dis_diff();
                            min_index = i;
                        }
                    }
                }
            }
#endif // NORMAL

            if (flag)
            {
                //取出目标装甲板及其灯条
                Light aim_male_light, aim_female_light;
                aim_male_light = Matching_Armor[min_index].get_male_light();
                aim_female_light = Matching_Armor[min_index].get_female_light();

                aim_male_light.rect.points(male_light_vertices);
                aim_female_light.rect.points(female_light_vertices);

                //绘制矩形
                for (int i = 0; i < 4; i++)
                    line(frame, male_light_vertices[i], male_light_vertices[(i + 1) % 4], Scalar(0, 0, 255), 2, 8, 0);
                for (int i = 0; i < 4; i++)
                    line(frame, female_light_vertices[i], female_light_vertices[(i + 1) % 4], Scalar(0, 0, 255), 2, 8, 0);

                //标定目标矩形
                line(frame, aim_male_light.rect.center, aim_female_light.rect.center, Scalar(255, 255, 255), 2, 8, 0);
                center = Point2f((aim_male_light.rect.center.x + aim_female_light.rect.center.x) / 2, (aim_male_light.rect.center.y + aim_female_light.rect.center.y) / 2);
                circle(frame, center, 3, Scalar(0, 0, 255), -1, 8, 0);
            }
        }

#ifdef PREDICT
        //求导进行预测
        dx = center.x - pre_center.x;
        dy = center.y - pre_center.y;

        //计算运行时间
        end = static_cast<double>(getTickCount());
        dt = (end - start) / getTickFrequency();

        //计算出预测中心点坐标
        predict_center = Point2f(center.x + factor * dx * dt, center.y + factor * dy * dt);
        circle(frame, predict_center, 10, Scalar(0, 255, 0), -1, 8, 0);

        pre_center = center;
#endif // PREDICT

#ifdef DEBUG
        imshow("Origin", frame);
        imshow("Video", src);
#endif // DEBUG

        if (waitKey(30) == 27)
        {
            if (waitKey(0) == 27)
            {
                break;
            }
        }
    }
}

//调整矩形角度和宽高
RotatedRect tranform_rect(RotatedRect &rect)
{
    float &width = rect.size.width;
    float &height = rect.size.height;
    float &angle = rect.angle;

    if (angle >= 90.0)
        angle -= 180.0;
    if (angle < -90.0)
        angle += 180.0;

    if (angle >= 45.0)
    {
        swap(width, height);
        angle -= 90.0;
    }
    else if (angle < -45.0)
    {
        swap(width, height);
        angle += 90.0;
    }

    return rect;
}

//计算距离函数
float distance(Point2f p1, Point2f p2)
{
    float x_diff = p1.x - p2.x;
    float y_diff = p1.y - p2.y;
    return sqrt(pow(x_diff, 2) + pow(y_diff, 2));
}
