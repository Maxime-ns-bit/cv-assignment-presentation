#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>

using namespace cv;
using namespace std;

// Quantize grayscale image to 12 levels: 0..11
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

// Equalization in 12-level domain using full normalized formula
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

    for (int y = 0; y < q12.rows; y++)
    {
        for (int x = 0; x < q12.cols; x++)
        {
            int oldVal = q12.at<uchar>(y, x);

            float s = ((float)(cdf[oldVal] - cdf_min) / (N - cdf_min)) * (L - 1);
            if (s < 0) s = 0;

            result.at<uchar>(y, x) = (uchar)round(s);   // still 0..11
        }
    }

    return result;
}

// Expand 12-level image back to 0..255 for display
Mat expand12To255(const Mat& img12)
{
    Mat display(img12.rows, img12.cols, CV_8UC1);

    for (int y = 0; y < img12.rows; y++)
    {
        for (int x = 0; x < img12.cols; x++)
        {
            int level12 = img12.at<uchar>(y, x);

            // Map 0..11 back to 0..255
            display.at<uchar>(y, x) = (uchar)round(level12 * 255.0 / 11.0);
        }
    }

    return display;
}

// Draw histogram for a normal grayscale image (256 bins)
Mat drawHistogram256(const Mat& gray)
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

// Draw histogram for a 12-level image (12 bins)
Mat drawHistogram12(const Mat& img12, const string& titleText)
{
    int hist[12] = { 0 };

    for (int y = 0; y < img12.rows; y++)
        for (int x = 0; x < img12.cols; x++)
            hist[img12.at<uchar>(y, x)]++;

    int maxVal = hist[0];
    for (int i = 1; i < 12; i++)
        if (hist[i] > maxVal) maxVal = hist[i];

    int hist_w = 600, hist_h = 400;
    int margin = 40;
    int usable_w = hist_w - 2 * margin;
    int bin_w = usable_w / 12;

    Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(255, 255, 255));

    // title
    putText(histImage, titleText, Point(20, 30),
        FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0, 0, 0), 2);

    // axis
    line(histImage, Point(margin, hist_h - margin), Point(hist_w - margin, hist_h - margin), Scalar(0, 0, 0), 1);
    line(histImage, Point(margin, margin), Point(margin, hist_h - margin), Scalar(0, 0, 0), 1);

    // bars + labels
    for (int i = 0; i < 12; i++)
    {
        int x1 = margin + i * bin_w + 5;
        int x2 = margin + (i + 1) * bin_w - 5;
        int bar_h = cvRound(((double)hist[i] / maxVal) * (hist_h - 2 * margin));
        int y1 = hist_h - margin - bar_h;
        int y2 = hist_h - margin;

        rectangle(histImage, Point(x1, y1), Point(x2, y2), Scalar(0, 0, 0), FILLED);

        // label level
        putText(histImage, to_string(i), Point(x1 + 5, hist_h - 10),
            FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 1);

        // label frequency
        putText(histImage, to_string(hist[i]), Point(x1, y1 - 5),
            FONT_HERSHEY_SIMPLEX, 0.4, Scalar(255, 0, 0), 1);
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

    // Convert to grayscale
    Mat gray;
    cvtColor(src, gray, COLOR_BGR2GRAY);

    // Step 1: quantize image to 12 levels
    Mat gray12 = quantizeTo12Levels(gray);

    // Step 2: equalize in 12-level domain
    Mat eq12 = equalize12LevelsNormalized(gray12);

    // Step 3: scale 12-level images to 0..255 for display
    Mat gray12_display = expand12To255(gray12);
    Mat eq12_display = expand12To255(eq12);

    // Histograms
    Mat histOriginal = drawHistogram256(gray);
    Mat histQuant12 = drawHistogram12(gray12, "Histogram of 12-Level Quantized Image");
    Mat histEq12 = drawHistogram12(eq12, "Histogram of Equalized 12-Level Image");

    // Display images
    imshow("Original Gray", gray);
    imshow("Quantized to 12 Levels", gray12_display);
    imshow("Equalized with L=12", eq12_display);

    // Display histograms
    imshow("Hist Original Gray (256 bins)", histOriginal);
    imshow("Hist Quantized 12 Levels", histQuant12);
    imshow("Hist Equalized 12 Levels", histEq12);

    waitKey(0);
    destroyAllWindows();
    return 0;
}
