Packages needed on Ubuntu 10.04.1 server

 sudo apt-get install libcv-dev build-essential libglib2.0-dev libhighgui-dev libcvaux-dev

To get SDL going you additionally need the following

 sudo apt-get install libunicap2-dev libsdl1.2-dev

If you want to run X11 in "barebone" mode this should suffice 

 sudo apt-get install xinit


In order to better understand the Haar Principles used in privateface, check the following sites:

http://note.sonots.com/SciSoftware/haartraining.html
http://code.google.com/p/tutorial-haartraining/

and grab the following repo:

svn co http://tutorial-haartraining.googlecode.com/svn/trunk/HaarTraining/

= SDLcam =

== What is SDLcam? ==

SDLcam is a simple Video4Linux program, that was designed to view video streams coming from a Philips USB Webcam.

It uses SDL, and has a simple user interface. SDLcam can save snapshots in Jpeg, PNG or BMP formats. It also has a lot of video filters that can be combined and applied in real time to the video stream. See Features for more info.

http://sdlcam.raphnet.net

opencv-example

Quick and dirty example on how to load a jpeg draw a window and display it

= MacOSX =

So you really want this on Mac.

Grab MacPorts:

http://www.macports.org/

Install and do the following on a Terminal:

port install subversion opencv

in theory all is fine now, but do not forget to adjust you compile line

As expected libunicap is not "that" easy.

to get libunicap compiled you additionally need to:

sudo port install intltool

= UCView =

ucview is a quick an dirty way to see if your unicap cam is working. You need libunicap and bazaar to get all going

sudo apt-get install libglade2-dev bzr libunicap2-dev libunicapgtk2-dev intltool libgconf2-dev

mkdir ucview ; cd ucview ; bzr init ; bzr pull lp:ucview

== OpenCV from Source ==

Wrapper: https://github.com/jayrambhia/Install-OpenCV

sudo apt-get install build-essential cmake cmake-qt-gui
sudo apt-get install libgtk2.0-dev libavcodec-dev libavformat-dev libjpeg62-dev libtiff4-dev
sudo apt-get install libdc1394-22-dev libjasper-dev libgstreamer0.10-dev libgstreamermm-0.10-dev libgstreamer-plugins-base0.10-dev
sudo apt-get install libswscale-dev libv4l-dev libxine-dev libunicap-dev

cmake -D BUILD_EXAMPLES=ON -D CMAKE_INSTALL_PREFIX=/opt ~/OpenCV-2.1.0
cd ~/OpenCV-2.1.0
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

# Compile unicap and ucview on a recent (2015) ubuntu install
sudo apt-get install bzr autoconf libtool libgettextpo0 gettext gtk-doc-tools libraw1394-dev  libpango1.0-dev libgstreamer0.10-dev libxv-dev  libgtk2.0-dev libgconf2-dev libglade2-dev libglade2.0 libglademm-2.4-dev libgstreamer-ocaml-dev libgstreamer-plugins-base0.10-dev

mkdir ~/unicap & cd ~/unicap
bzr init; bzr pull lp:unicap ; cd libunicap
./autogen.sh ; ./configure --datadir=/usr --prefix=/usr --localstatedir=/var --sysconfdir=/etc ; make ; sudo make install

cd ../libucil
./autogen.sh ;  ./configure --datadir=/usr --prefix=/usr --localstatedir=/var --sysconfdir=/etc ; make; sudo make install

cd ../libunicapgtk/
./autogen.sh ;  ./configure --datadir=/usr --prefix=/usr --localstatedir=/var --sysconfdir=/etc ; make; sudo make install

mkdir ~/ucview & cd ~/ucview
bzr init ; bzr pull lp:ucview ; cd trunk/ucview
./autogen.sh;  ./configure --datadir=/usr --prefix=/usr --localstatedir=/var --sysconfdir=/etc ; make; sudo make install

cd ../ucview-plugins/
./autogen.sh; ./configure --datadir=/usr --prefix=/usr --localstatedir=/var --sysconfdir=/etc
