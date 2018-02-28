
#include<iostream>
#include<algorithm>
#include<fstream>
#include<iomanip>
#include<chrono>
#include<string>

#include<opencv2/core/core.hpp>

#include<System.h>
#include <fstream>
    
using namespace std;
using namespace cv; 


int main(int argc, char **argv)
{
    // Create SLAM system. It initializes all system threads and gets ready to process frames.
    ORB_SLAM2::System SLAM(argv[1],argv[2],ORB_SLAM2::System::MONOCULAR, true);

    cout << endl << "-------" << endl;
    cout << "Start processing sequence ..." << endl;

    std::ifstream timestamps(argv[4]);

    //VideoCapture cap(0);  // camera
    VideoCapture cap(argv[3]);

    // Main loop
    Mat orig;
    while(cap.isOpened()) {

        cap >> orig;
        if(orig.empty()) {
            std::cout<<"read fail!!\n";        
            continue;
        }

        Mat frame;
        resize(orig, frame, Size(), 0.5, 0.5);
        std::cout<<frame.size()<<"\n";

        int i;       
        double tframe;
        //tframe =  std::chrono::system_clock::now().time_since_epoch().count(); 
        timestamps >> i >> tframe;

        std::cout<<i<<" "<<tframe<<"\n";

        // Pass the image to the SLAM system
        SLAM.TrackMonocular(frame, tframe);
    }


    // Stop all threads
    SLAM.Shutdown();

    // Save camera trajectory
    SLAM.SaveTrajectoryKITTI("CameraTrajectory.txt");

    return 0;
}
