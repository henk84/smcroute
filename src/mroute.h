/* Generic kernel multicast routing API for Linux and *BSD */
#ifndef SMCROUTE_MROUTE_H_
#define SMCROUTE_MROUTE_H_

#include "config.h"
#include <stdint.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>

#include "queue.h"		/* Needed by netinet/ip_mroute.h on FreeBSD */


#ifdef HAVE_LINUX_MROUTE_H
#define _LINUX_IN_H             /* For Linux <= 2.6.25 */
#include <linux/types.h>
#include <linux/mroute.h>
#endif

#ifdef HAVE_LINUX_MROUTE6_H
#include <linux/mroute6.h>
#endif

#ifdef HAVE_LINUX_FILTER_H
#include <linux/filter.h>
#endif

#ifdef HAVE_NET_ROUTE_H
#include <net/route.h>
#endif

#ifdef HAVE_NETINET_IP_MROUTE_H
#define _KERNEL
#include <netinet/ip_mroute.h>
#undef _KERNEL
#else
# ifdef __APPLE__
#  include "ip_mroute.h"
# endif
#endif

#ifdef HAVE_NETINET6_IP6_MROUTE_H
#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#include <netinet6/ip6_mroute.h>
#endif

#ifndef IN6_IS_ADDR_MULTICAST
#define IN6_IS_ADDR_MULTICAST(a) (((__const uint8_t *) (a))[0] == 0xff)
#endif

#include "inet.h"

/*
 * IPv4 multicast route
 */
#ifndef MAXVIFS
#define MAXVIFS 32
#endif

struct mroute {
	LIST_ENTRY(mroute) link;

	inet_addr_t        source;	/* originating host, may be inet_anyaddr() */
	short              src_len;     /* source prefix len, or 0:disabled */

	inet_addr_t        group;       /* multicast group */
	short              len;		/* prefix len, or 0:disabled */

	uint16_t           inbound;     /* incoming VIF    */
	uint8_t            ttl[MAXVIFS];/* outgoing VIFs   */

	unsigned long      valid_pkt;   /* packet counter at last mroute4_dyn_expire() */
	time_t             last_use;    /* timestamp of last forwarded packet */
};

/*
 * IPv6 multicast route
 */
#ifdef HAVE_IPV6_MULTICAST_ROUTING
# ifndef MAXMIFS
# define MAXMIFS MAXVIFS
# endif

# if MAXMIFS != MAXVIFS
# error "IPv6 MAXMIFS constants does not match IPv4 MAXVIFS, mroute.h or mroute6.h needs to be fixed!"
# endif
#endif

int  mroute4_dyn_add   (struct mroute *mroute);
void mroute4_dyn_expire(int max_idle);
int  mroute4_add       (struct mroute *mroute);
int  mroute4_del       (struct mroute *mroute);

int  mroute6_dyn_add   (struct mroute *mroute);
int  mroute6_add       (struct mroute *mroute);
int  mroute6_del       (struct mroute *mroute);

int  mroute_add_vif    (char *ifname, uint8_t mrdisc, uint8_t threshold);
int  mroute_del_vif    (char *ifname);

int  mroute_init       (int do_vifs, int table_id, int cache_tmo);
void mroute_exit       (int close_socket);

int  mroute_show       (int sd, int detail);

#endif /* SMCROUTE_MROUTE_H_ */
