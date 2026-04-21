#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

Mat quantizeTo12Levels(const Mat& gray)
{
    Mat q(gray.rows, gray.cols, CV_8UC1);

    for (int y = 0; y < gray.rows; y++)
    {
        for (int x = 0; x < gray.cols; x++)
        {
            int oldVal = gray.at<uchar>(y, x);

            // Map 0..255 to 0..11
            int level12 = (oldVal * 12) / 256;
            if (level12 > 11) level12 = 11;

            q.at<uchar>(y, x) = (uchar)level12;
        }
    }

    return q;
}

Mat equalize12LevelsNormalized(const Mat& q12)
{
    Mat result = q12.clone();

    int hist[12] = { 0 };

    // Histogram for levels 0..11
    for (int y = 0; y < q12.rows; y++)
        for (int x = 0; x < q12.cols; x++)
            hist[q12.at<uchar>(y, x)]++;

    int cdf[12] = { 0 };
    cdf[0] = hist[0];
    for (int i = 1; i < 12; i++)
        cdf[i] = cdf[i - 1] + hist[i];

    int cdf_min = 0;
    for (int i = 0; i < 12; i++)
    {
        if (cdf[i] != 0)
        {
            cdf_min = cdf[i];
            break;
        }
    }

    int N = q12.rows * q12.cols;
    int L = 12;

    // Equalization in 12-level domain
    for (int y = 0; y < q12.rows; y++)
    {
        for (int x = 0; x < q12.cols; x++)
        {
            int oldVal = q12.at<uchar>(y, x);

            float s = ((float)(cdf[oldVal] - cdf_min) / (N - cdf_min)) * (L - 1);
            if (s < 0) s = 0;

            result.at<uchar>(y, x) = (uchar)round(s);  // still 0..11
        }
    }

    return result;
}

Mat expand12To255(const Mat& img12)
{
    Mat display(img12.rows, img12.cols, CV_8UC1);

    for (int y = 0; y < img12.rows; y++)
    {
        for (int x = 0; x < img12.cols; x++)
        {
            int level12 = img12.at<uchar>(y, x);

            // Map 0..11 back to 0..255 for display
            display.at<uchar>(y, x) = (uchar)round(level12 * 255.0 / 11.0);
        }
    }

    return display;
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

    // Step 1: quantize image to 12 levels
    Mat gray12 = quantizeTo12Levels(gray);

    // Step 2: equalize in 12-level domain
    Mat eq12 = equalize12LevelsNormalized(gray12);

    // Step 3: scale both to 0..255 for display
    Mat gray12_display = expand12To255(gray12);
    Mat eq12_display = expand12To255(eq12);

    imshow("Original Gray", gray);
    imshow("Quantized to 12 Levels", gray12_display);
    imshow("Equalized with L=12", eq12_display);

    waitKey(0);
    return 0;
}