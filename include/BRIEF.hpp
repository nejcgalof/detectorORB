#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include <random>
#include <math.h>

void generate_pairs(vector<std::pair<cv::Point, cv::Point>> &pairs) {
	std::random_device rd;
	std::mt19937 generator(rd());
	double std = ((1.0 / 5.0) * 31);
	std::normal_distribution<double> distribution(0, std);
	for (int i = 0; i < 256; i++) {
		int x1 = 15+distribution(generator);
		int y1 = 15+distribution(generator);
		int x2 = 15+distribution(generator);
		int y2 = 15+distribution(generator);
		pairs.push_back(std::make_pair(cv::Point(x1, y1), cv::Point(x2, y2)));
	}
	/*Mat image(31, 31, CV_8UC3, Scalar(0, 0, 0));
	for (int i = 0; i < 256; i++) {
		cv::line(image, pairs.at(i).first, pairs.at(i).second, cv::Scalar(255,255,255));		
	}
	imwrite("pairs.jpg", image);*/
}

void brief(cv::Mat image, vector<Point> points) {
	copyMakeBorder(image, image, 15, 15, 15, 15, cv::BORDER_REFLECT101);
	GaussianBlur(image, image, cv::Size(5, 5), 0);
	vector<std::pair<cv::Point,cv::Point>> pairs;
	generate_pairs(pairs);
	for (int k = 0; k < points.size(); k++) {
		cv::Rect roi(points.at(k).x, points.at(k).y, 31, 31);
		cv::Mat image_roi = image(roi);
	}
}