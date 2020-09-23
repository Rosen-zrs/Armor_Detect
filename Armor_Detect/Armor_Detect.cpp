#include "Object/include/Armor.h"
#include "Object/include/Light.h"
#include <time.h>

using namespace cv;
using namespace std;

int main()
{
    //导入视频
    VideoCapture capture(0);
    capture.open("/home/rosen/桌面/Rosen/RM/Armor_Detect/原视频.avi");

    if (!capture.isOpened())
    {
        printf("could not find video data file...\n");
        return -1;
    }

    namedWindow("Origin", WINDOW_AUTOSIZE);
    namedWindow("Video", WINDOW_AUTOSIZE);
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
        threshold(src, src, 50, 255, THRESH_BINARY);

        //提取轮廓
        vector<vector<Point>> contours;
        findContours(src, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point());

        //定义外接拟合矩形
        Rect rect;
        //定义动态灯条数组
        p_light = new Light[contours.size()];

        for (size_t i = 0; i < contours.size(); i++)
        {
            //筛除小轮廓(斑点)
            if (contourArea(contours[i]) < 10)
            {
                continue;
            }
            //拟合外接矩形并绘制
            rect = boundingRect(contours[i]);
            rectangle(frame, rect, Scalar(0, 0, 255), 2, 8, 0);

            //配置灯条成员信息
            p_light[i] = Light(rect.width, rect.height);
            p_light[i].set_Area(rect.width * rect.height);
            p_light[i].set_top_point(rect.tl().x + rect.width / 2, rect.tl().y);
            p_light[i].set_botton_point(rect.tl().x + rect.width / 2, rect.br().y);
            p_light[i].set_center(rect.tl().x + rect.width / 2, (rect.tl().y + rect.br().y) / 2);
        }

        if (contours.size() >= 2)
        {
            //绘制连线
            line(frame, p_light[0].get_top_point(), p_light[1].get_botton_point(), Scalar(255, 255, 255), 1, 8, 0);
            line(frame, p_light[1].get_top_point(), p_light[0].get_botton_point(), Scalar(255, 255, 255), 1, 8, 0);

            //标定矩形和中心点
            center = Point((p_light[0].get_center().x + p_light[1].get_center().x) / 2, (p_light[0].get_center().y + p_light[1].get_center().y) / 2);
            circle(frame, center, 5, Scalar(0, 0, 255), -1, 8, 0);

            //求导进行预测
            dx = center.x - pre_center.x;
            dy = center.y - pre_center.y;

            //计算运行时间
            end = static_cast<double>(getTickCount());
            dt = (end - start) / getTickFrequency();

            //计算出预测中心点坐标
            predict_center = Point2f(center.x + factor * dx * dt, center.y + factor * dy * dt);
            circle(frame, predict_center, 5, Scalar(0, 255, 0), -1, 8, 0);
        }

        pre_center = center;

        imshow("Origin", frame);
        imshow("Video", src);
        if (waitKey(30) == 27)
        {
            break;
        }
    }
}
