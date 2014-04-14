using namespace std;

#ifndef _ARP_H
#define _ARP_H

#define ETH_IP              0x0800 
#define ETH_ARP             0x0806 
#define ARP_REQUEST         0x0001 // arp����� 
#define ARP_REPLY           0x0002 // arpӦ��� 
#define ARP_HARDWARE        0x0001 
// #define max_num_adapter     10 


#pragma pack(push, 1)

struct ethhdr
{
    u_char eh_dst[6];       // ��̫��Ŀ�ĵ�ַ 
    u_char eh_src[6];       // ��̫��Դ��ַ
    u_short eh_type;
};

struct arphdr
{
    u_short arp_hdr; // Ӳ������ 
    u_short arp_pro; // Э������ 
    u_char arp_hln; // Ӳ����ַ���� 
    u_char arp_pln; // Э���ַ���� 
    u_short arp_opt; // 
    u_char arp_sha[6]; // ���Ͷ���̫����ַ 
    u_long arp_spa; // ���Ͷ�ip��ַ 
    u_char arp_tha[6]; // ���ն���̫����ַ 
    u_long arp_tpa; // ���ն�ip��ַ 
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