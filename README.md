# netflix-cached
Netflix caching server

To build netflix_cache_server, move into the source directory and invoke 'make cache_server'. To remove any unecessary object files and the binary before rebuild run 'make clean'

To build the test_cache_client, move into the test directory and invoke 'make cache_client'. To remove objects run 'make clean' from the test directory.

Data size Limits:
This projecy only supports text commands of maximum size 2048 bytes. The key can not have control characters and is limited to a size of 250 characters.
