#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/legacy/legacy.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <vector>
using namespace cv;
			
cv::Mat panorama(Mat src1, Mat src2, vector<int> matcher, vector<Point> &points1, vector<Point> points2, cv::Mat& mask) {
	std::vector<Point2f> obj;
	std::vector<Point2f> scene;
	for (int i = 0; i < matcher.size(); i++) {
		obj.push_back(Point2f(points1.at(i).x, points1.at(i).y));
		scene.push_back(Point2f(points2.at(matcher.at(i)).x, points2.at(matcher.at(i)).y));
	}
	// Find the Homography Matrix
	Mat H = findHomography(scene, obj, CV_RANSAC);
	
	std::vector<cv::Point2f> corners(4);
	corners[0] = cv::Point2f(0, 0);
	corners[1] = cv::Point2f(0, src1.rows);
	corners[2] = cv::Point2f(src1.cols, 0);
	corners[3] = cv::Point2f(src1.cols, src1.rows);

	std::vector<cv::Point2f> cornersTransform(4);
	cv::perspectiveTransform(corners, cornersTransform, H);
	
	double offsetX = 0.0;
	double offsetY = 0.0;
	//Get max offset outside of the image
	for (size_t i = 0; i < 4; i++) {
		if (cornersTransform[i].x < offsetX) {
			offsetX = cornersTransform[i].x;
		}

		if (cornersTransform[i].y < offsetY) {
			offsetY = cornersTransform[i].y;
		}
	}
	offsetX = -offsetX;
	offsetY = -offsetY;
	//Get max width and height for the new size of the panorama
	double maxX = MAX((double)src2.cols + offsetX, (double)std::max(cornersTransform[2].x, cornersTransform[3].x) + offsetX);
	double maxY = MAX((double)src2.rows + offsetY, (double)std::max(cornersTransform[1].y, cornersTransform[3].y) + offsetY);
	cv::Size size_warp(maxX, maxY);

	//Create the transformation matrix to be able to have all the pixels
	cv::Mat H2 = cv::Mat::eye(3, 3, CV_64F);
	H2.at<double>(0, 2) = offsetX;
	H2.at<double>(1, 2) = offsetY;

	cv::Mat panorama(size_warp, CV_8UC3);
	warpPerspective(src1, panorama, H2*H, size_warp);

	points1.clear();
	cv::perspectiveTransform(obj, obj, H2);
	cv::perspectiveTransform(scene, scene, H2*H);
	for (int i = 0; i < scene.size(); i++) {
		points1.push_back(Point(scene.at(i).x, scene.at(i).y));
	}
	for (int i = 0; i < obj.size(); i++) {
		points1.push_back(Point(obj.at(i).x, obj.at(i).y));
	}
	cv::Mat dva = panorama.clone();
	cv::imwrite("dva.jpg", dva);

	//ROI for img1
	cv::Rect img1_rect(offsetX, offsetY, src2.cols, src2.rows);

	cv::Mat panorama1(size_warp, CV_8UC3);
	//cv::Mat half1 = cv::Mat(panorama1, img1_rect);
	src2.copyTo(panorama1(img1_rect));
	cv::Mat ena = panorama1.clone();
	cv::imwrite("ena.jpg", panorama1);

	cv::Mat mask2;
	cv::Mat mask3;
	if (mask.empty()) {
		//Copy img1 in the panorama using the ROI
		cv::Mat half = cv::Mat(panorama, img1_rect);
		src2.copyTo(half);

		//Create the new mask matrix for the panorama
		mask = cv::Mat::ones(src1.size(), CV_8U) * 255;
		cv::warpPerspective(mask, mask, H2*H, size_warp);
		cv::imwrite("mask1.jpg", mask);

		mask2 = cv::Mat::zeros(size_warp, CV_8U);
		mask2(img1_rect) = cv::Scalar(255);
		cv::imwrite("mask2.jpg", mask2);

		cv::bitwise_and(mask, mask2, mask3);
		cv::imwrite("mask3.jpg", mask3);

		cv::bitwise_or(mask, mask2, mask);

		//perspectiveTransform(points2, points2, H2);
		//perspectiveTransform(points1, points1, H2);

		/*Mat acc(panorama.size(), CV_64F, Scalar(0));
		accumulate(ena, acc);
		accumulate(dva, acc);
		Mat avg;
		acc.convertTo(avg, CV_8U, 1.0 / 2);*/
		//avg.copyTo(panorama, mask3);
	}
	else {
		//Create an image with the final size to paste img1
		cv::Mat maskTmp = cv::Mat::zeros(size_warp, src2.type());
		cv::Mat half = cv::Mat(maskTmp, img1_rect);
		src2.copyTo(half);

		//Copy img1 into panorama using a mask
		cv::Mat maskTmp2 = cv::Mat::zeros(size_warp, CV_8U);
		half = cv::Mat(maskTmp2, img1_rect);
		mask.copyTo(half);
		maskTmp.copyTo(panorama, maskTmp2);

		//Create a mask for the warped part
		maskTmp = cv::Mat::ones(src1.size(), CV_8U) * 255;
		cv::warpPerspective(maskTmp, maskTmp, H2*H, size_warp);

		maskTmp2 = cv::Mat::zeros(size_warp, CV_8U);
		half = cv::Mat(maskTmp2, img1_rect);
		//Copy the old mask in maskTmp2
		mask.copyTo(half);
		//Merge the old mask with the new one
		maskTmp += maskTmp2;
		maskTmp.copyTo(mask);
	}
	return panorama;
}