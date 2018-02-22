#!/usr/bin/env bash

# Set environment variables
BUILDS="/opt"
HOME="/home/vagrant"
echo HOME: $HOME

apt-get update >/dev/null 2>&1

# CMake, C/++ compiler ---------------------------------------------------------
echo "Installing C/++ compiler and CMake..."
apt-get install -y cmake gcc g++ >/dev/null 2>&1

# pip --------------------------------------------------------------------------
echo "Installing pip..."
apt-get install -y python3-pip >/dev/null 2>&1

# CGAL requirements ------------------------------------------------------------
echo "Installing Eigen3, Boost, GMP, MPFR..."
# get Boost
apt-get install -y libboost-all-dev >/dev/null 2>&1
# get Eigen3
apt-get install -y libeigen3-dev >/dev/null 2>&1
# get GMP and MPFR
apt-get install -y libgmp10 libgmp-dev libmpfr-dev libmpfr4 >/dev/null 2>&1

apt-get update >/dev/null 2>&1
apt-get upgrade -y >/dev/null 2>&1

# GEOS -------------------------------------------------------------------------
echo "Downloading and building GEOS 3.6.2 ..."
cd $BUILDS
wget https://git.osgeo.org/gitea/geos/geos/archive/3.6.2.tar.gz >/dev/null 2>&1
tar xf 3.6.2.tar.gz
rm 3.6.2.tar.gz
mkdir $BUILDS/geos/build && cd $BUILDS/geos/build
cmake ..
make
geos_test=$(make check | grep -o '100% tests passed')
if [ "$geos_test" = "100% tests passed" ]; then
    make install
    ldconfig
    make clean
else
    echo "GEOS test failed, exiting provisioning"
    exit 1
fi


# CGAL -------------------------------------------------------------------------
# For some reason CGAL cannot link Eigen3 to boost_thread when the release tarball
# is used for compilation. However, everything works when CGAL is compiled from
# the source tarball (copy of GitHub master branch). Only the md5sum is not 
# correct in the provided md5sum.txt
echo "Downloading and building CGAL 4.10.2 ..."
cd $BUILDS
wget https://github.com/CGAL/cgal/archive/releases/CGAL-4.10.2.tar.gz >/dev/null 2>&1
#wget https://github.com/CGAL/cgal/releases/download/releases%2FCGAL-4.10.2/md5sum.txt >/dev/null 2>&1

#CGAL_TAR=$(md5sum CGAL-4.10.2.tar.xz)
#CGAL_MD5=$(cat md5sum.txt | grep CGAL-4.10.2.tar.xz)
#if [ "$CGAL_TAR" = "$CGAL_MD5" ]; then
#    tar -xf CGAL-4.10.2.tar.xz
#    rm CGAL-4.10.2.tar.xz
#    rm md5sum.txt
#else
#    echo "md5sum of CGAL not matching, exiting provisioning"
#    exit 1
#fi

tar -xf CGAL-4.10.2.tar.gz
rm CGAL-4.10.2.tar.gz

mkdir $BUILDS/cgal-releases-CGAL-4.10.2/build && cd $BUILDS/cgal-releases-CGAL-4.10.2/build
cmake .. -DWITH_Eigen3=ON
make
make install
make clean
ldconfig


# pytest -----------------------------------------------------------------------
echo "Installing pytest..."
pip3 install pytest

# val3dity ---------------------------------------------------------------------
echo "Building val3dity..."
# Vagrant shared folders incur a heavy performance penalty within the
# virtual machine when there is heavy I/O, so they should only be used for
# source files. Any compilation step, database files, and so on should be done
# outside the shared folder filesystem inside the guest filesystem itself.
mkdir $HOME/val3dity_build && cd $HOME/val3dity_build
cmake $HOME/val3dity
make
make install
cd $HOME

val3dity --version

chown vagrant:vagrant $HOME/val3dity_build

echo "See VAGRANT.md for additional configuration instructions and then run 'vagrant ssh' to log into the val3dity virtual machine."
