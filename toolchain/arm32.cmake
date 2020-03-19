
SET ( CMAKE_SYSTEM_NAME Linux )
SET ( CMAKE_SYSTEM_PROCESSOR armv7 )
# for the reason of aarch64-linux-gnu-gcc DONOT need to be installed, make sure aarch64-linux-gnu-gcc and aarch64-linux-gnu-g++ can be found in $PATH: 
set(CMAKE_C_COMPILER /home/roywang/share/cross_compile/raspberry-cross/bin/armv7-rpi4-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER /home/roywang/share/cross_compile/raspberry-cross/bin/armv7-rpi4-linux-gnueabihf-g++)

# set searching rules for cross-compiler
SET ( CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER )
SET ( CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY )
SET ( CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY )

# SET ( CMAKE_CXX_FLAGS "-std=c++11 -march=armv8-a  -fopenmp ${CMAKE_CXX_FLAGS}" )
SET ( CMAKE_CXX_FLAGS "-std=c++11 -march=armv7 -Wno-psabi ${CMAKE_CXX_FLAGS}" )

# other settings
add_definitions(-DLINUX)
SET ( LINUX true)
