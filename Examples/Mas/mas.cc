
#include<iostream>
#include<algorithm>
#include<fstream>
#include<iomanip>
#include<chrono>
#include<string>

#include<opencv2/core/core.hpp>

#include<System.h>

using namespace std;
using namespace cv; 


int main(int argc, char **argv)
{
    // Create SLAM system. It initializes all system threads and gets ready to process frames.
    ORB_SLAM2::System SLAM(argv[1],argv[2],ORB_SLAM2::System::MONOCULAR, true);

    cout << endl << "-------" << endl;
    cout << "Start processing sequence ..." << endl;

    //VideoCapture cap(0);  // camera
    VideoCapture cap(argv[3]);

    // Main loop
    Mat frame;
    while(cap.isOpened()) {

        cap >> frame;
        if(frame.empty()) {
            std::cout<<"read fail!!\n";        
            continue;
        }

        double tframe =  std::chrono::system_clock::now().time_since_epoch().count();        

        // Pass the image to the SLAM system
        SLAM.TrackMonocular(frame, tframe);
    }


    // Stop all threads
    SLAM.Shutdown();

    // Save camera trajectory
    SLAM.SaveTrajectoryKITTI("CameraTrajectory.txt");

    return 0;
}
