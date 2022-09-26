Welcome to the cache lab for TDT4260!
Congratulations on passing the first test of the lab, namely reading the file named README!

The new cache lab was developed last year and uses the gem5 simulator, the leading simulator for computer architecture research.
However, we have simplified many things in order to make using it much easier. For now, all you need to care about resides in the directory under tdt4260.

To get started, look over the files in cache_impl, which make up a simple 1-level cache. This cache is defined in the file cache_hierarchy.hh and cache_hierarchy.cc
The basic principle is that we connect a CPU which is executing a given program atomically, and feed the cpuCacheInterconnect the requests. 
These requests get funneled to the L1Cache, which "services" the requests if it has the cache line, or otherwise requests it from its own L1CacheInterconnect.
In this system, the L1CacheInterconnect is directly connected to memory and memory can always service all requests. 
As you can see, the program also supports using multiple levels of caches of different sizes, but for this lab that remains commented out. 

Your first task is to complete the calculateTag and calculateIndex functions found in simple_cache.cc
Think carefully about how cache tags and indexes are calculated, including thinking about blockSize, number of sets, number of ways, and number of entries. 