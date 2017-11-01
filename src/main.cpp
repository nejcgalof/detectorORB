#include <iostream>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <build_panorama.hpp>
#include <FAST.hpp>
#include <BRIEF.hpp>

using namespace cv;
using namespace std;

int main(int argc, char** argv )
{
	Mat image = imread("./panorama_slike/panorama1/DSC_0098.jpg");
	Mat image2 = imread("./panorama_slike/panorama1/DSC_0099.jpg");
	cv::resize(image, image, cv::Size(), 0.25, 0.25);
	cv::resize(image2, image2, cv::Size(), 0.25, 0.25);
	Mat image1_org = image.clone();
	Mat image2_org = image2.clone();
	cv::cvtColor(image, image, CV_BGR2GRAY);
	cv::cvtColor(image2, image2, CV_BGR2GRAY);
	vector<Point> points;
	vector<Point> points1;
	vector<Point> points2;
	vector<vector<int>> features1;
	vector<vector<int>> features2;
	if (argc < 2) {
		cout << "HELP/n";
		FAST(image, points1, 70, true);
		brief(image.clone(), points1, features1);

		FAST(image2, points2, 70, true);
		brief(image2.clone(), points2, features2);
		/*std::vector<std::tuple<Mat, std::vector<Point>, int >> infos = FAST_multisized(image, points, 20, true, 4);
		for (int i = 0; i < 4; i++) {
			drawPoints(std::get<0>(infos[i]), std::get<1>(infos[i]));
			imwrite("FAST"+to_string(i)+".jpg", std::get<0>(infos[i]));
		}*/
		vector<int> matcher;
		matching(features1, features2, matcher);
		draw_matches(image, image2, points1, points2, matcher);
		drawPoints(image1_org, points1);
		imwrite("img1.jpg", image1_org);
		drawPoints(image2_org, points2);
		imwrite("img2.jpg", image2_org);
		return 0;
	}
	else {
		image = imread(argv[1]);
		FAST(image, points, atoi(argv[2]), atoi(argv[3]));
	}
	return 0;
	/*Mat result;
	Mat mask;
	for (int i = 2; i < argc; i++) {
		if(i==2){
			result = imread(argv[i - 1]);
			Mat image = imread(argv[i]);
		}
		Mat image = imread(argv[i]);
		if (!result.data || !image.data)
		{
			cout << "error reading images " << endl;
			return -1;
		}
		//result = panorama(result, image, mask).clone();
		result = panorama_old(result, image).clone();
	}
	imwrite("result.jpg", result);
	return 0;*/
}