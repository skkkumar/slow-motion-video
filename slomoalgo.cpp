/*
 * slomoalgo.cpp
 *
 *  Created on: Sep 8, 2015
 *      Author: sriram
 */

#include "slomoalgo.h"

// Constructor : Initializing Source and Output file locations, Factor = Slowdown speed (2x or 4x)
slomoalgo::slomoalgo(int factor_, string input_, string output_) {
	source.open(input_);
	if (!source.isOpened())
		throw "Error in opening input file";
	output.open(output_, CV_FOURCC('M', 'J', 'P', 'G'),
			source.get(CV_CAP_PROP_FPS),
			Size((int) source.get(CV_CAP_PROP_FRAME_WIDTH), // Acquire input size
			(int) source.get(CV_CAP_PROP_FRAME_HEIGHT)), true);
	if (!output.isOpened())
		throw "Error in opening output file";
	factor = factor_;
	frame_count = 0;
}

slomoalgo::~slomoalgo() {
	// TODO Auto-generated destructor stub
}

// It blurs the optical flow at 100 pixels in the border.
// The optical flow in the border are very noisy.
// So Smoothing the optical flow would reduce the jittery motion.
void slomoalgo::motionBlur() {
	try {
		const int gsize = 15;
		int width = uflow.cols;
		int height = uflow.rows;
		const int border = 100;
		// Apply gaussianblur in the borders.
		cv::Rect region(0, 0, border, height);
		cv::GaussianBlur(uflow(region), uflow(region), Size(0, 0), gsize);
		cv::Rect region1(width - border, 0, border, height);
		cv::GaussianBlur(uflow(region1), uflow(region1), Size(0, 0), gsize);
		cv::Rect region2(border, 0, width - 2 * border, border);
		cv::GaussianBlur(uflow(region2), uflow(region2), Size(0, 0), gsize);
		cv::Rect region3(border, height - border, width - 2 * border, border);
		cv::GaussianBlur(uflow(region3), uflow(region3), Size(0, 0), gsize);
	} catch (...) {
		throw " Problem in Motion Blur module ";
	}
}

// Returns the number of frames in the source video
int slomoalgo::getNoofFrame() {
	return source.get(7);
}

// Retrieve frames from the source video
bool slomoalgo::capture_new_frame(bool flag) {
	bool bSuccess;
	if (flag) {
		bSuccess = source.read(frame1); // read frame1 from source video
		if (!bSuccess) //if not success, break loop
		{
			return false;
		}
	} else
		frame2.copyTo(frame1);  // copy frame2 to frame1
	bSuccess = source.read(frame2); // read frame2 from source video
	if (!bSuccess) //if not success, break loop
	{
		return false;
	}
	cvtColor(frame1, frame1_gray, COLOR_BGR2GRAY); // Convert BGR2GRAY frame 1 and frame 2
	cvtColor(frame2, frame2_gray, COLOR_BGR2GRAY);
	return true;
}

// Check if frame2 is empty
bool slomoalgo::frame_present() {
	if (frame2.empty())
		return false;
	else
		return true;
}

// Compute optical flow : Using Farneback(Default arguments are set.)
void slomoalgo::compute_optical_flow(double pyr_scale, int levels, int winsize,
		int iterations, int poly_n, double poly_sigma) {
	try {
		calcOpticalFlowFarneback(frame2_gray, frame1_gray, uflow, pyr_scale,
				levels, winsize, iterations, poly_n, poly_sigma,
				OPTFLOW_FARNEBACK_GAUSSIAN);
		//perform motion blur
		motionBlur();
	} catch (...) {
		throw "Problem occurred when computing optical flow";
	}
	return;
}

// The optical flow (u,v) for each pixel in split by slowdown value (here it is split in to 4)
// and the intermediate positions are calculated and interpolated to those positions.
// Intermediate Frames are produced and added to the output file. Each time the frame1 is also added to
// the output file.
void slomoalgo::interpolateAndsave() {
	try {
		output << frame1;
		frame_count++;
		for (int k = 1; k < factor; k++) {
			//determine the source pixels (float) which will fill the destination pixels (int)
			Mat locx = Mat(frame1_gray.rows, frame1_gray.cols, CV_32FC1,
					cvScalar(0.));
			Mat locy = Mat(frame1_gray.rows, frame1_gray.cols, CV_32FC1,
					cvScalar(0.));
			Mat filled = Mat(frame1_gray.rows, frame1_gray.cols, CV_32FC1,
					cvScalar(0.));
			for (int y = 0; y < frame1_gray.rows; y += 1)
				for (int x = 0; x < frame1_gray.cols; x += 1) {
					const Point2f& fxy = uflow.at<Point2f>(y, x);
					//apply optical flow at the destination pixel to find the float source pixel
					float rx = (x + fxy.x * ((k) / factor));
					float ry = (y + fxy.y * ((k) / factor));
					if (rx > 0 && rx < frame1_gray.cols && ry > 0
							&& ry < frame1_gray.rows) {
						//if the pixel is in image range then write it
						locx.at<float>(y, x) = rx;
						locy.at<float>(y, x) = ry;
						//keep a check that this destination pixel has color in it
						filled.at<float>(y, x) = 1;
					}
				}

			//for the destination pixels which were ignored, use the same pixel location from the source
			for (int y = 0; y < frame1_gray.rows; y += 1)
				for (int x = 0; x < frame1_gray.cols; x += 1) {
					if (filled.at<float>(y, x) == 0) {
						locx.at<float>(y, x) = x;
						locy.at<float>(y, x) = y;
					}
				}
			//apply reverse interpolation on image 1
			Mat new_frame;
			remap(frame1, new_frame, locx, locy, CV_INTER_LINEAR, BORDER_WRAP,
					Scalar(0, 0, 0));
			output << new_frame;
			frame_count++;
		}
	} catch (...) {
		throw "Error in Interpolation and Saving";
	}
}

// Save the final frame1 to the output as the frame2 will be null.
void slomoalgo::outputfinalframe() {
	output << frame1;
	frame_count++;
}

// Return the current frame count for tracking how much process is done.
int slomoalgo::curr_proc_frame_no() {
	return frame_count;
}
