#include <iostream>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <build_panorama.hpp>

using namespace cv;
using namespace std;

int main(int argc, char** argv )
{
	Mat result;
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
	return 0;
}