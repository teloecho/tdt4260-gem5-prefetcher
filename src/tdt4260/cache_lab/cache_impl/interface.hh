#ifndef __TDT4260_INTERFACE_HH__
#define __TDT4260_INTERFACE_HH__

typedef unsigned long int uint64_t;
typedef uint64_t Addr;

class Interface
{
    public:
        virtual void recvReq(Addr req, int size) = 0;
        virtual void recvResp(Addr resp) = 0;
};

#endif //__TDT4260_INTERFACE_HH__