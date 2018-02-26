
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

void capture_for_calib(Mat l, Mat r, const char *folder) {
    
    static int captured = 0;

    std::string l_file = std::string(folder) + "/left" + std::to_string(captured) + ".jpg";
    std::string r_file = std::string(folder) + "/right" + std::to_string(captured) + ".jpg";

    imshow("left",  l);
    imshow("right", r);
    int key = waitKey(20); // waits to display frame
    
    if(key == (int)'c') {
        imwrite(l_file.c_str(), l);
        imwrite(r_file.c_str(), r);
        captured++;

        std::cout<<"left: "<<l.cols<<" "<<l.rows<<"\n";
        std::cout<<"right: "<<r.cols<<" "<<r.rows<<"\n";    
        std::cout<<captured<<" captured!!!\n";
    }
}


int main(int argc, char **argv)
{
    // Create SLAM system. It initializes all system threads and gets ready to process frames.
    ORB_SLAM2::System SLAM(argv[1],argv[2],ORB_SLAM2::System::STEREO,true);


    // Read rectification parameters
    cv::FileStorage fsSettings(argv[2], cv::FileStorage::READ);
    if(!fsSettings.isOpened())
    {
        cerr << "ERROR: Wrong path to settings" << endl;
        return -1;
    }

    cv::Mat K_l, K_r, P_l, P_r, R_l, R_r, D_l, D_r;
    fsSettings["LEFT.K"] >> K_l;
    fsSettings["RIGHT.K"] >> K_r;

    fsSettings["LEFT.P"] >> P_l;
    fsSettings["RIGHT.P"] >> P_r;

    fsSettings["LEFT.R"] >> R_l;
    fsSettings["RIGHT.R"] >> R_r;

    fsSettings["LEFT.D"] >> D_l;
    fsSettings["RIGHT.D"] >> D_r;

    int rows_l = fsSettings["LEFT.height"];
    int cols_l = fsSettings["LEFT.width"];
    int rows_r = fsSettings["RIGHT.height"];
    int cols_r = fsSettings["RIGHT.width"];

    if( K_l.empty() || K_r.empty() || P_l.empty() || P_r.empty() || 
        R_l.empty() || R_r.empty() || D_l.empty() || D_r.empty() ||
        rows_l==0 || rows_r==0 || cols_l==0 || cols_r==0)
    {
        cerr << "ERROR: Calibration parameters to rectify stereo are missing!" << endl;
        return -1;
    }

    cv::Mat M1l,M2l,M1r,M2r;
    cv::initUndistortRectifyMap(K_l,D_l,R_l,P_l.rowRange(0,3).colRange(0,3),
        cv::Size(cols_l,rows_l),CV_32F,M1l,M2l);
    cv::initUndistortRectifyMap(K_r,D_r,R_r,P_r.rowRange(0,3).colRange(0,3),
        cv::Size(cols_r,rows_r),CV_32F,M1r,M2r);
    // -------------------------------------------------------------------------

    cout << endl << "-------" << endl;
    cout << "Start processing sequence ..." << endl;

    VideoCapture cap(0);  

    // Main loop
    Mat frame;
    cv::Mat imLeft, imRight, imLeftRect, imRightRect;
    while(cap.isOpened()) {

        cap >> frame;
        if(frame.empty()) {
            std::cout<<"read fail!!\n";        
            continue;
        }

        double tframe =  std::chrono::system_clock::now().time_since_epoch().count();        

        imLeft  = frame(Rect(0,            0, frame.cols/2, frame.rows));
        imRight = frame(Rect(frame.cols/2, 0, frame.cols/2, frame.rows));

        //capture_for_calib(imLeft, imRight, "calib_imgs");

        //rectify
        cv::remap(imLeft, imLeftRect, M1l, M2l, cv::INTER_LINEAR);
        cv::remap(imRight,imRightRect,M1r, M2r, cv::INTER_LINEAR);


        // Pass the images to the SLAM system
        SLAM.TrackStereo(imLeftRect, imRightRect, tframe);
    }


    // Stop all threads
    SLAM.Shutdown();

    // Save camera trajectory
    SLAM.SaveTrajectoryKITTI("CameraTrajectory.txt");

    return 0;
}
