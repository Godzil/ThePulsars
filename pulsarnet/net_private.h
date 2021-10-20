#ifndef __PROLO_NET_PRIVATE_H__
#define __PROLO_NET_PRIVATE_H__

/*#define __DEBUG_NETLIB__*/

#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <pulsarnet.h>

void net_error(net_type_t get, net_type_t wanted);

#endif
