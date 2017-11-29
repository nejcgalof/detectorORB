#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include <random>
#include <bitset>
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
		if (x1 < 31 && x1>=0 && x2 < 31 && x2 >= 0 && y1 < 31 && y1 >= 0 && y2 < 31 && y2 >= 0) {
			pairs.push_back(std::make_pair(cv::Point(x1, y1), cv::Point(x2, y2)));
		}
		else {
			i--;
		}
	}
}	

vector<int> create_binary_vector(cv::Mat image_roi, vector<std::pair<cv::Point, cv::Point>> pairs) {
	vector<int> binary_vector;
	binary_vector.clear();
	for (int i = 0; i < pairs.size(); i++) {
		if (static_cast<int>(image_roi.at<uchar>(pairs.at(i).first)) > static_cast<int>(image_roi.at<uchar>(pairs.at(i).second))) {
			binary_vector.push_back(1);
		}
		else {
			binary_vector.push_back(0);
		}
	}
	return binary_vector;
}

void brief(cv::Mat image, vector<Point> &points, vector<vector<int>> &features, vector<std::pair<cv::Point, cv::Point>> pairs) {
	copyMakeBorder(image, image, 15, 15, 15, 15, cv::BORDER_REFLECT101);
	GaussianBlur(image, image, cv::Size(5, 5), 0);
	for (int k = 0; k < points.size(); k++) {
		if (points.at(k).x < 0 || points.at(k).y < 0 || image.cols < points.at(k).x || image.rows < points.at(k).y) {
			std::cout << "maybi kresne zdle" << std::endl;
			points.erase(points.begin() + k);
			k--;
			continue;
		}
		else {
			cv::Rect roi(points.at(k).x, points.at(k).y, 31, 31);
			cv::Mat image_roi = image(roi);
			features.push_back(create_binary_vector(image_roi, pairs));
		}
	}
}