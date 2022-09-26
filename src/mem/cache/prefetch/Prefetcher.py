# Copyright (c) 2012, 2014, 2019 ARM Limited
# All rights reserved.
#
# The license below extends only to copyright in the software and shall
# not be construed as granting a license to any other intellectual
# property including but not limited to intellectual property relating
# to a hardware implementation of the functionality of the software
# licensed hereunder.  You may use the software subject to the license
# terms below provided that you ensure that this notice is replicated
# unmodified and in its entirety in all distributions of the software,
# modified or unmodified, in source code or in binary form.
#
# Copyright (c) 2005 The Regents of The University of Michigan
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met: redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer;
# redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the distribution;
# neither the name of the copyright holders nor the names of its
# contributors may be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

from m5.SimObject import *
from m5.params import *
from m5.proxy import *

from m5.objects.ClockedObject import ClockedObject
from m5.objects.IndexingPolicies import *
from m5.objects.ReplacementPolicies import *

class HWPProbeEvent(object):
    def __init__(self, prefetcher, obj, *listOfNames):
        self.obj = obj
        self.prefetcher = prefetcher
        self.names = listOfNames

    def register(self):
        if self.obj:
            for name in self.names:
                self.prefetcher.getCCObject().addEventProbe(
                    self.obj.getCCObject(), name)

class BasePrefetcher(ClockedObject):
    type = 'BasePrefetcher'
    abstract = True
    cxx_class = 'gem5::prefetch::Base'
    cxx_header = "mem/cache/prefetch/base.hh"
    cxx_exports = [
        PyBindMethod("addEventProbe"),
        PyBindMethod("addTLB"),
    ]
    sys = Param.System(Parent.any, "System this prefetcher belongs to")

    # Get the block size from the parent (system)
    block_size = Param.Int(Parent.cache_line_size, "Block size in bytes")

    on_miss = Param.Bool(False, "Only notify prefetcher on misses")
    on_read = Param.Bool(True, "Notify prefetcher on reads")
    on_write = Param.Bool(True, "Notify prefetcher on writes")
    on_data  = Param.Bool(True, "Notify prefetcher on data accesses")
    on_inst  = Param.Bool(True, "Notify prefetcher on instruction accesses")
    prefetch_on_access = Param.Bool(Parent.prefetch_on_access,
        "Notify the hardware prefetcher on every access (not just misses)")
    prefetch_on_pf_hit = Param.Bool(Parent.prefetch_on_pf_hit,
        "Notify the hardware prefetcher on hit on prefetched lines")
    use_virtual_addresses = Param.Bool(False,
        "Use virtual addresses for prefetching")
    page_bytes = Param.MemorySize('4KiB',
            "Size of pages for virtual addresses")

    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self._events = []
        self._tlbs = []

    def addEvent(self, newObject):
        self._events.append(newObject)

    # Override the normal SimObject::regProbeListeners method and
    # register deferred event handlers.
    def regProbeListeners(self):
        for tlb in self._tlbs:
            self.getCCObject().addTLB(tlb.getCCObject())
        for event in self._events:
            event.register()
        self.getCCObject().regProbeListeners()

    def listenFromProbe(self, simObj, *probeNames):
        if not isinstance(simObj, SimObject):
            raise TypeError("argument must be of SimObject type")
        if len(probeNames) <= 0:
            raise TypeError("probeNames must have at least one element")
        self.addEvent(HWPProbeEvent(self, simObj, *probeNames))

    def registerTLB(self, simObj):
        if not isinstance(simObj, SimObject):
            raise TypeError("argument must be a SimObject type")
        self._tlbs.append(simObj)

class MultiPrefetcher(BasePrefetcher):
    type = 'MultiPrefetcher'
    cxx_class = 'gem5::prefetch::Multi'
    cxx_header = 'mem/cache/prefetch/multi.hh'

    prefetchers = VectorParam.BasePrefetcher([], "Array of prefetchers")

