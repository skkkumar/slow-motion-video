/*
 * slomoalgo.h
 *
 *  Created on: Sep 8, 2015
 *      Author: sriram
 */

#ifndef SLOMOALGO_H_
#define SLOMOALGO_H_
#include <iostream>
#include <cv.h>
#include "opencv2/nonfree/nonfree.hpp"
#include <highgui.h>
using namespace std;
using namespace cv;
class slomoalgo {
	VideoCapture source;
	VideoWriter output;
	Mat frame1, frame2, frame1_gray, frame2_gray, uflow;
	double factor;
	int frame_count;
public:
	slomoalgo(int factor_ = 4, String input_ = "../jump.mp4", String output_ =
			"../output.avi");
	virtual ~slomoalgo();
	int getNoofFrame();
	bool capture_new_frame(bool flag = false);
	bool frame_present();
	void motionBlur();
	void compute_optical_flow(double pyr_scale = 0.5, int levels = 5,
			int winsize = 51, int iterations = 5, int poly_n = 7,
			double poly_sigma = 1.5);
	void interpolateAndsave();
	void outputfinalframe();
	int curr_proc_frame_no();
};

#endif /* SLOMOALGO_H_ */
