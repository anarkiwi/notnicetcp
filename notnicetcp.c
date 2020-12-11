// TODO: remove workaround necessary for GitHub Actions/bcc Dec 2020
#ifdef asm_inline
#undef asm_inline
#define asm_inline asm
#endif

#define KBUILD_MODNAME "notnicetcp"
#include <linux/bpf.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/in.h>
#include <linux/tcp.h>

static __always_inline struct iphdr *parseip(void *data, void *data_end) {
  struct ethhdr *eth = data;
  if ((void*)eth + sizeof(*eth) <= data_end) {
    u16 eth_type = ntohs(eth->h_proto);
    struct iphdr *ip = NULL;
    if (eth_type == ETH_P_IP) {
      return data + sizeof(*eth);
    } else if (eth_type == ETH_P_8021Q || eth_type == ETH_P_8021AD) {
      if ((void*)eth + sizeof(*eth) + sizeof(struct vlan_hdr) <= data_end) {
        struct vlan_hdr *vlan_hdr = (void*)eth + sizeof(*eth);
        eth_type = ntohs(vlan_hdr->h_vlan_encapsulated_proto);
        if (eth_type == ETH_P_IP) {
          return (void*)vlan_hdr + sizeof(*vlan_hdr);
        }
      }
    }
  }
  return NULL;
}
  
int notnicetcp(struct xdp_md *ctx) {
  void *data = (void*)(long)ctx->data;
  void *data_end = (void*)(long)ctx->data_end;
  struct iphdr *ip = parseip(data, data_end);
  if (ip) {
    if (ip->protocol == IPPROTO_TCP && (void*)ip + sizeof(*ip) + sizeof(struct tcphdr) <= data_end) {
      struct tcphdr *tcp = (void*)ip + sizeof(*ip);
      u16 tcp_header_size = tcp->doff << 2;
      u16 tot_len = ntohs(ip->tot_len);
      // Look for a GET, and maybe break the URL.
      if ((void*)tcp + tcp_header_size + 8 <= data_end && tot_len > 52) {
        unsigned char *payload = (void*)tcp + tcp_header_size;
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
