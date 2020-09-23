#include "Class/include/energy_switch.h"
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char **argv)
{
    //define images
    Mat frame, src, bin_src, Temp1, Temp2, dst, result, img;
    Mat c_frame[3];
    double max1, max2;

    //load Template from image
    Temp1 = imread("/home/rosen/桌面/Rosen/RM/ES_Detect/p1.jpg", 0);
    Temp2 = imread("/home/rosen/桌面/Rosen/RM/ES_Detect/p2.jpg", 0);

    //opencv vedio
    VideoCapture capture(0);
    capture.open("/home/rosen/下载/Task/Video1.avi");
    if (!capture.isOpened())
    {
        printf("could not find video data file...\n");
        return -1;
    }

    //create windows
    namedWindow("Origin", WINDOW_AUTOSIZE);
    namedWindow("Video", WINDOW_AUTOSIZE);
    moveWindow("Origin", 800, 90);

    while (capture.read(frame))
    {

        //split the image RGB channels
        split(frame, c_frame);

        //sub channel ; blue - red
        src = c_frame[0] - c_frame[2];

        GaussianBlur(src, src, Size(5, 5), 0);

        //二值化
        threshold(src, bin_src, 50, 255, THRESH_BINARY);

        //定义结构元素，进行闭操作
        Mat element = getStructuringElement(MORPH_RECT, Size(5, 5), Point(-1, -1));
        dilate(bin_src, bin_src, element);
        morphologyEx(bin_src, bin_src, MORPH_CLOSE, element);

        // 查找轮廓
        vector<vector<Point>> contours;
        vector<Vec4i> hierarchy;
        findContours(bin_src, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point());

        // 绘制轮廓
        Point2f vertices[4]; //定义矩形的4个顶点
        Point2f goal_rect[4];
        Moments mu;
        Point2f mc;

        for (size_t i = 0; i < contours.size(); i++)
        {
            if (contourArea(contours[i]) > 800)
            {
                RotatedRect rect = minAreaRect(contours[i]);
                rect.points(vertices); //计算矩形的4个顶点

                Energy_Switch es(vertices);
                dst = Affine(bin_src, es);

                //matchTemplate
                matchTemplate(dst, Temp1, result, TM_CCOEFF_NORMED, Mat()); //模板匹配
                minMaxLoc(result, 0, &max1, 0, 0, Mat());                   //定位最匹配的位置
                matchTemplate(dst, Temp2, result, TM_CCOEFF_NORMED, Mat()); //模板匹配
                minMaxLoc(result, 0, &max2, 0, 0, Mat());                   //定位最匹配的位置

                if (max1 >= 0.65 || max2 >= 0.66)
                {
                    if (hierarchy[i][2] != -1)
                    {
                        //compute the center of the sub contour 
                        mu = moments(contours[hierarchy[i][2]], false);
                        mc = Point2f(mu.m10 / mu.m00, mu.m01 / mu.m00);

                        //draw the point and circle of the aim
                        circle(frame, mc, 3, Scalar(255, 255, 255), -1, 8, 0);
                        circle(frame, mc, 27, Scalar(255, 255, 255), 1, 8, 0);
                    }

                    // // draw retangle of the aim energy_switch
                    // for (int i = 0; i < 4; i++)
                    // line(frame, vertices[i], vertices[(i + 1) % 4], Scalar(0, 255, 0), 1);
                }
            }
        }

        //显示图像
        imshow("Origin", frame);
        imshow("Video", dst);

        if (waitKey(30) == 27)
        {
            waitKey(0);
            break;
        }
    }
    capture.release();

    return 0;
}

//make warpaffine through the origin src and Energy_Switch
Mat Affine(Mat src, Energy_Switch es)
{
    Mat dst, output;
    Mat warp_mat = getAffineTransform(es.srcQuad, es.dstQuad);
    warpAffine(src, dst, warp_mat, src.size());
    output = dst(Rect(0, 0, 120, 60));
    return output;
}