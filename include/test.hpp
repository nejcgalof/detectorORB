#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/legacy/legacy.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <vector>
using namespace cv;

Mat panorama(Mat src1, Mat src2)
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
