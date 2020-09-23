#ifndef __ENERGY_SWITCH__
#define __ENERGY_SWITCH__

#include <opencv2/opencv.hpp>
#include <math.h>

using namespace cv;

//get the distance between two Points
double get_distance(Point2f p1, Point2f p2);

// define a Energy_Switch Class
class Energy_Switch
{
    public:
        Energy_Switch(Point2f* p_rect);

        // get height and width
        double get_height();
        double get_width();

        //set height and width
        void set_height(double height);
        void set_width(double width);
        void swap_height_width();

        //get area and set area
        double get_area();
        void set_area(double area);

        //define the ballet of ES minAreaRect point
        Point2f srcQuad[4], dstQuad[4];

    private:
        double height, width;
        double area;

};

Mat Affine(Mat src, Energy_Switch es);


#endif // !__ENERGY_SWITCH__