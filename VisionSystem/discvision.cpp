#include <opencv2/opencv.hpp>
#include <opencv2/geometry/2d.hpp>

#include <iostream>
#include <string>
#include <vector>

int main()
{
    cv::VideoCapture camera(0);
    if (!camera.isOpened())
    {
        std::cerr << "Error: could not open camera 0." << std::endl;
        return 1;
    }

    const std::string windowName = "Circle Detection";
    cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE);

    cv::Mat frame;
    cv::Mat gray;
    cv::Mat img;
    while (true)
    {
        /*
        if (!camera.read(frame))
        {
            std::cerr << "Error: could not read a frame from the camera." << std::endl;
            return 1;
        }
        */

        img = cv::imread("sample_images/sampleboard1.jpg");

        double scale_factor = 0.4;
        int new_w = static_cast<int>(img.cols * scale_factor);
        int new_h = static_cast<int>(img.rows * scale_factor);

        cv::resize(img, frame, cv::Size(new_w, new_h), 0, 0, cv::INTER_AREA);

        if (frame.empty())
        {
            std::cerr << "Error: received an empty camera frame." << std::endl;
            return 1;
        }

        cv::Mat hsv, redMask, blackMask;
        cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);

        cv::Mat mask1, mask2;
        cv::inRange(hsv, cv::Scalar(0, 100, 100), cv::Scalar(10, 255, 255), mask1);
        cv::inRange(hsv, cv::Scalar(170, 100, 100), cv::Scalar(180, 255, 255), mask2);
        redMask = mask1 | mask2;

        // Black discs: dark objects with low brightness (adjust if needed)
        cv::inRange(hsv, cv::Scalar(0, 0, 0), cv::Scalar(180, 255, 110), blackMask);

        cv::Mat kernel = cv::getStructuringElement(
            cv::MORPH_ELLIPSE,
            cv::Size(7, 7)
        );

        cv::morphologyEx(redMask, redMask, cv::MORPH_CLOSE, kernel);
        cv::morphologyEx(blackMask, blackMask, cv::MORPH_CLOSE, kernel);

        auto drawCircularContours = [&](const cv::Mat& mask, const cv::Scalar& color)
        {
            std::vector<std::vector<cv::Point>> contours;
            cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

            for (const auto& contour : contours)
            {
                double area = cv::contourArea(contour);

                if (area < 100 || area > 2000)
                    continue;

                double perimeter = cv::arcLength(contour, true);

                if (perimeter <= 0)
                    continue;

                double circularity =
                    4.0 * CV_PI * area / (perimeter * perimeter);

                if (circularity < 0.65)
                    continue;

                cv::Point2f center;
                float radius;
                cv::minEnclosingCircle(contour, center, radius);

                cv::circle(frame, center, (int)radius, color, 2);
            }
        };

        drawCircularContours(redMask, cv::Scalar(0, 255, 0));
        drawCircularContours(blackMask, cv::Scalar(255, 0, 0));

        cv::imshow("Red Mask", redMask);
        cv::imshow("Black Mask", blackMask);

        cv::imshow(windowName, frame);

        const int key = cv::waitKey(1);
        if (key == 27 || key == 'q' || key == 'Q')
        {
            break;
        }
    }

    camera.release();
    cv::destroyAllWindows();
    return 0;
}
