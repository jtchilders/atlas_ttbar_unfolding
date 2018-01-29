#!/usr/bin/env bash

#SETUP ROOT
export ROOT_VERSION=5.34.01
export BUILD_TYPE=dbg
export ROOTSYS=/afs/cern.ch/sw/lcg/app/releases/ROOT/$ROOT_VERSION

build=$(uname -m)
build64='x86_64'
if [ $build = $build64 ]; then
   echo "Setting up 64 bit build of ROOT version" $ROOT_VERSION
   ROOT_BUILD=x86_64-slc5-gcc43-$BUILD_TYPE
   GCC_BUILD=x86_64-slc5
else
   echo "Setting up 32 bit build of ROOT version" $ROOT_VERSION
   ROOT_BUILD=i686-slc5-gcc43-$BUILD_TYPE
   GCC_BUILD=i686-slc5
fi



export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$ROOTSYS/$ROOT_BUILD/lib
export PATH=$PATH:$ROOTSYS/$ROOT_BUILD/bin
export PYTHONPATH=$ROOTSYS/$ROOT_BUILD/lib:$PYTHONPATH

HERE=$PWD

source /afs/cern.ch/sw/lcg/contrib/gcc/4.3/$GCC_BUILD/setup.sh
source $ROOTSYS/$ROOT_BUILD/root/bin/thisroot.sh
cd $HERE

cd ..
export LD_LIBRARY_PATH=$PWD/RooUnfold:$LD_LIBRARY_PATH
cd $HERE
