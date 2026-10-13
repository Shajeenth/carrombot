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

        std::vector<cv::Vec3f> circles;

        /*
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        cv::GaussianBlur(gray, gray, cv::Size(9, 9), 2.0);

        cv::HoughCircles(
            gray,
            circles,
            cv::HOUGH_GRADIENT,
            1.0,
            8.0,
            50.0,
            25.0,
            10.0,
            25.0
        );
        */

        cv::Mat hsv, mask;
        cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);

        // Example: Threshold for red pieces (Red wraps around the HSV hue boundary 0/180)
        cv::Mat mask1, mask2;
        cv::inRange(hsv, cv::Scalar(0, 100, 100), cv::Scalar(10, 255, 255), mask1);
        cv::inRange(hsv, cv::Scalar(170, 100, 100), cv::Scalar(180, 255, 255), mask2);
        mask = mask1 | mask2;

        // Clean up noise with morphological operations
        cv::erode(mask, mask, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3)));
        cv::dilate(mask, mask, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));

        // Find contours of the coins
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        for (const auto& contour : contours) {
            double area = cv::contourArea(contour);
            if (area > 100 && area < 2000) { // Filter by expected coin area size
                cv::Point2f center;
                float radius;
                cv::minEnclosingCircle(contour, center, radius);
                
                // Draw valid detected coins
                cv::circle(frame, center, (int)radius, cv::Scalar(0, 255, 0), 2);
            }
        }
        
        /*

        for (const cv::Vec3f& circle : circles)
        {
            const cv::Point center(
                cvRound(circle[0]),
                cvRound(circle[1]));
            const int radius = cvRound(circle[2]);

            cv::circle(frame, center, 3, cv::Scalar(0, 255, 0), cv::FILLED);
            cv::circle(frame, center, radius, cv::Scalar(0, 0, 255), 2);
        }
        */

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
