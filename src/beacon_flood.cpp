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

/**
 * @brief 6바이트 MAC 배열을 "xx:xx:xx:xx:xx:xx" 형태의 문자열로 변환
 */
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

/**
 * @brief 파일에서 SSID 목록을 한 줄씩 읽어 vector로 반환
 */
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

/**
 * @brief 주어진 BeaconPacket 리스트를 무한 반복 전송
 *        실제 구현 시에는 종료 조건(예: 키 입력, 신호 처리 등)을 넣어주어야 할 수도 있음
 */
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
        for (const auto &bp : beaconList) {
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

        // 10ms(0.01초) 지연 -> CPU 과부하 방지
        // C++11 방식 예시: this_thread::sleep_for(chrono::milliseconds(10));
        usleep(10000);
    }

    pcap_close(handle);
}
