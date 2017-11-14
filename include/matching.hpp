#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/legacy/legacy.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <vector>
using namespace cv;

int hamming_distance(vector<int> first, vector<int> second) {
	vector<int> result;
	result.clear();
	for (int i = 0; i < 256; i++) {
		if (first.at(i) == second.at(i)) {
			result.push_back(0);
		}
		else {
			result.push_back(1);
		}
	}
	int hamming_dist = 0;
	for (std::vector<int>::iterator it = result.begin(); it != result.end(); ++it) {
		hamming_dist += *it;
	}
	return hamming_dist;
}

void matching(vector<vector<int>> features1, vector<vector<int>> features2, vector<int> &matcher, int threshold) {
	matcher.clear();
	for (int i = 0; i < features1.size(); i++) {
		int smallest_hamming_dist = INT16_MAX;
		int smallest_index = 0;
		for (int j = 0; j < features2.size(); j++) {
			int hamming_dist = hamming_distance(features1.at(i), features2.at(j));
			if (smallest_hamming_dist > hamming_dist) {
				smallest_hamming_dist = hamming_dist;
				smallest_index = j;
			}
		}
		if (smallest_hamming_dist > threshold) {
			matcher.push_back(-1);
		}
		else {
			matcher.push_back(smallest_index);
		}
	}
}

void draw_matches(Mat image, Mat image2, vector<Point> points1, vector<Point> points2, vector<int> matcher) {
	RNG rng(12345);
	Mat newImage;
	hconcat(image, image2, newImage);
	for (int i = 0; i < matcher.size(); i++) {
		if (matcher.at(i) == -1) {
			continue;
		}
		cv::line(newImage, points1.at(i), Point(image.size().width + points2.at(matcher.at(i)).x, points2.at(matcher.at(i)).y), cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)), 2, CV_AA);
	}
	imwrite("matches.jpg", newImage);
}