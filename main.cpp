/*
 * main.cpp
 *
 *  Created on: Sep 8, 2015
 *      Author: sriram
 */

#include "slomoalgo.h"
#include <sstream>
static void show_usage() {
	std::cerr << "Usage: " << " Only 3 arguments are accepted." << std::endl;
	std::cerr << "Example Command 1: "
			<< "./slowmotion 4 ../inputvideo.avi ../outputvideo.avi"
			<< std::endl;
	std::cerr << "Example Command 2: "
			<< "./slowmotion 2"
			<< std::endl;
	std::cerr << "Example Command 3: "
			<< "./slowmotion"
			<< std::endl;
}

int main(int argc, char* argv[]) {
	slomoalgo agent; // Slowmotion algorithm agent
	int complete_perc = 0; // variable for progress status
	int slowBy;
	std::string in, out;

	//  Initialize the constructor of slomoalgo agent with different conditions.
	if (argc > 4 || (argc < 4 && argc > 2)) {
		show_usage();  // Show how to call the program
		return 0;
	} else if (argc == 4) {
		slowBy = atoi(argv[1]);
		in = *(argv[2]);
		out = *(argv[3]);
		agent = slomoalgo(slowBy, argv[2], argv[3]);
	} else if (argc == 2) {
		slowBy = atoi(argv[1]);
		agent = slomoalgo(atoi(argv[1]));
	} else {
		slowBy = 4;
		agent = slomoalgo();
	}
	//=====================

	int frames_to_process = agent.getNoofFrame() * slowBy; // Total frames after the process
	if (!agent.capture_new_frame(true))  // Retrieve frame1 and frame2
		return 0;
	while (agent.frame_present()) {
		try {
			agent.compute_optical_flow(); // compute optical flow
			agent.interpolateAndsave(); // Interpolation and save output
			if (!agent.capture_new_frame()) { // Reinitialize frame1 and frame2
				agent.outputfinalframe();	// if frame2 is empty, add the final frame1 to the output file.
				cout << "*** 100% Completed ***" << endl << flush;
				break;
			}
			// For progress Status
			if (complete_perc
					!= (agent.curr_proc_frame_no() * 100) / frames_to_process) {
				complete_perc = (agent.curr_proc_frame_no() * 100)
						/ frames_to_process;
				cout << complete_perc << "% Completed" << endl << flush;
			}
		} catch (...) {
			cerr << "Problem in Main" << endl;
			return 0;
		}
	}
	return 1;
}
