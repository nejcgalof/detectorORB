#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <build_panorama.hpp>
#include <FAST.hpp>
#include <BRIEF.hpp>
#include <matching.hpp>

using namespace cv;
using namespace std;

static void show_usage(std::string name)
{
	std::cerr << "Parameters in that order:\n- Fast_threshold\n- nonmax_suppresion(0|1)\n- middle_image_path\n- second_image_path\n- third_image_path\netc..." << std::endl;
}
int main(int argc, char** argv )
{
	bool camera = false;
	if (argc == 3) {
		camera = true;
		cout << "CAMERA" << endl;
	}
	else if (argc < 4) {
		show_usage(argv[0]);
		return 1;
	}
	cv::Mat panorama_image;
	vector<Point> points1;
	vector<Point> points2;
	vector<vector<int>> features1;
	vector<vector<int>> features2;
	vector<int> matcher;
	cv::Mat mask_pano;
	Mat image1_org;
	Mat image2_org;
	Mat image;
	Mat image2;
	RemoveDirectory("./process");
	RemoveDirectory("./panoramas");
	CreateDirectory("./panoramas",NULL);
	CreateDirectory("./process", NULL);
	std::cout << "Generate pairs" << endl;
	vector<std::pair<cv::Point, cv::Point>> pairs;
	int fast_threshold = atoi(argv[1]);
	bool non_max = true;
	if (atoi(argv[2]) == 0) {
		non_max = false;
	}
	generate_pairs(pairs);
	VideoCapture cap;
	Mat frame;
	cap.open(0);
	if (!cap.isOpened()) {
		cerr << "ERROR! Unable to open camera\n";
		return -1;
	}
	if (camera) {
		int i = 2;
		cout << "n - next image; q - exit" << endl;
		namedWindow("panorama", WINDOW_AUTOSIZE);
		for (;;)
		{
			i++;
			cout << "_______________" << endl;
			if (panorama_image.empty()) {
				CreateDirectory(("./process/" + to_string(i - 1)).c_str(), NULL);
				cout << "1 and 2 image" << endl;
				cout << "'n'- take a picture" << endl;
				auto k = waitKey();
				if ('n' == k)
				{
					cout << "taking a picture" << endl;
					cap.read(image);
				}
				else { 
					cout << "End" << endl;
					return 1; 
				}
				cout << "'n'- take a picture" << endl;
				auto g = waitKey();
				if (g == 'n')
				{
					cout << "Taking a picture" << endl;
					cap.read(image2);
				}
				else { 
					cout << "End" << endl;
					return 1;
				}
				cout << "FAST for first image" << endl;
				FAST(image.clone(), points1, fast_threshold, non_max);
			}
			else {
				CreateDirectory(("./process/" + to_string(i - 2)).c_str(), NULL);
				cout << "Add " << (i - 2) << " image" << endl;
				image = panorama_image.clone();
				cout << "'n'- take a picture" << endl;
				auto g = waitKey();
				if (g == 'n')
				{
					cout << "Taking a picture" << endl;
					cap.read(image2);
				}
				else { return 1; }

				//points1.clear();
				features1.clear();
				points2.clear();
				features2.clear();
			}
			image1_org = image.clone();
			image2_org = image2.clone();
			cv::cvtColor(image2, image2, CV_BGR2GRAY);
			cv::cvtColor(image, image, CV_BGR2GRAY);

			for (int k = 0; k < points1.size(); k++) {
				if (points1.at(k).x < 0 || points1.at(k).y < 0 || image.cols < points1.at(k).x || image.rows < points1.at(k).y) {
					points1.erase(points1.begin() + k);
					k--;
				}
			}
			cout << "BRIEF for first image" << endl;
			brief(image.clone(), points1, features1, pairs);

			cout << "FAST for second image" << endl;
			FAST(image2.clone(), points2, fast_threshold, non_max);
			cout << "BRIEF for second image" << endl;
			brief(image2.clone(), points2, features2, pairs);

			/*std::vector<std::tuple<Mat, std::vector<Point>, int >> infos = FAST_multisized(image, points, 20, true, 4);
			for (int i = 0; i < 4; i++) {
			drawPoints(std::get<0>(infos[i]), std::get<1>(infos[i]));
			imwrite("FAST"+to_string(i)+".jpg", std::get<0>(infos[i]));
			}*/
			cout << "Matching" << endl;
			matching(features1, features2, matcher, INT_MAX);

			cout << "Drawing" << endl;
			cv::imwrite("./process/" + to_string(i - 2) + "/matches.jpg", draw_matches(image1_org.clone(), image2_org.clone(), points1, points2, matcher).clone());
			cv::Mat draw_points_image1 = image1_org.clone();
			cv::Mat draw_points_image2 = image2_org.clone();
			drawPoints(draw_points_image1, points1);
			imwrite("./process/" + to_string(i - 2) + "/img1.jpg", draw_points_image1);
			drawPoints(draw_points_image2, points2);
			imwrite("./process/" + to_string(i - 2) + "/img2.jpg", draw_points_image2);
			cout << "Panorama" << endl;
			panorama_image = panorama(image2_org.clone(), image1_org.clone(), matcher, points1, points2, mask_pano);
			imshow("panorama", panorama_image.clone());
			waitKey(30);
			cv::imwrite("./panoramas/panorama" + to_string(i - 1) + ".jpg", panorama_image.clone());
			cv::imwrite("./process/" + to_string(i - 2) + "/mask.jpg", mask_pano);
		}
	}
	for (int i = 3; i < argc; i++) {
		cout << "_______________" << endl;
		if (panorama_image.empty()) {
			CreateDirectory(("./process/" + to_string(i - 1)).c_str(), NULL);
			cout << "1 and 2 image"<< endl;
			image = imread(argv[i]);
			image2 = imread(argv[++i]);
			cv::resize(image, image, cv::Size(), 0.25, 0.25);
			cv::resize(image2, image2, cv::Size(), 0.25, 0.25);
			cout << "FAST for first image" << endl;
			FAST(image.clone(), points1, fast_threshold, non_max);
		}
		else {
			CreateDirectory(("./process/" + to_string(i - 2)).c_str(), NULL);
			cout << "Add "<<(i-2)<<" image"<< endl;
			image = panorama_image.clone();
			image2 = imread(argv[i], CV_LOAD_IMAGE_COLOR);
			cv::resize(image2, image2, cv::Size(), 0.25, 0.25);

			//points1.clear();
			features1.clear();
			points2.clear();
			features2.clear();
		}
		image1_org = image.clone();
		image2_org = image2.clone();
		cv::cvtColor(image2, image2, CV_BGR2GRAY);
		cv::cvtColor(image, image, CV_BGR2GRAY);

		//cout << "FAST for first image" << endl;
		//FAST(image.clone(), points1, fast_threshold, non_max);
		for (int k = 0; k < points1.size(); k++) {
			if (points1.at(k).x < 0 || points1.at(k).y < 0 || image.cols < points1.at(k).x || image.rows < points1.at(k).y) {
				points1.erase(points1.begin() + k);
				k--;
			}
		}
		cout << "BRIEF for first image" << endl;
		brief(image.clone(), points1, features1, pairs);

		cout << "FAST for second image" << endl;
		FAST(image2.clone(), points2, fast_threshold, non_max);
		cout << "BRIEF for second image" << endl;
		brief(image2.clone(), points2, features2, pairs);

		/*std::vector<std::tuple<Mat, std::vector<Point>, int >> infos = FAST_multisized(image, points, 20, true, 4);
		for (int i = 0; i < 4; i++) {
		drawPoints(std::get<0>(infos[i]), std::get<1>(infos[i]));
		imwrite("FAST"+to_string(i)+".jpg", std::get<0>(infos[i]));
		}*/
		cout << "Matching" << endl;
		matching(features1, features2, matcher, INT_MAX);

		cout << "Drawing" << endl;
		cv::imwrite("./process/" + to_string(i - 2) + "/matches.jpg", draw_matches(image1_org.clone(), image2_org.clone(), points1, points2, matcher).clone());
		cv::Mat draw_points_image1 = image1_org.clone();
		cv::Mat draw_points_image2 = image2_org.clone();
		drawPoints(draw_points_image1, points1);
		imwrite("./process/" + to_string(i - 2) + "/img1.jpg", draw_points_image1);
		drawPoints(draw_points_image2, points2);
		imwrite("./process/" + to_string(i - 2) + "/img2.jpg", draw_points_image2);
		cout << "Panorama" << endl;
		panorama_image = panorama(image2_org.clone(), image1_org.clone(), matcher, points1, points2, mask_pano);
		cv::imwrite("./panoramas/panorama" + to_string(i - 2) + ".jpg", panorama_image.clone());
		cv::imwrite("./process/" + to_string(i - 2) + "/mask.jpg", mask_pano);
	}
	return 0;
}