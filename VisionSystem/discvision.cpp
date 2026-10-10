#include <opencv2/opencv.hpp>

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
    while (true)
    {
        if (!camera.read(frame))
        {
            std::cerr << "Error: could not read a frame from the camera." << std::endl;
            return 1;
        }

        if (frame.empty())
        {
            std::cerr << "Error: received an empty camera frame." << std::endl;
            return 1;
        }

        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        cv::GaussianBlur(gray, gray, cv::Size(9, 9), 2.0);

        std::vector<cv::Vec3f> circles;
        cv::HoughCircles(
            gray,
            circles,
            cv::HOUGH_GRADIENT,
            1.0,
            gray.rows / 8.0,
            100.0,
            30.0);

        for (const cv::Vec3f& circle : circles)
        {
            const cv::Point center(
                cvRound(circle[0]),
                cvRound(circle[1]));
            const int radius = cvRound(circle[2]);

            cv::circle(frame, center, 3, cv::Scalar(0, 255, 0), cv::FILLED);
            cv::circle(frame, center, radius, cv::Scalar(0, 0, 255), 2);
        }

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
