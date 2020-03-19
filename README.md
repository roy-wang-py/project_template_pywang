Build
---
- mkdir build && cd build
- cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain/x86_64.cmake  -DCMAKE_INSTALL_PREFIX=./ -DCHIP_TYPE=x86_64  ..
- cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain/arm32.cmake  -DCMAKE_INSTALL_PREFIX=./ -DCHIP_TYPE=arm32   ..
- make -j32 && make install 

---
Usage
---
- ./startup.sh

---
Info
---
- common project files tree

