using namespace std;

#ifndef _ARP_H
#define _ARP_H

#define ETH_IP              0x0800 
#define ETH_ARP             0x0806 
#define ARP_REQUEST         0x0001 // arp请求包 
#define ARP_REPLY           0x0002 // arp应答包 
#define ARP_HARDWARE        0x0001 
// #define max_num_adapter     10 


#pragma pack(push, 1)

struct ethhdr
{
    u_char eh_dst[6];       // 以太网目的地址 
    u_char eh_src[6];       // 以太网源地址
    u_short eh_type;
};

struct arphdr
{
    u_short arp_hdr; // 硬件类型 
    u_short arp_pro; // 协议类型 
    u_char arp_hln; // 硬件地址长度 
    u_char arp_pln; // 协议地址长度 
    u_short arp_opt; // 
    u_char arp_sha[6]; // 发送端以太网地址 
    u_long arp_spa; // 发送端ip地址 
    u_char arp_tha[6]; // 接收端以太网地址 
    u_long arp_tpa; // 接收端ip地址 
};

struct ip_mac
{
    u_long ip;
    u_char mac[6];
};

#pragma pack(push)


void getmac(pcap_t * fp, ip_mac & dst_ipmac, string target, string gateway);
void sendARPReply(pcap_t * fp, ip_mac & dst_ipmac, string gateway);

#endif