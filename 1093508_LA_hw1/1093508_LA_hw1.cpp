#include<opencv2/opencv.hpp>
#include<iostream>
#include<string>
#include<vector>
using namespace std;
using namespace cv;

int getMaxAreaContourId(vector <vector<cv::Point>> contours) {// code from https://stackoverflow.com/a/46200638 of course
	double maxArea = 0;
	int maxAreaContourId = -1;
	for (int j = 0; j < contours.size(); j++) {
		double newArea = cv::contourArea(contours.at(j));
		if (newArea > maxArea) {
			maxArea = newArea;
			maxAreaContourId = j;
		} // End if
	} // End for
	return maxAreaContourId;
} // End function


int main(int argc, char* argv[])
{
	//modify this two line for input and output
	String image_path = argv[1];
	String image_path_out = argv[2];


	Mat img = imread(image_path, IMREAD_GRAYSCALE);//load image as greyscale
	if (img.empty()) {
		cout << "Couldn't read the img: " << image_path << endl;
		return 1;
	}
	
	//imshow("Original", img);

	Mat thresh;
	threshold(img, thresh, 200, 255, THRESH_BINARY);

	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(thresh, contours, hierarchy, RETR_TREE, CHAIN_APPROX_NONE);
	
	//=====removeal of noise=====
	Mat imgcpy = img.clone();

	Mat thresh_;
	inRange(imgcpy, 0, 0, thresh_);
	Mat thresh_inverted = 255 - thresh_;

	Mat mask = imgcpy.clone();
	mask = Scalar(0, 0, 0);
	drawContours(mask, contours, getMaxAreaContourId(contours), Scalar(255, 255, 255), -1);//Last arg FILLED is -1 in linux

	Mat mask_inverted = 255 - mask;
	Mat imgmasked;
	bitwise_and(imgcpy, imgcpy, imgmasked, mask);// masking image


	/*
	Mat bg = imgcpy.clone();
	bg = Scalar(0, 0, 255);//make copy of img and turn to blue
	Mat bgmasked;
	bitwise_and(imgcpy, imgcpy, bgmasked, mask_inverted);


	Mat combined;
	add(imgmasked, img, combined);
	*/
	
	//imshow("removal", imgmasked);
	//===== end =====


	
	//find the orientation of the skull
	RotatedRect ellipse_out = fitEllipse(contours[(getMaxAreaContourId(contours))]);//find the largest contour
	

	//=====Drawing outlines for debugging comment before submitting =====
	/*
	* Mat out = img.clone();//making a copy of img
	cvtColor(out, out, COLOR_GRAY2BGR);//convert to RGB so I can draw color on it LOL
	ellipse(out, ellipse_out, Scalar(0, 255, 0, 255), 3);//drawing ellipse
	
	//draw circle at center
	circle(out, ellipse_out.center, 10, (255, 255, 255, 255), -1);
	//imshow("drawing", out);
	*/

	//=====end of Drawing=====

	//rotate the image
	Mat rotation;//matrix for storing rotation matrix
	rotation = getRotationMatrix2D(ellipse_out.center, ellipse_out.angle, 1.0);//ellipse_out.angle for rotation angle
	warpAffine(imgmasked, imgmasked, rotation, imgmasked.size());//rotate the image
	
	
	imwrite(image_path_out, imgmasked);

	//imshow("finsished", imgmasked);
	//end of rotate



	

	

	/*
	Mat image_copy = img.clone();
	drawContours(image_copy, contours, -1, Scalar(0, 255, 0), 2);
	imshow("None approximation", image_copy);

	
	//contourArea(thresh, false);
	imshow("Binary mage", thresh);
	*/



	
	//waitKey(0);
	//return 0;
}