from m5.params import *
from m5.SimObject import SimObject

class BaseCacheHierarchy(SimObject):
    type = 'BaseCacheHierarchy'
    cxx_class = 'gem5::CacheHierarchy'
    cxx_header = 'tdt4260/cache_lab/cache_impl/cache_hierarchy.hh'

    blockSize = Param.Unsigned(64, "Size (in bytes) for cache lines")

    l1CacheSize = Param.Unsigned(16384, "Size (in bytes) for L1 Cache")
    l1CacheAssociativity = Param.Unsigned(8, "Associativity for L1 Cache")

    l2CacheSize = Param.Unsigned(131072, "Size (in bytes) for L2 Cache")
    l2CacheAssociativity = Param.Unsigned(16, "Associativity for L2 Cache")

    l3CacheSize = Param.Unsigned(4194304, "Size (in bytes) for L3 Cache")
    l3CacheAssociativity = Param.Unsigned(32, "Associativity for L3 Cache")