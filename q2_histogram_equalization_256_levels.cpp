#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

Mat myEqualizeHistNormalized(const Mat& gray)
{
    Mat result = gray.clone();

    int hist[256] = { 0 };

    // Step 1: histogram
    for (int y = 0; y < gray.rows; y++)
        for (int x = 0; x < gray.cols; x++)
            hist[gray.at<uchar>(y, x)]++;

    // Step 2: cumulative histogram (CDF)
    int cdf[256] = { 0 };
    cdf[0] = hist[0];
    for (int i = 1; i < 256; i++)
        cdf[i] = cdf[i - 1] + hist[i];

    // Step 3: find CDF_min = first non-zero CDF
    int cdf_min = 0;
    for (int i = 0; i < 256; i++)
    {
        if (cdf[i] != 0)
        {
            cdf_min = cdf[i];
            break;
        }
    }

    int total = gray.rows * gray.cols;

    // Step 4: remap each pixel using full normalized formula
    for (int y = 0; y < gray.rows; y++)
    {
        for (int x = 0; x < gray.cols; x++)
        {
            int oldVal = gray.at<uchar>(y, x);

            float newVal = ((float)(cdf[oldVal] - cdf_min) / (total - cdf_min)) * 255.0f;

            if (newVal < 0) newVal = 0;
            result.at<uchar>(y, x) = cvRound(newVal);
        }
    }

    return result;
}

Mat drawHistogram(const Mat& gray)
{
    int hist[256] = { 0 };

    for (int y = 0; y < gray.rows; y++)
        for (int x = 0; x < gray.cols; x++)
            hist[gray.at<uchar>(y, x)]++;

    int maxVal = hist[0];
    for (int i = 1; i < 256; i++)
        if (hist[i] > maxVal) maxVal = hist[i];

    int hist_w = 512, hist_h = 400;
    int bin_w = cvRound((double)hist_w / 256);
    Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(255, 255, 255));

    for (int i = 1; i < 256; i++)
    {
        int y1 = hist_h - cvRound(((double)hist[i - 1] / maxVal) * hist_h);
        int y2 = hist_h - cvRound(((double)hist[i] / maxVal) * hist_h);

        line(histImage,
            Point(bin_w * (i - 1), y1),
            Point(bin_w * i, y2),
            Scalar(0, 0, 0), 2);
    }

    return histImage;
}

int main()
{
    Mat src = imread("image1.jpg");
    if (src.empty())
    {
        cout << "Error loading image\n";
        return -1;
    }

    Mat gray;
    cvtColor(src, gray, COLOR_BGR2GRAY);

    Mat opencvEq, manualEq;
    equalizeHist(gray, opencvEq);
    manualEq = myEqualizeHistNormalized(gray);

    imshow("Original Gray", gray);
    imshow("OpenCV Equalized", opencvEq);
    imshow("Manual Equalized", manualEq);

    imshow("Hist Original", drawHistogram(gray));
    imshow("Hist OpenCV", drawHistogram(opencvEq));
    imshow("Hist Manual", drawHistogram(manualEq));

    waitKey(0);
    return 0;
}