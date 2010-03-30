
#include "limb.h"
#include "tools.h"
#include "settings.h"

using namespace cv;
using namespace std;


Limb::Limb() {
    center_big = center_small = Point();
    radius_big = radius_small = 0;
    data = false;
};

// scale is ratio between big and small
// We keep both
Limb::Limb(vector<Point> contour_small, float scale, Mat frame) {
    Limb::contour_small = contour_small;
    contour_big = scale_contour(contour_small, 1.0/scale);
    contour_big = inflate_contour(contour_big, INFLATE_SIZE);
    
    Limb::frame = frame;
    data = true;
    minEnclosingCircle(contour_small, center_small, radius_small);
    minEnclosingCircle(contour_big, center_big, radius_big);
    
    Mat mask = Mat(frame.size(), CV_8U, Scalar(0));
    Mat temp = Mat(frame.size(), CV_8U, Scalar(0));
    
    vector<vector<Point> > contours;
    contours.push_back(contour_big);
    drawContours( mask, contours, -1, Scalar(255), CV_FILLED);
    
    frame.copyTo(temp, mask);    
    cutout = temp(boundingRect(contour_big));

    compute_hog();
};


// compute hog of sub
void Limb::compute_hog() {
    
    Mat sized;
    resize(cutout, sized, Size(64,128));
    cvtColor(sized, bw, CV_BGR2GRAY);
    equalizeHist(bw, bw);
    vector<Point> locations;
    Size winStride = Size(8, 8);
    Size padding = Size(0, 0);
    hog = HOGDescriptor();
    hog.compute(bw, hog_descriptors, winStride, padding, locations);
};


Mat Limb::get_limb_image() {
    assert(cutout.data);
    return cutout;
}


bool compare_limbs(const Limb& a, const Limb& b) {
    return a.radius_small > b.radius_small;
}


bool compare_limbs_xpos(const Limb& a, const Limb& b) {
    return a.center_small.x < b.center_small.x;
}