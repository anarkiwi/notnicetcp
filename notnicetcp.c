#define KBUILD_MODNAME "notnicetcp"
#include <linux/bpf.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/in.h>
#include <linux/tcp.h>


int notnicetcp(struct xdp_md *ctx) {
  void *data = (void *)(long)ctx->data;
  void *data_end = (void *)(long)ctx->data_end;
  // TODO: handle VLAN
  struct ethhdr *eth = data;
  if ((void*)eth + sizeof(*eth) <= data_end) {
    struct iphdr *ip = data + sizeof(*eth);
    const u32 opts_size = 12; // TODO
    if ((void*)ip + sizeof(*ip) + sizeof(struct tcphdr) + opts_size + 8 <= data_end) {
      unsigned int tot_len = ntohs(ip->tot_len);
      // Look for a get, and maybe break the URL.
      if (ip->protocol == IPPROTO_TCP && tot_len > 52) {
        struct tcphdr *tcp = (void*)ip + sizeof(*ip);
	unsigned char *payload = (void*)tcp + sizeof(*tcp) + opts_size;
	if (payload[0] == 'G' && payload[1] == 'E' && payload[2] == 'T') {
          bpf_trace_printk("got a GET\n");
	  u32 dice = bpf_get_prandom_u32();
	  if (dice > 2147483648) {
	    payload[4] = dice & 255;
	    bpf_trace_printk("broke the GET\n");
	  }
	}
      }
    }
  }
  return XDP_TX; // hairpin packet no matter what.
}
