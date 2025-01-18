#ifndef BEACON_FLOOD_H
#define BEACON_FLOOD_H

#include <string>
#include <vector>
#include <cstdint>

class BeaconPacket;


using namespace std;

// MAC 주소(6바이트)를 "XX:XX:XX:XX:XX:XX" 형태 문자열로 변환
string mac_to_string(const uint8_t mac[6]);

// 파일로부터 SSID 목록을 불러와서 vector<string>으로 반환
vector<string> load_ssid_list(const string &filename);

// 지정된 인터페이스(ifname)로부터 beacon 패킷들을 주기적으로 방송(전송) 시작
void start_beacon_flood(const string &ifname,
                        const vector<BeaconPacket> &beaconList);

#endif // BEACON_FLOOD_H
