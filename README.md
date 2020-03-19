# netflix-cached
Netflix Caching Server

## Build Instructions
To build netflix_cache_server, move into the source directory and invoke 'make cache_server'. To remove any unecessary object files and the binary before rebuild run 'make clean'

To build the test_cache_client, move into the test directory and invoke 'make cache_client'. To remove objects run 'make clean' from the test directory.

NB: 'test_cache_client' only works when there is a 'netflix_cache_server' running. To run tests, please run 'netflix_cache_server' first before running 'test_cache_client'

## Data size Limits
This project only supports text commands of maximum size 2048 bytes. The key can not have control characters and is limited to a size of 250 characters.

## Compaitbility, Development and Test Environment
Linux Version
```
Linux vm-2997079 4.4.0-143-generic #169-Ubuntu SMP Thu Feb 7 07:56:38 UTC 2019 x86_64 x86_64 x86_64 GNU/Linux
```
g++ compiler version
```
Thread model: posix
gcc version 7.4.0 (Ubuntu 7.4.0-1ubuntu1~16.04~ppa1) 
```


NB: The minimum requirement to build the project is to have a gcc version of 7 or higher; please update g++ if running into any errors
