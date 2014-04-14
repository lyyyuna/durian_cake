#include <iostream>
#include <vector>
#include <string>

#include <pcap.h>
#include <winsock.h>

#include "device_description.h"

using namespace std;


char * iptos(u_long in);

void getDevice(vector<string> & device_name)
{
    pcap_if_t * alldevs;
    pcap_if_t * d;
    int i = 0;
    char errbuf[PCAP_ERRBUF_SIZE];

    // retrieve the device list
    if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf) == -1) {
        cerr << "error in pcap_findalldevs_ex: " << errbuf << endl;
        // fprintf(stderr, "error in pcap_findalldevs_ex: %s\n", errbuf);
        exit(1);
    }

    // print list
    for (d = alldevs; d != NULL; d = d->next) {
        cout << i++ << endl 
             << d->name << endl;
        if (d->description)
            cout << "(" << d->description << ")" << endl;
        else
            cout << "no description" << endl;

        // record the name
        device_name.push_back(string(d->name));

        pcap_addr_t * a;
        /* IP addresses */
        for(a=d->addresses;a;a=a->next) {
  
            switch(a->addr->sa_family)
            {
            case AF_INET:
                if (a->addr)
                    cout << "\tAddress: " << iptos(((struct sockaddr_in *)a->addr)->sin_addr.s_addr) << endl;
                break;

            default:
                // 
                break;
            }
        }

        cout << endl;
    }

    if (0 == i) {
        cout << endl
             << "no device found...." << endl;

        return ;
    }

    pcap_freealldevs(alldevs);
        
}

#define IPTOSBUFFERS    12
char *iptos(u_long in)
{
    static char output[IPTOSBUFFERS][3*4+3+1];
    static short which;
    u_char *p;

    p = (u_char *)&in;
    which = (which + 1 == IPTOSBUFFERS ? 0 : which + 1);
    _snprintf_s(output[which], sizeof(output[which]), sizeof(output[which]),"%d.%d.%d.%d", p[0], p[1], p[2], p[3]);
    return output[which];
}