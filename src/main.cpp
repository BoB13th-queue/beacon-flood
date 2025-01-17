#include <iostream>
#include <cstdlib>

#include "beacon_flood.h"
#include "beaconpacket.h"

using namespace std;

static void usage() {
    cerr << "syntax : beacon-flood <interface> <ssid-list-file>\n"
         << "sample : beacon-flood mon0 ssid-list.txt\n";
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        usage();
        exit(EXIT_FAILURE);
    }

    string iface = argv[1];
    string ssid_file = argv[2];

    // SSID 목록 로드
    vector<string> ssids = load_ssid_list(ssid_file);
    if (ssids.empty()) {
        cerr << "No SSIDs loaded. Check file or content.\n";
        exit(EXIT_FAILURE);
    }

    cout << "[*] Loaded " << ssids.size() << " SSIDs from " << ssid_file << endl;
    for (const auto &ssid : ssids) {
        cout << "    - " << ssid << endl;
    }

    cout << "[*] Starting Beacon Flood on interface: " << iface << endl;
    
    vector<BeaconPacket> beaconList;
    
    start_beacon_flood(iface, beaconList);

    return 0;
}
