#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/legacy/legacy.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <vector>
using namespace cv;

Mat panorama(Mat src1, Mat src2, Mat& mask)
{
	vector<KeyPoint> keyPoints_obj, keyPoints_scene;
	Mat descriptors_obj, descriptors_scene;

	ORB orb;
	orb(src1, Mat(), keyPoints_obj, descriptors_obj);
	orb(src2, Mat(), keyPoints_scene, descriptors_scene);
	
	BruteForceMatcher<HammingLUT> matcher;
	vector<DMatch> matches;
	matcher.match(descriptors_obj, descriptors_scene, matches);

	double max_dist = 0; double min_dist = 100;
	//-- Quick calculation of max and min distances between keypoints
	for (int i = 0; i < descriptors_obj.rows; i++)
	{
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}
	printf("-- Max dist : %f \n", max_dist);
	printf("-- Min dist : %f \n", min_dist);
	//-- Draw only "good" matches (i.e. whose distance is less than 0.6*max_dist )
	//-- PS.- radiusMatch can also be used here.
	std::vector< DMatch > good_matches;
	for (int i = 0; i < descriptors_obj.rows; i++)
	{
		if (matches[i].distance < 0.6*max_dist)
		{
			good_matches.push_back(matches[i]);
		}
	}

	// localize the object
	std::vector<Point2f> obj;
	std::vector<Point2f> scene;

	for (int i = 0; i < good_matches.size(); i++)
	{
		//-- Get the keypoints from the good matches
		obj.push_back(keyPoints_obj[good_matches[i].queryIdx].pt);
		scene.push_back(keyPoints_scene[good_matches[i].trainIdx].pt);
	}

	Mat img_matches;
	drawMatches(src1, keyPoints_obj, src2, keyPoints_scene, matches, img_matches);
	imwrite("matches.jpg", img_matches);
	// Find the Homography Matrix
	Mat H = findHomography(obj, scene, CV_RANSAC);

	//My ADD
	std::vector<cv::Point2f> corners(4);
	corners[0] = cv::Point2f(0, 0);
	corners[1] = cv::Point2f(0, src2.rows);
	corners[2] = cv::Point2f(src2.cols, 0);
	corners[3] = cv::Point2f(src2.cols, src2.rows);

	std::vector<cv::Point2f> cornersTransform(4);
	cv::perspectiveTransform(corners, cornersTransform, H);

	double offsetX = 0.0;
	double offsetY = 0.0;

	//Get max offset outside of the image
	for (size_t i = 0; i < 4; i++) {
		std::cout << "cornersTransform[" << i << "]=" << cornersTransform[i] << std::endl;
		if (cornersTransform[i].x < offsetX) {
			offsetX = cornersTransform[i].x;
		}

		if (cornersTransform[i].y < offsetY) {
			offsetY = cornersTransform[i].y;
		}
	}

	offsetX = -offsetX;
	offsetY = -offsetY;
	std::cout << "offsetX=" << offsetX << " ; offsetY=" << offsetY << std::endl;
	//Get max width and height for the new size of the panorama
	double maxX = std::max((double)src1.cols + offsetX, (double)std::max(cornersTransform[2].x, cornersTransform[3].x) + offsetX);
	double maxY = std::max((double)src1.rows + offsetY, (double)std::max(cornersTransform[1].y, cornersTransform[3].y) + offsetY);
	std::cout << "maxX=" << maxX << " ; maxY=" << maxY << std::endl;

	cv::Size size_warp(maxX, maxY);
	cv::Mat panorama(size_warp, CV_8UC3);
	//Create the transformation matrix to be able to have all the pixels
	cv::Mat H2 = cv::Mat::eye(3, 3, CV_64F);
	H2.at<double>(0, 2) = offsetX;
	H2.at<double>(1, 2) = offsetY;

	cv::warpPerspective(src1, panorama, H2*H, size_warp);
	imwrite("rotate.jpg", panorama);
	cv:Mat half1;
	//ROI for img1
	cv::Rect img1_rect(offsetX, offsetY, src1.cols, src1.rows);
	if (mask.empty()) {
		cv::Mat half1 = cv::Mat(panorama, img1_rect);
		src2.copyTo(half1);

		mask = cv::Mat::ones(src1.size(), CV_8U) * 255;
		warpPerspective(mask, mask, H2*H, size_warp);
		cv::rectangle(mask, img1_rect, cv::Scalar(255), -1);
	}
	else {
		imwrite("mask.jpg", mask);
		cv::Mat maskTmp = cv::Mat::zeros(size_warp, src2.type());
		half1 = cv::Mat(maskTmp, img1_rect);
		src2.copyTo(half1);

		//Copy img1 into panorama using a mask
		cv::Mat maskTmp2 = cv::Mat::zeros(size_warp, CV_8U);
		half1 = cv::Mat(maskTmp2, img1_rect);
		mask.copyTo(half1);
		maskTmp.copyTo(panorama, maskTmp2);

		//Create a mask for the warped part
		maskTmp = cv::Mat::ones(src1.size(), CV_8U) * 255;
		cv::warpPerspective(maskTmp, maskTmp, H2*H, size_warp);

		maskTmp2 = cv::Mat::zeros(size_warp, CV_8U);
		half1 = cv::Mat(maskTmp2, img1_rect);
		//Copy the old mask in maskTmp2
		mask.copyTo(half1);
		//Merge the old mask with the new one
		maskTmp += maskTmp2;
		maskTmp.copyTo(mask);
	}
	return panorama;
}

