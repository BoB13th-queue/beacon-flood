#include "beacon_flood.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>     // for setw, setfill
#include <vector>
#include <string>
#include <cstring>     // for memcpy, memset
#include <cstdlib>     // for rand, srand (필요 시)
#include <unistd.h>    // for usleep (대안: this_thread::sleep_for)
#include <pcap.h>      // for pcap functions

#include "radiotap.h"
#include "beaconframe.h"
#include "tagedparam.h"
#include "beaconpacket.h"

using namespace std;

string mac_to_string(const uint8_t mac[6]) {
    ostringstream oss;
    // 1바이트째는 바로 출력
    oss << hex << setw(2) << setfill('0') << static_cast<int>(mac[0]);
    // 2~6바이트는 ":"로 구분
    for (int i = 1; i < 6; i++) {
        oss << ":" << setw(2) << setfill('0') << static_cast<int>(mac[i]);
    }
    return oss.str();
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
                        const vector<BeaconPacket> &beaconList)
{
    char errbuf[PCAP_ERRBUF_SIZE];
    // promiscuous=1, read_timeout=1000ms
    pcap_t* handle = pcap_open_live(ifname.c_str(), 1024, 1, 1000, errbuf);
    if (!handle) {
        cerr << "pcap_open_live(" << ifname << ") failed: " << errbuf << endl;
        return;
    }

    while (true) {
        for (auto &bp : beaconList) {
            // 1) 직렬화
            vector<uint8_t> packet = bp.toBytes();

            // 2) 전송
            if (pcap_sendpacket(handle, packet.data(), static_cast<int>(packet.size())) != 0) {
                cerr << "[!] pcap_sendpacket error: "
                          << pcap_geterr(handle) << endl;
            } else {
                // 디버깅/로그
                cout << "[DEBUG] Sent Beacon: BSSID="
                          << bp.beaconHdr.getBssidString()
                          << ", SSID=\"" << bp.ssid << "\"" << endl;
            }
        }

        usleep(10000);
    }

    pcap_close(handle);
}
