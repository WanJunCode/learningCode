#ifndef WANJUN_TOOL_H
#define WANJUN_TOOL_H

#include <stdio.h>

namespace Tool{

    bool isBig();
    // host network change
    unsigned long int host2Netlong(unsigned long int hostlong);
    unsigned short int host2NetShort(unsigned short int hostshort);
    unsigned long int net2Hostlong(unsigned long int netlong);
    unsigned short int net2HostShort(unsigned short int netshort);

};


#endif // !WANJUN_TOOL_H