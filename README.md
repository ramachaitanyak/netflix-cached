# netflix-cached
Netflix caching server

=== Build Instructions ===
To build netflix_cache_server, move into the source directory and invoke 'make cache_server'. To remove any unecessary object files and the binary before rebuild run 'make clean'

To build the test_cache_client, move into the test directory and invoke 'make cache_client'. To remove objects run 'make clean' from the test directory.

NB: 'test_cache_client' only works when there is a 'netflix_cache_server' running. To run tests, please run 'netflix_cache_server' first before running 'test_cache_client'

=== Data size Limits ===
This project only supports text commands of maximum size 2048 bytes. The key can not have control characters and is limited to a size of 250 characters.

=== Compaitbility, Development and Test Environment ===

vm-2997079:/home/rkavuluru/Craft/netflix-cached# uname -a
Linux vm-2997079 4.4.0-143-generic #169-Ubuntu SMP Thu Feb 7 07:56:38 UTC 2019 x86_64 x86_64 x86_64 GNU/Linux

vm-2997079:/home/rkavuluru/Craft/netflix-cached# g++ -v
Using built-in specs.
COLLECT_GCC=g++
COLLECT_LTO_WRAPPER=/usr/lib/gcc/x86_64-linux-gnu/7/lto-wrapper
OFFLOAD_TARGET_NAMES=nvptx-none
OFFLOAD_TARGET_DEFAULT=1
Target: x86_64-linux-gnu
Configured with: ../src/configure -v --with-pkgversion='Ubuntu 7.4.0-1ubuntu1~16.04~ppa1' --with-bugurl=file:///usr/share/doc/gcc-7/README.Bugs --enable-languages=c,ada,c++,go,brig,d,fortran,objc,obj-c++ --prefix=/usr --with-gcc-major-version-only --program-suffix=-7 --program-prefix=x86_64-linux-gnu- --enable-shared --enable-linker-build-id --libexecdir=/usr/lib --without-included-gettext --enable-threads=posix --libdir=/usr/lib --enable-nls --with-sysroot=/ --enable-clocale=gnu --enable-libstdcxx-debug --enable-libstdcxx-time=yes --with-default-libstdcxx-abi=new --enable-gnu-unique-object --disable-vtable-verify --enable-libmpx --enable-plugin --with-system-zlib --with-target-system-zlib --enable-objc-gc=auto --enable-multiarch --disable-werror --with-arch-32=i686 --with-abi=m64 --with-multilib-list=m32,m64,mx32 --enable-multilib --with-tune=generic --enable-offload-targets=nvptx-none --without-cuda-driver --enable-checking=release --build=x86_64-linux-gnu --host=x86_64-linux-gnu --target=x86_64-linux-gnu
Thread model: posix
gcc version 7.4.0 (Ubuntu 7.4.0-1ubuntu1~16.04~ppa1) 

NB: The minimum requirement to build the project is to have a gcc version of 7 or higher; please update g++ if running into any errors
