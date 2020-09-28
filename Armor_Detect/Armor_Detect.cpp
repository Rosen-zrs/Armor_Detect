#include "Object/include/Armor.h"
#include "Object/include/Light.h"
#include "Object/include/Match_Condition.h"
#include <time.h>

using namespace cv;
using namespace std;

void swap(float width, float height);
RotatedRect tranform_rect(RotatedRect &rect);
float distance(Point2f p1, Point2f p2);

int main()
{
    //导入视频
    VideoCapture capture(0);
    capture.open("/home/rosen/桌面/Rosen/RM/Armor_Detect/Armor1.mp4");

    if (!capture.isOpened())
    {
        printf("could not find video data file...\n");
        return -1;
    }

    namedWindow("Origin", WINDOW_NORMAL);
    namedWindow("Video", WINDOW_NORMAL);
    resizeWindow("Origin", Size(640, 480));
    resizeWindow("Video", Size(640, 480));
    moveWindow("Origin", 800, 90);

    //定义Mat变量
    Mat frame, src, dst;
    Mat pre_frame;
    Mat c_frame[3];

    //定义灯条指针
    Light *p_light;

    //储存中心点和上一帧中心点信息
    Point2f center, pre_center, predict_center;
    float dx, dy, Derivative, modulus;

    //计算时间变量
    double start, end, dt;

    //定义相乘系数
    double factor = 5000.0;
    while (capture.read(frame))
    {
        //计算程序运行时间
        start = static_cast<double>(getTickCount()); //获取开始执行时间

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

        //Define Matching Condition struct
        Match_Condition MATCH_COND;

        for (size_t i = 0; i < contours.size(); i++)
        {
            //筛除小轮廓(斑点)
            if (contourArea(contours[i]) < 10)
            {
                continue;
            }

            //imitate ellipse of the contour and make minArearect
            if (contours[i].size() > 5)
            {
                rect = fitEllipse(contours[i]);
                tranform_rect(rect);
                rect.points(vertices);

                float contour_area = contourArea(contours[i]);

                if(rect.size.width / rect.size.height > MATCH_COND.MAX_WH_RATIO || contour_area/rect.size.area() < MATCH_COND.MIN_AREA_FULL) continue;

                //draw rectangle
                for (int i = 0; i < 4; i++)
                    line(frame, vertices[i], vertices[(i + 1) % 4], Scalar(0, 255, 0), 1);

                //配置灯条成员信息
                Light light = Light(rect, contourArea(contours[i]), arcLength(contours[i], true));
                light.rect = rect;
                lights.push_back(light);
            }
        }


        Light aim_light[2];
        bool flag = false;

        Point2f male_light_vertices[4];   //定义公灯条矩形的4个顶点
        Point2f female_light_vertices[4]; //定义母灯条的4个顶点

        float min_center_dis = 0;
        float max_closest_area = 0;

        //Matching the light
        for (auto male_light : lights)
        {
            aim_light[0] = male_light;
            for (auto female_light : lights)
            {
                if (female_light != male_light)
                {
                    //calc the matching character
                    float area_ratio_diff = abs((male_light.get_area() / female_light.get_area()) - 1);
                    float perimeter_ratio_diff = abs((male_light.get_perimeter() / female_light.get_perimeter()) - 1);
                    float width_ratio_diff = abs((male_light.get_width() / female_light.get_width()) - 1);
                    float height_ratio_diff = abs((male_light.get_height() / female_light.get_height()) - 1);
                    float angle_ratio_diff = abs(male_light.get_angle() - female_light.get_angle());
                    float center_dis_diff = distance(male_light.get_center(), female_light.get_center());
                    float center_y_diff = abs(male_light.get_center().y - female_light.get_center().y);

                    // cout << male_light.get_area() << ',' << female_light.get_area() << ',' << area_ratio_diff << ',' << perimeter_ratio_diff << ',' << width_ratio_diff << ',' << 
                    // height_ratio_diff << ',' << angle_ratio_diff << ',' << center_dis_diff<<endl;
                    // Matching male_light and female_light

                    if (area_ratio_diff < MATCH_COND.MAX_AREA_RATIO_DIFF && perimeter_ratio_diff < MATCH_COND.MAX_PERIMRTER_RATIO_DIFF  && height_ratio_diff < MATCH_COND.MAX_HEIGHT_RATIO_DIFF)
                    {
                        if (angle_ratio_diff < MATCH_COND.MAX_ANGLE_DIFF && center_dis_diff < 2.5 * male_light.get_height() && center_y_diff < 0.8 * male_light.get_height())
                        {
                            //choose the closest matchling light
                            if (min_center_dis == 0 || center_dis_diff < min_center_dis)
                            {
                                if (max_closest_area == 0 || (female_light.get_area() + male_light.get_area()) / 2 > max_closest_area)
                                {
                                    //get the aim matching lights
                                    max_closest_area = (female_light.get_area() + male_light.get_area()) / 2;
                                    min_center_dis = center_dis_diff;

                                    aim_light[1] = female_light;

                                    aim_light[0].rect.points(male_light_vertices);
                                    aim_light[1].rect.points(female_light_vertices);

                                    flag = true;
                                }
                            }
                        }
                    }
                }
            }
            if (flag)
            {
                line(frame, aim_light[0].rect.center, aim_light[1].rect.center, Scalar(255, 255, 255), 5, 8, 0);
                Point aim;
                aim = Point2f((aim_light[0].rect.center.x + aim_light[1].rect.center.x) / 2, (aim_light[0].rect.center.y + aim_light[1].rect.center.y) / 2);
                circle(frame, aim, 7, Scalar(0, 0, 255), -1, 8, 0);
                //draw rectangle
                for (int i = 0; i < 4; i++)
                    line(frame, male_light_vertices[i], male_light_vertices[(i + 1) % 4], Scalar(0, 0, 255), 2, 8, 0);
                //draw rectangle
                for (int i = 0; i < 4; i++)
                    line(frame, female_light_vertices[i], female_light_vertices[(i + 1) % 4], Scalar(0, 0, 255), 2, 8, 0);
                break;
            }
        }

        // //绘制连线
        // line(frame, p_light[0].get_top_point(), p_light[1].get_botton_point(), Scalar(255, 255, 255), 1, 8, 0);
        // line(frame, p_light[1].get_top_point(), p_light[0].get_botton_point(), Scalar(255, 255, 255), 1, 8, 0);

        // //标定矩形和中心点
        // center = Point((p_light[0].get_center().x + p_light[1].get_center().x) / 2, (p_light[0].get_center().y + p_light[1].get_center().y) / 2);
        // circle(frame, center, 5, Scalar(0, 0, 255), -1, 8, 0);

        // //求导进行预测
        // dx = center.x - pre_center.x;
        // dy = center.y - pre_center.y;

        // //计算运行时间
        // end = static_cast<double>(getTickCount());
        // dt = (end - start) / getTickFrequency();

        // //计算出预测中心点坐标
        // predict_center = Point2f(center.x + factor * dx * dt, center.y + factor * dy * dt);
        // circle(frame, predict_center, 5, Scalar(0, 255, 0), -1, 8, 0);

        // pre_center = center;

        imshow("Origin", frame);
        imshow("Video", src);

        if (waitKey(30) == 27)
        {
            if (waitKey(0) == 27)
            {
                break;
            }
        }
    }
}

//adjust the angle of the rect
RotatedRect tranform_rect(RotatedRect &rect)
{
    float &width = rect.size.width;
    float &height = rect.size.height;
    float &angle = rect.angle;

    //according to width and height change the direction

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

//swap the width and height of the rectangle
void swap(float width, float height)
{
    float temp = width;
    width = height;
    height = temp;
}

//calc the distance between two points
float distance(Point2f p1, Point2f p2)
{
    float x_diff = p1.x - p2.x;
    float y_diff = p1.y - p2.y;
    return sqrt(pow(x_diff, 2) + pow(y_diff, 2));
}
