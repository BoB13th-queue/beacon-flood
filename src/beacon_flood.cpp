#include "beacon_flood.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unistd.h>   // for usleep
#include <pcap.h>

using namespace std;

vector<string> load_ssid_list(const string &filename) {
    vector<string> ssids;
    ifstream ifs(filename);
    if (!ifs.is_open()) {
        cerr << "Failed to open SSID list file: " << filename << endl;
        return ssids;
    }

    string line;
    while (getline(ifs, line)) {
        if (!line.empty()) {
            ssids.push_back(line);
        }
    }
    return ssids;
}

void start_beacon_flood(const string &ifname, const vector<string> &ssids) {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t* handle = pcap_open_live(ifname.c_str(), 1024, 1, 1000, errbuf);
    if (!handle) {
        cerr << "pcap_open_live(" << ifname << ") failed: " << errbuf << endl;
        return;
    }


    pcap_close(handle);
}
