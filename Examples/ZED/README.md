# Calibration 

### left
./calibrate -w 8 -h 6 -n 20 -s 0.024 -d "../../ORB_SLAM2/Examples/ZED/calib_imgs/" -i "left" -o "cam_left.yml" -e "jpg"
### right
./calibrate -w 8 -h 6 -n 20 -s 0.024 -d "../../ORB_SLAM2/Examples/ZED/calib_imgs/" -i "right" -o "cam_right.yml" -e "jpg"

### stereo
./calibrate_stereo -n 20 -u cam_left.yml -v cam_right.yml -L ../../ORB_SLAM2/Examples/ZED/calib_imgs/ -R ../../ORB_SLAM2/Examples/ZED/calib_imgs/ -l left -r right -o cam_stereo.yml

### test
./undistort_rectify -l ../../ORB_SLAM2/Examples/ZED/calib_imgs/left0.jpg -r ../../ORB_SLAM2/Examples/ZED/calib_imgs/right0.jpg -c cam_stereo.yml -L left.jpg -R right.jpg

### merge
in cam_stereo.yml matricies:
1 is left
2 is right

fx, cx, fy, cy     are in P matrix
k1, k2, p1, p2, k3 are in D matrix


# Run
./Examples/Stereo/stereo_zed Vocabulary/ORBvoc.txt Examples/ZED/zed.yaml 

