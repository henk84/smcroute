/* Physical and virtual interface API */
#ifndef SMCROUTE_IFACE_H_
#define SMCROUTE_IFACE_H_

#include "config.h"

#include <stdint.h>
#include <net/if.h>			/* IFNAMSIZ */
#include <netinet/in.h>			/* struct in_addr */

#define DEFAULT_THRESHOLD 1		/* Packet TTL must be at least 1 to pass */

struct iface {
	char name[IFNAMSIZ];
	struct in_addr inaddr;		/* == 0 for non IP interfaces */
	int   ifindex;			/* Physical interface index   */
	short flags;
	short vif;
	short mif;
	uint8_t mrdisc;			/* Enable multicast router discovery */
	uint8_t threshold;		/* TTL threshold: 1-255, default: 1 */
};

struct ifmatch {
	unsigned int iter;
	unsigned int match_count;
};

void          iface_init              (void);
void          iface_exit              (void);

struct iface *iface_iterator          (int first);

struct iface *iface_find              (int ifindex);
struct iface *iface_find_by_name      (const char *ifname);
struct iface *iface_find_by_vif       (int vif);

void          iface_match_init        (struct ifmatch *state);
struct iface *iface_match_by_name     (const char *ifname, struct ifmatch *state);
int           ifname_is_wildcard      (const char *ifname);

int           iface_get_vif           (struct iface *iface);
int           iface_get_mif           (struct iface *iface);

int           iface_match_vif_by_name (const char *ifname, struct ifmatch *state, struct iface **found);
int           iface_match_mif_by_name (const char *ifname, struct ifmatch *state, struct iface **found);

int           iface_show              (int sd, int detail);

static inline int iface_exist(char *ifname)
{
	struct ifmatch ifm;

	iface_match_init(&ifm);
	return iface_match_by_name(ifname, &ifm) != NULL;
}

#endif /* SMCROUTE_IFACE_H_ */

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */