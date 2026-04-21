#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

int main()
{
    Mat img = imread("image1.jpg", IMREAD_GRAYSCALE);
    if (img.empty()) { cout << "Error loading image\n"; return -1; }

    Mat gx, gy, abs_gx, abs_gy, sobelEdge;
    Mat sobelX = (Mat_<float>(3, 3) << -1, 0, 1, -2, 0, 2, -1, 0, 1);
    Mat sobelY = (Mat_<float>(3, 3) << -1, -2, -1, 0, 0, 0, 1, 2, 1);
    filter2D(img, gx, CV_32F, sobelX);
    filter2D(img, gy, CV_32F, sobelY);
    convertScaleAbs(gx, abs_gx);
    convertScaleAbs(gy, abs_gy);
    addWeighted(abs_gx, 0.5, abs_gy, 0.5, 0, sobelEdge);

    Mat px, py, abs_px, abs_py, prewittEdge;
    Mat prewittX = (Mat_<float>(3, 3) << -1, 0, 1, -1, 0, 1, -1, 0, 1);
    Mat prewittY = (Mat_<float>(3, 3) << -1, -1, -1, 0, 0, 0, 1, 1, 1);
    filter2D(img, px, CV_32F, prewittX);
    filter2D(img, py, CV_32F, prewittY);
    convertScaleAbs(px, abs_px);
    convertScaleAbs(py, abs_py);
    addWeighted(abs_px, 0.5, abs_py, 0.5, 0, prewittEdge);

    imshow("Original", img);
    imshow("Sobel via filter2D", sobelEdge);
    imshow("Prewitt", prewittEdge);
    waitKey(0);
    return 0;
}
