# hist-opt
experimentation for faster histogram calculation in C++

## build
At repo root:
'''
mkdir build
cd build
cmake ..
make
cd ..
'''

## run
'''
cd build
./optimisation $IMAGE_WIDTH $IMAGE_HEIGHT $IMAGE_CHANNEL $IMAGE_BIT_DEPTH \
$COUNT $LOOP $THREADS
'''