class QueuedPrefetcher(BasePrefetcher):
    type = "QueuedPrefetcher"
    abstract = True
    cxx_class = 'gem5::prefetch::Queued'
    cxx_header = "mem/cache/prefetch/queued.hh"
    latency = Param.Int(1, "Latency for generated prefetches")
    queue_size = Param.Int(32, "Maximum number of queued prefetches")
    max_prefetch_requests_with_pending_translation = Param.Int(32,
        "Maximum number of queued prefetches that have a missing translation")
    queue_squash = Param.Bool(True, "Squash queued prefetch on demand access")
    queue_filter = Param.Bool(True, "Don't queue redundant prefetches")
    cache_snoop = Param.Bool(False, "Snoop cache to eliminate redundant request")

    tag_prefetch = Param.Bool(True, "Tag prefetch with PC of generating access")

    # The throttle_control_percentage controls how many of the candidate
    # addresses generated by the prefetcher will be finally turned into
    # prefetch requests
    # - If set to 100, all candidates can be discarded (one request
    #   will always be allowed to be generated)
    # - Setting it to 0 will disable the throttle control, so requests are
    #   created for all candidates
    # - If set to 60, 40% of candidates will generate a request, and the
    #   remaining 60% will be generated depending on the current accuracy
    throttle_control_percentage = Param.Percent(0, "Percentage of requests \
        that can be throttled depending on the accuracy of the prefetcher.")

class StridePrefetcherHashedSetAssociative(SetAssociative):
    type = 'StridePrefetcherHashedSetAssociative'
    cxx_class = 'gem5::prefetch::StridePrefetcherHashedSetAssociative'
    cxx_header = "mem/cache/prefetch/stride.hh"

class StridePrefetcher(QueuedPrefetcher):
    type = 'StridePrefetcher'
    cxx_class = 'gem5::prefetch::Stride'
    cxx_header = "mem/cache/prefetch/stride.hh"

    # Do not consult stride prefetcher on instruction accesses
    on_inst = False

    confidence_counter_bits = Param.Unsigned(3,
        "Number of bits of the confidence counter")
    initial_confidence = Param.Unsigned(4,
        "Starting confidence of new entries")
    confidence_threshold = Param.Percent(50,
        "Prefetch generation confidence threshold")

    use_requestor_id = Param.Bool(True, "Use requestor id based history")

    degree = Param.Int(4, "Number of prefetches to generate")

    table_assoc = Param.Int(4, "Associativity of the PC table")
    table_entries = Param.MemorySize("64", "Number of entries of the PC table")
    table_indexing_policy = Param.BaseIndexingPolicy(
        StridePrefetcherHashedSetAssociative(entry_size = 1,
        assoc = Parent.table_assoc, size = Parent.table_entries),
        "Indexing policy of the PC table")
    table_replacement_policy = Param.BaseReplacementPolicy(RandomRP(),
        "Replacement policy of the PC table")

    def listenFromProbeRetiredInstructions(self, simObj):
        if not isinstance(simObj, SimObject):
            raise TypeError("argument must be of SimObject type")
        self.addEvent(HWPProbeEventRetiredInsts(self, simObj,"RetiredInstsPC"))

class TDTPrefetcherHashedSetAssociative(SetAssociative):
    type = 'TDTPrefetcherHashedSetAssociative'
    cxx_class = 'gem5::prefetch::TDTPrefetcherHashedSetAssociative'
    cxx_header = "mem/cache/prefetch/tdt_prefetcher.hh"

class TDTPrefetcher(QueuedPrefetcher):
    type = 'TDTPrefetcher'
    cxx_class = 'gem5::prefetch::TDTPrefetcher'
    cxx_header = "mem/cache/prefetch/tdt_prefetcher.hh"

    table_assoc = Param.Int(4, "Assocaitivity of the PC table")
    table_entries = Param.MemorySize("64", "Number of entries of the PC table")
    table_indexing_policy = Param.BaseIndexingPolicy(
        TDTPrefetcherHashedSetAssociative(entry_size = 1,
        assoc = Parent.table_assoc, size = Parent.table_entries),
        "Indexing policy of the PC table")

    table_replacement_policy = Param.BaseReplacementPolicy(RandomRP(),
        "Replacement policy of the PC table")

