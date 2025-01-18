#ifndef BEACONPACKET_H
#define BEACONPACKET_H

#include <cstdint>
#include <vector>
#include <string>

#include "radiotap.h"       // RadiotapHeader
#include "beaconframe.h"    // BeaconFrameHeader
#include "tagedparam.h"     // DSParameterSet, SupportedRates, etc

using namespace std;

struct BeaconPacket {
    RadiotapHeader    radiotap;
    BeaconFrameHeader beaconHdr;
    DSParameterSet    ds;
    SupportedRates    rates;

    // SSID는 무선 프레임 태그(SSID Tag)에 직접 들어가므로,
    // 여기서는 저장만 하고 직렬화할 때 실제 태그로 만듦
    string ssid;

    // 최종 패킷 직렬화
    vector<uint8_t> toBytes() const {
        vector<uint8_t> packet;

        // 1) Radiotap 헤더 직렬화
        vector<uint8_t> rbytes = radiotap.toBytes();
        packet.insert(packet.end(), rbytes.begin(), rbytes.end());

        // 2) 802.11 Beacon Frame 직렬화
        vector<uint8_t> bbytes = beaconHdr.toBytes();
        packet.insert(packet.end(), bbytes.begin(), bbytes.end());

        // 3) SSID 태그
        //    - Tag ID = 0x00
        //    - Tag Length = ssid.size()
        //    - Tag Data = SSID 문자열
        packet.push_back(0x00); // Tag ID (SSID)
        packet.push_back(static_cast<uint8_t>(ssid.size()));
        for (char c : ssid) {
            packet.push_back(static_cast<uint8_t>(c));
        }

        // 4) DSParam, SupportedRates 등 다른 태그
        vector<uint8_t> ds_bytes    = ds.toBytes();
        vector<uint8_t> rates_bytes = rates.toBytes();

        packet.insert(packet.end(), ds_bytes.begin(), ds_bytes.end());
        packet.insert(packet.end(), rates_bytes.begin(), rates_bytes.end());

        return packet;
    }

    // 여러 SSID에 대해 BeaconPacket 리스트를 생성하는 헬퍼 함수
    static vector<BeaconPacket> generateBeaconPackets(const vector<string> &ssids) {
        vector<BeaconPacket> packets;
        packets.reserve(ssids.size()); // 메모리 효율(선택사항)

        for (const auto &ssid : ssids) {
            BeaconPacket bp;
            bp.ssid = ssid;
            packets.push_back(bp);
        }
        return packets;
    }
};

#endif // BEACONPACKET_H
