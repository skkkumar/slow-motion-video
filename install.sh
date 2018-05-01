#set the global path, so that it can be used eerywhere
PATH=$PATH:/bin:/usr/bin
export PATH

#check if cmake is installed
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' cmake|grep "install ok installed")
echo Checking for cmake: $PKG_OK
if [ "" == "$PKG_OK" ]; then
  echo "No somelib. Setting up somelib."
  sudo apt-get --force-yes --yes install cmake
fi

if [ -d "build" ]; then
    echo "build already exists"
else
    echo "creating build"
    echo $(mkdir build)
fi

echo "running ccmake"
echo $(cd build && cmake ..)
echo "running make"
echo $(cd build && make)
echo "installing"
#echo $(cd build && make install)
echo "===================================="
echo "Now run the process by 'cd build' and typing './slowmotion [speed reduction] [input video] [output video]'"
echo "e.g. './slowmotion 4 ../jump.mp4 ../output.avi'"
#echo $(cd build && ./collectdata 4)
echo "===================================="
