#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>

#include "beacon_flood.h"
#include "beaconpacket.h"

using namespace std;

/**
 * @brief 사용 방법을 출력하고 종료
 */
static void usage() {
    cerr << "syntax : beacon-flood <interface> <ssid-list-file>\n"
        << "sample : beacon-flood mon0 ssid-list.txt\n";
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        usage();
        exit(EXIT_FAILURE);
    }

    // 명령줄 인자 파싱
    string iface     = argv[1];
    string ssid_file = argv[2];

    // SSID 목록 로드
    vector<string> ssids = load_ssid_list(ssid_file);
    if (ssids.empty()) {
        cerr << "No SSIDs loaded. Check file or content.\n";
        exit(EXIT_FAILURE);
    }

    // 로드된 SSID 정보 출력
    cout << "[*] Loaded " << ssids.size() << " SSIDs from " << ssid_file << endl;
    for (const auto &ssid : ssids) {
        cout << "    - " << ssid << endl;
    }

    cout << "[*] Starting Beacon Flood on interface: " << iface << endl;

    // BeaconPacket 리스트 생성
   vector<BeaconPacket> beaconList = BeaconPacket::generateBeaconPackets(ssids);

    // 무한 루프로 Beacon Flood 시작
    start_beacon_flood(iface, beaconList);

    return 0;
}
