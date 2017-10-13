#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>

int longest_sequence(vector<int> arr) {
	int longest = 0;
	for (int i = 0; i < arr.size();) {
		if (arr.at(i) == 0) {
			i++;
			continue;
		}
		int current = arr.at(i);
		int current_length = 0;
		for (; i < arr.size() && current == arr.at(i); i++) {
			current_length++;
		}
		if (current_length > longest) {
			longest = current_length;
		}
	}
	return longest;
}

int compare_pixels(int middle_pixel, int neighbour_pixel, int threshold) {
	if (neighbour_pixel > (middle_pixel + threshold)) {
		return 1;
	}
	else if (neighbour_pixel < (middle_pixel - threshold)) {
		return -1;
	}
	else {
		return 0;
	}
}

bool high_speed_test(Mat img, vector<int> intensity, int threshold) {
	int middle_pixel = img.at<uchar>(Point(3, 3));
	intensity.push_back(compare_pixels(middle_pixel, img.at<uchar>(Point(3, 0)), threshold)); // 1
	intensity.push_back(compare_pixels(middle_pixel, img.at<uchar>(Point(6, 3)), threshold)); // 5
	intensity.push_back(compare_pixels(middle_pixel, img.at<uchar>(Point(3, 6)), threshold)); // 9
	intensity.push_back(compare_pixels(middle_pixel, img.at<uchar>(Point(0, 3)), threshold)); // 13
	if (std::count(intensity.begin(), intensity.end(), 1) >= 3 || std::count(intensity.begin(), intensity.end(), -1) >= 3) {
		return true;
	}
	else {
		return false;
	}
}

void fill_vector_intensity(Mat img, vector<int>& intensity, int threshold) {
	intensity.clear();
	if (high_speed_test(img, intensity, threshold) == false) {
		return;
	}
	int middle_pixel = img.at<uchar>(Point(3, 3));
	// 16 pixels around the pixel under test
	intensity.push_back(compare_pixels(middle_pixel, img.at<uchar>(Point(2, 0)), threshold));
	intensity.push_back(compare_pixels(middle_pixel, img.at<uchar>(Point(1, 1)), threshold));
	intensity.push_back(compare_pixels(middle_pixel, img.at<uchar>(Point(0, 2)), threshold));
	intensity.push_back(compare_pixels(middle_pixel, img.at<uchar>(Point(0, 3)), threshold));
	intensity.push_back(compare_pixels(middle_pixel, img.at<uchar>(Point(0, 4)), threshold));
	intensity.push_back(compare_pixels(middle_pixel, img.at<uchar>(Point(1, 5)), threshold));
	intensity.push_back(compare_pixels(middle_pixel, img.at<uchar>(Point(2, 6)), threshold));
	intensity.push_back(compare_pixels(middle_pixel, img.at<uchar>(Point(3, 6)), threshold));
	intensity.push_back(compare_pixels(middle_pixel, img.at<uchar>(Point(4, 6)), threshold));
	intensity.push_back(compare_pixels(middle_pixel, img.at<uchar>(Point(5, 5)), threshold));
	intensity.push_back(compare_pixels(middle_pixel, img.at<uchar>(Point(6, 4)), threshold));
	intensity.push_back(compare_pixels(middle_pixel, img.at<uchar>(Point(6, 3)), threshold));
	intensity.push_back(compare_pixels(middle_pixel, img.at<uchar>(Point(6, 2)), threshold));
	intensity.push_back(compare_pixels(middle_pixel, img.at<uchar>(Point(5, 1)), threshold));
	intensity.push_back(compare_pixels(middle_pixel, img.at<uchar>(Point(4, 0)), threshold));
	intensity.push_back(compare_pixels(middle_pixel, img.at<uchar>(Point(3, 0)), threshold));
	// Insert in the end of vector one more copy - simulate a circle
	intensity.insert(intensity.end(), intensity.begin(), intensity.end());
}

void FAST(cv::Mat img, std::vector<Point>& points, int threshold = 0, bool nonmax_suppresion = false) {
	points.clear();
	threshold = std::min(std::max(threshold, 0), 255);
	cv::cvtColor(img, img, CV_BGR2GRAY);
	vector<int> intensity;
	for (int i = 3; i < img.cols - 3; i++) {
		for (int j = 3; j < img.rows - 3; j++) {
			fill_vector_intensity(img(cv::Rect(i - 3, j - 3, 7, 7)).clone(), intensity, threshold);
			if (longest_sequence(intensity) > 12) {
				points.push_back(Point(i, j));
			}
		}
	}
}

void drawPoints(cv::Mat& img, std::vector<Point> points) {
	for (int k = 0; k < points.size(); k++) {
		cv::circle(img, points.at(k), 2, Scalar(255, 0, 0), CV_FILLED);
	}
	imshow("FAST points", img);
	waitKey(30);
	std::cin.get();
}