Mat panorama_old(Mat src1, Mat src2)
{
	OrbFeatureDetector detector(3000);
	OrbDescriptorExtractor extrator;
	vector<KeyPoint> keyPoints_obj, keyPoints_scene;
	Mat descriptors_obj, descriptors_scene;

	//ORB orb;
	//orb(src1, Mat(), keyPoints_obj, descriptors_obj);
	//orb(src2, Mat(), keyPoints_scene, descriptors_scene);
	detector.detect(src1, keyPoints_obj);
	detector.detect(src2, keyPoints_scene);
	extrator.compute(src1, keyPoints_obj, descriptors_obj);
	extrator.compute(src2, keyPoints_scene, descriptors_scene);

	BruteForceMatcher<HammingLUT> matcher;
	vector<DMatch> matches;
	matcher.match(descriptors_obj, descriptors_scene, matches);

	double max_dist = 0; double min_dist = 100;
	//-- Quick calculation of max and min distances between keypoints
	for (int i = 0; i < descriptors_obj.rows; i++)
	{
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}
	printf("-- Max dist : %f \n", max_dist);
	printf("-- Min dist : %f \n", min_dist);
	//-- Draw only "good" matches (i.e. whose distance is less than 0.6*max_dist )
	//-- PS.- radiusMatch can also be used here.
	std::vector< DMatch > good_matches;
	for (int i = 0; i < descriptors_obj.rows; i++)
	{
		if (matches[i].distance < 0.6*max_dist)
		{
			good_matches.push_back(matches[i]);
		}
	}

	// localize the object
	std::vector<Point2f> obj;
	std::vector<Point2f> scene;

	for (int i = 0; i < good_matches.size(); i++)
	{
		//-- Get the keypoints from the good matches
		obj.push_back(keyPoints_obj[good_matches[i].queryIdx].pt);
		scene.push_back(keyPoints_scene[good_matches[i].trainIdx].pt);
	}

	// Find the Homography Matrix
	Mat H = findHomography(obj, scene, CV_RANSAC);
	// Use the Homography Matrix to warp the images
	cv::Mat result;
	warpPerspective(src1, result, H, cv::Size(src1.cols + src2.cols, src1.rows));
	cv::Mat half(result, cv::Rect(0, 0, src2.cols, src2.rows));
	src2.copyTo(half);
	return result;
}
