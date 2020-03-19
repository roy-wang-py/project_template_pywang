
SET ( CMAKE_SYSTEM_NAME Linux )
SET ( CMAKE_SYSTEM_PROCESSOR x86_64 )
# for the reason of aarch64-linux-gnu-gcc DONOT need to be installed, make sure aarch64-linux-gnu-gcc and aarch64-linux-gnu-g++ can be found in $PATH: 
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

# set searching rules for cross-compiler
SET ( CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER )
SET ( CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY )
SET ( CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY )

# SET ( CMAKE_CXX_FLAGS "-std=c++11 -march=armv8-a  -fopenmp ${CMAKE_CXX_FLAGS}" )
SET ( CMAKE_CXX_FLAGS "-std=c++11 -march=native ${CMAKE_CXX_FLAGS}" )

# other settings
add_definitions(-DLINUX)
SET ( LINUX true)
