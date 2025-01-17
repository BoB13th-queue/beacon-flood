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
    RadiotapHeader     radiotap;
    BeaconFrameHeader  beaconHdr;
    DSParameterSet     ds;
    SupportedRates     rates;

    // 다른 태그나 RSN/WPA 태그를 추가로 넣을 수도 있음
    // ex) RsnParameter rsn;
    // ex) VendorSpecificTag vendor;

    // SSID는 태그에 직접 넣어야 하므로, 여기서는 임의로 보관
    string ssid;

    // 최종 패킷 직렬화
    vector<uint8_t> toBytes() const {
        vector<uint8_t> packet;

        // 1) Radiotap
        vector<uint8_t> rbytes = radiotap.toBytes();
        packet.insert(packet.end(), rbytes.begin(), rbytes.end());

        // 2) 802.11 Beacon Frame
        vector<uint8_t> bbytes = beaconHdr.toBytes();
        packet.insert(packet.end(), bbytes.begin(), bbytes.end());

        // 3) SSID 태그
        packet.push_back(0x00); // Tag (SSID)
        packet.push_back(static_cast<uint8_t>(ssid.size()));
        for (char c : ssid) {
            packet.push_back(static_cast<uint8_t>(c));
        }

        // 4) DSParam, SupportedRates 등
        auto ds_bytes    = ds.toBytes();
        auto rates_bytes = rates.toBytes();
        packet.insert(packet.end(), ds_bytes.begin(), ds_bytes.end());
        packet.insert(packet.end(), rates_bytes.begin(), rates_bytes.end());

        return packet;
    }
};

#endif // BEACONPACKET_H