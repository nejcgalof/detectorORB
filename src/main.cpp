#include <iostream>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <build_panorama.hpp>
#include <FAST.hpp>

using namespace cv;
using namespace std;

int main(int argc, char** argv )
{
	Mat image = imread("./debug/2.jpg");
	vector<Point> points;
	vector<Point> points1;
	if (argc < 2) {
		cout << "HELP/n";
		FAST(image, points1, 50, true);
		std::vector<std::tuple<Mat, std::vector<Point>, int >> infos = FAST_multisized(image, points, 50, true, 4);
		return 0;
	}
	else {
		image = imread(argv[1]);
		FAST(image, points, atoi(argv[2]), atoi(argv[3]));
	}
	drawPoints(image, points);
	imwrite("FAST.jpg", image);
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