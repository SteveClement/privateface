:warning: needs testing

# Compiling privateface

Packages needed on Ubuntu 18.04 server or Debian Buster (Raspi inclued)

```
sudo apt-get install libopencv-dev build-essential libglib2.0-dev libopencv-highgui-dev 
# libcvaux-dev -> unavailable in 18.04 bionic
```

To get SDL going you additionally need the following

```
sudo apt-get install libunicap2-dev libsdl1.2-dev
```
If you want to run X11 in "barebone" mode this should suffice 

```
sudo apt-get install xinit
```

In order to better understand the Haar Principles used in privateface, check the following sites:

http://note.sonots.com/SciSoftware/haartraining.html
http://code.google.com/p/tutorial-haartraining/

and grab the following repo:

```
git clone https://github.com/Glavin001/tutorial-haartraining.git
```

# SDLcam 
## What is SDLcam?

SDLcam is a simple Video4Linux program, that was designed to view video streams coming from a Philips USB Webcam.

It uses SDL, and has a simple user interface. SDLcam can save snapshots in Jpeg, PNG or BMP formats. It also has a lot of video filters that can be combined and applied in real time to the video stream. See Features for more info.

http://sdlcam.raphnet.net

opencv-example

Quick and dirty example on how to load a jpeg draw a window and display it

# MacOSX

So you really want this on Mac.

Install Homebrew: 

```
ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
```

Install and do the following on a Terminal:

```
brew install subversion opencv
```

Theoretically all is fine now, but do not forget to adjust your compile line.

As expected libunicap is not "that" easy.

To get libunicap compiled you additionally need to:

```
brew install intltool
```

# UCView

ucview is a quick an dirty way to see if your unicap cam is working. You need libunicap and bazaar to get all going

```
sudo apt-get install libglade2-dev bzr libunicap2-dev intltool libgconf2-dev
```
# libunicapgtk2-dev 

```
mkdir ucview ; cd ucview ; bzr init ; bzr pull lp:ucview
```

OSX quirks:

```
brew install gconf intltool gettext
ln -s /usr/local/Cellar/gettext/0.19.8.1/share/aclocal/nls.m4 /usr/local/share/aclocal/nls.m4
```

## OpenCV from Source

Wrapper: https://github.com/jayrambhia/Install-OpenCV


:warning: Something needs: FIX!!!!

```
sudo apt-get install build-essential cmake cmake-qt-gui
sudo apt-get install libgtk2.0-dev libavcodec-dev libavformat-dev libtiff5-dev
sudo apt-get install libdc1394-22-dev libgstreamermm-1.0-dev
# replace libjasper-dev  libgstreamermm-0.10-dev libgstreamer-plugins-base0.10-dev
sudo apt-get install libswscale-dev libv4l-dev libxine2-dev libunicap-dev

wget https://github.com/Itseez/opencv/archive/2.4.13.7.zip

cmake -D BUILD_EXAMPLES=ON -D CMAKE_INSTALL_PREFIX=/opt ~/OpenCV-2.4.13.7
cd ~/OpenCV-2.4.13.17
make
sudo make install
echo "add: /opt/lib"
sleep 3
sudo vi /etc/ld.so.conf.d/opencv.conf
sudo ldconfig
# Optional
echo "PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/opt/lib/pkgconfig"
echo "export PKG_CONFIG_PATH"
sleep 3
sudo vi /etc/bash.bashrc
# Examples
wget http://dircweb.king.ac.uk/reason/simple-examples.tar.gz
./build-all.sh
./edge fruits.jpg
./cvplayer-v2 SEQ-003-C5_X4.mpg
```

# Compile unicap and ucview on a recent (2016) ubuntu install

```
sudo apt-get install bzr autoconf libtool libgettextpo0 gettext gtk-doc-tools libraw1394-dev  libpango1.0-dev libxv-dev  libgtk2.0-dev libgconf2-dev libglade2-dev libglade2.0 libglademm-2.4-dev libgstreamer-ocaml-dev
# libgstreamer0.10-dev
# libgstreamer-plugins-base0.10-dev
```

# OSX Quirks

```
brew install gtk+ libglade dpkg gtk-doc
ln -s /usr/local/Cellar/gettext/0.19.8.1/share/aclocal/nls.m4 /usr/local/share/aclocal/nls.m4
sudo ln -s /usr/local/Cellar/libtool/2.4.6_1/share/libtool/build-aux/config.sub /usr/share/misc/config.sub
sudo ln -s /usr/local/Cellar/libtool/2.4.6_1/share/libtool/build-aux/config.guess /usr/share/misc/config.guess
```

```
mkdir ~/Desktop/code/unicap ; cd ~/Desktop/code/unicap
bzr init; bzr pull lp:unicap ; cd libunicap
./autogen.sh ; ./configure --datadir=/usr --prefix=/usr --localstatedir=/var --sysconfdir=/etc ; make ; sudo make install

cd ../libucil
./autogen.sh ;  ./configure --datadir=/usr --prefix=/usr --localstatedir=/var --sysconfdir=/etc ; make; sudo make install

cd ../libunicapgtk/
./autogen.sh ;  ./configure --datadir=/usr --prefix=/usr --localstatedir=/var --sysconfdir=/etc ; make; sudo make install

mkdir ~/Desktop/code/ucview ; cd ~/Desktop/code/ucview
bzr init ; bzr pull lp:ucview ; cd trunk/ucview
./autogen.sh;  ./configure --datadir=/usr --prefix=/usr --localstatedir=/var --sysconfdir=/etc ; make; sudo make install

cd ../ucview-plugins/
./autogen.sh; ./configure --datadir=/usr --prefix=/usr --localstatedir=/var --sysconfdir=/etc
```

# OpenCV 3.1 + contrib

```
mkdir ~/Desktop/code ; cd ~Desktop/code
wget https://github.com/Itseez/opencv/archive/3.1.0.zip
unzip 3.1.0.zip
cd opencv-3.1.0 ; mkdir build ; cd build
cmake -DOPENCV_EXTRA_MODULES_PATH=/home/steve/Desktop/code/opencv_contrib/modules -DBUILD_opencv_legacy=OFF -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local -D WITH_TBB=OFF -D WITH_V4L=ON -D WITH_QT=ON -D WITH_OPENGL=ON -D BUILD_EXAMPLES=ON ..
make -j5
cd doc
make -j7 html_docs
cd ../
sudo make install
```

# OpenCV Latest + contrib

```
mkdir ~/Desktop/code ; cd ~Desktop/code
git clone https://github.com/Itseez/opencv.git
git clone https://github.com/Itseez/opencv_contrib.git
cd opencv ; mkdir build ; cd build
cmake -DOPENCV_EXTRA_MODULES_PATH=/home/steve/Desktop/code/opencv_contrib/modules -DBUILD_opencv_legacy=OFF -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local -D WITH_TBB=OFF -D WITH_V4L=ON -D WITH_QT=ON -D WITH_OPENGL=ON -D BUILD_EXAMPLES=ON ..
make -j7 # runs 7 jobs in parallel
cd doc
make -j7 html_docs
cd ..
sudo make install
```
