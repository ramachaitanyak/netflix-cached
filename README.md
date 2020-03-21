# netflix-cached
A memcached like caching server

## Introduction
The goal of this program is to create a high-performance concurrent memcached-like server.
For the scope of this program, the following simplifications are made from a full implementation:
```
• Supports only the memcached text protocol
• Supports only the get and set operations
 * The flags, exptime, and (optional) noreply parameters are accepted but not used
• The only required configuration is for the TCP listening port or listen on 11211 port by default
• The get command can be for one key only if desired
```

## Relevant Document References

* [Design and Functional specification Document](https://docs.google.com/document/d/1-re6c37zJBwAJtJ73bm5GlXK9Eqzm6wE3yluSmz2J2g/edit?usp=sharing)
* [Functional and Requirement Test Document](https://docs.google.com/document/d/1-re6c37zJBwAJtJ73bm5GlXK9Eqzm6wE3yluSmz2J2g/edit?usp=sharing)
* [Continuous Integration Pipeline for Maintainability](https://docs.google.com/document/d/1-re6c37zJBwAJtJ73bm5GlXK9Eqzm6wE3yluSmz2J2g/edit?usp=sharing)


## Build & Test Instructions
```
* To build `netflix_cache_server`
  - cd ./source
  - make cache_server
* To remove any unecessary object files and the binary before rebuild
  - cd ./source
  - make clean

* To build the test_cache_client
  - cd ./test
  - make cache_client 
* To remove test client
  - cd ./test
  - make clean
```

> NB 1: `test_cache_client` only works when there is a `netflix_cache_server` running. To run tests, please run `netflix_cache_server` first before running `test_cache_client`

> NB 2: The release versions of `netflix_cache_server` and `test_cache_client` are compiled and pre-built for use

Usage
```
* Start cache server
  - ./release/netflix_cache_server [optional-port]
* Start the test client within 20 seconds on starting cache server
  - ./release/test_cache_client [optional-port]
```

## Data Size Limits
This project only supports text commands of maximum size 5120 bytes. The key can not have control characters and is limited to a size of 250 characters.

## Performance Measurments
Tool used : `'memcslap' from package 'libmemcached-tools'` on Ubuntu Linux [memcslap](https://manpages.debian.org/jessie/libmemcached-tools/memcslap.1.en.html)
```
Usage
memcslap -s 127.0.0.1:11211 --test get/set

10000 sets of 4096 bytes of unstrcutured data took 0.717 seconds approximately
10000 gets on those keys took 0.257 seconds approximately
```

## Compatibility, Development and Test Environment
Linux Version
```
Linux vm-2997079 4.4.0-143-generic #169-Ubuntu SMP Thu Feb 7 07:56:38 UTC 2019 x86_64 x86_64 x86_64 GNU/Linux
```
g++ compiler version
```
Thread model: posix
gcc version 7.4.0 (Ubuntu 7.4.0-1ubuntu1~16.04~ppa1)
```
> NB: The minimum requirement to build the project is to have a gcc version of 7 or higher; please update g++ if running into any errors

[Upgrade Instructions](https://tuxamito.com/wiki/index.php/Installing_newer_GCC_versions_in_Ubuntu)

## References
xxHash - Non cryptographic hashing algorithm used for hashing
* https://github.com/Cyan4973/xxHash
* https://github.com/RedSpah/xxhash_cpp (xxHash for cpp)

Memcached text protocol
* https://github.com/memcached/memcached/blob/master/doc/protocol.txt
* https://docs.oracle.com/cd/E17952_01/mysql-5.0-en/ha-memcached-interfaces-protocol.html
* https://docs.oracle.com/cd/E17952_01/mysql-5.0-en/ha-memcached-interfaces-protocol.html#ha-memcached-interfaces-protocol-responses

Thread pool design patterns
* https://ncona.com/2019/05/using-thread-pools-in-cpp/

C++ Draft
* http://eel.is/c++draft/ 
