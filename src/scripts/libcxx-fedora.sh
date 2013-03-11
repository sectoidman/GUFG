#!/bin/bash

su -c "yum install libstdc++-static clang"
git clone git://github.com/llvm-mirror/libcxx.git
mkdir libcxx-build; cd libcxx-build

stdcxx="/usr/include/c++/$(gcc --version | head -n 1 | cut -d ")" -f 2 | cut -d " " -f 2)"
stdcxxpaths="$stdcxx;$stdcxx/x86_64-redhat-linux"
echo $stdcxxpaths
CC=clang \
CXX=clang++ \
cmake -DLIBCXX_CXX_ABI=libsupc++ \
      -DLIBCXX_LIBSUPCXX_INCLUDE_PATHS=$stdcxxpaths\
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_INSTALL_PREFIX=/usr/local \
      ../libcxx
make
su -c "make install"
