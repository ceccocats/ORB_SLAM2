# Calibration 
CALIB_EXE="../../../stereo-calibration/build/"

# left
$CALIB_EXE/calibrate -w 9 -h 6 -n 14 -s 0.24 -d "calib_imgs/" -i "left-" -o "calib_conf/cam_left.yml" -e "jpg"
echo "left calibrated"

# right
$CALIB_EXE/calibrate -w 9 -h 6 -n 14 -s 0.24 -d "calib_imgs/" -i "right-" -o "calib_conf/cam_right.yml" -e "jpg"
echo "right calibrated"

# stereo
$CALIB_EXE/calibrate_stereo -n 14 -u calib_conf/cam_left.yml -v calib_conf/cam_right.yml -L calib_imgs/ -R calib_imgs/ -l left- -r right- -o calib_conf/cam_stereo.yml
echo "stereo calibrated"

# test
$CALIB_EXE/undistort_rectify -l calib_imgs/left-0.jpg -r calib_imgs/right-0.jpg -c calib_conf/cam_stereo.yml -L left.jpg -R right.jpg

# generate yaml
function gen_yaml() {
    echo "%YAML:1.0\n"
    identify -format "Camera.height: %[fx:h]\nCamera.width: %[fx:w]\n" left.jpg
    camP=(`cat calib_conf/cam_stereo.yml | awk "/P1/,/]/" | xargs | grep -oe "\[.*\]" | cut -d"[" -f2 | tr -d "]" | cut -d"," -f1- --output-delimiter=" " | xargs`)
    echo -e "\nCamera.fx:" ${camP[0]} 
    echo -e "Camera.fy:" ${camP[5]} 
    echo -e "Camera.cx:" ${camP[2]} 
    echo -e "Camera.cy:" ${camP[6]}
    camD=(`cat calib_conf/cam_stereo.yml | awk "/D1/,/]/" | xargs | grep -oe "\[.*\]" | cut -d"[" -f2 | tr -d "]" | cut -d"," -f1- --output-delimiter=" " | xargs`)
    echo -e "\nCamera.k1:" ${camD[0]} 
    echo -e "Camera.k2:" ${camD[1]} 
    echo -e "Camera.p1:" ${camD[2]} 
    echo -e "Camera.p2:" ${camD[3]}


    echo -e "\nCamera.fps: 30.0"
    echo -e "Camera.bf:" `python -c "print ${camP[0]}*0.12"`
    echo -e "Camera.RGB: 0"
    echo -e "ThDepth: 35\n\n"


    identify -format "LEFT.height: %[fx:h]\nLEFT.width: %[fx:w]\n" left.jpg
    cat calib_conf/cam_stereo.yml | awk "/D1/,/]/" | sed -e "s/D1/LEFT.D/g" 
    cat calib_conf/cam_stereo.yml | awk "/K1/,/]/" | sed -e "s/K1/LEFT.K/g" 
    cat calib_conf/cam_stereo.yml | awk "/R1/,/]/" | sed -e "s/R1/LEFT.R/g" 
    cat calib_conf/cam_stereo.yml | awk "/P1/,/]/" | sed -e "s/P1/LEFT.P/g"

    identify -format "RIGHT.height: %[fx:h]\nRIGHT.width: %[fx:w]\n" right.jpg
    cat calib_conf/cam_stereo.yml | awk "/D2/,/]/" | sed -e "s/D2/RIGHT.D/g" 
    cat calib_conf/cam_stereo.yml | awk "/K2/,/]/" | sed -e "s/K2/RIGHT.K/g" 
    cat calib_conf/cam_stereo.yml | awk "/R2/,/]/" | sed -e "s/R2/RIGHT.R/g" 
    cat calib_conf/cam_stereo.yml | awk "/P2/,/]/" | sed -e "s/P2/RIGHT.P/g" 

    echo -e \
"\n\n#--------------------------------------------------------------------------------------------
# ORB Parameters
#--------------------------------------------------------------------------------------------

# ORB Extractor: Number of features per image
ORBextractor.nFeatures: 1200

# ORB Extractor: Scale factor between levels in the scale pyramid 	
ORBextractor.scaleFactor: 1.2

# ORB Extractor: Number of levels in the scale pyramid	
ORBextractor.nLevels: 8

# ORB Extractor: Fast threshold
# Image is divided in a grid. At each cell FAST are extracted imposing a minimum response.
# Firstly we impose iniThFAST. If no corners are detected we impose a lower value minThFAST
# You can lower these values if your images have low contrast			
ORBextractor.iniThFAST: 20
ORBextractor.minThFAST: 7

#--------------------------------------------------------------------------------------------
# Viewer Parameters
#--------------------------------------------------------------------------------------------
Viewer.KeyFrameSize: 0.05
Viewer.KeyFrameLineWidth: 1
Viewer.GraphLineWidth: 0.9
Viewer.PointSize:2
Viewer.CameraSize: 0.08
Viewer.CameraLineWidth: 3
Viewer.ViewpointX: 0
Viewer.ViewpointY: -0.7
Viewer.ViewpointZ: -1.8
Viewer.ViewpointF: 500"
}

gen_yaml > mas_stereo.yaml