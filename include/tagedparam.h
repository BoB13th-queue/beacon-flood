#ifndef TAGEDPARAM_H
#define TAGEDPARAM_H

#include <vector>
#include <cstdint>

using namespace std;

class DSParameterSet {
public:
    // 예: [ 0x03, 0x01, 0x06 ]
    uint8_t tag_number;  // 0x03
    uint8_t length;      // 0x01
    uint8_t channel;     // 0x06

    DSParameterSet() {
        tag_number = 0x03;
        length     = 0x01;
        channel    = 6;
    }

    vector<uint8_t> toBytes() const {
        vector<uint8_t> bytes;
        bytes.push_back(tag_number);
        bytes.push_back(length);
        bytes.push_back(channel);
        return bytes;
    }
};

class SupportedRates {
public:
    // 예: [0x01, 0x08, 0x82, 0x84, 0x8b, 0x96, 0x0c, 0x12, 0x18, 0x24]
    //  - 태그 번호(0x01), 길이(0x08), 그 뒤에 속도 값들
    //  - 여기서는 길이가 8인데 실제 배열은 9바이트라 혼동 주의
    uint8_t tag_number;
    vector<uint8_t> rates; // 실제 속도 목록

    SupportedRates() {
        tag_number = 0x01;
        // 원본: 0x01, 0x08, 0x82, 0x84, 0x8b, 0x96, 0x0c, 0x12, 0x18, 0x24
        // 여기서 '0x08'은 length이므로, rates.size() = 8 가 되어야 함
        //  -> 예) { 0x82, 0x84, 0x8b, 0x96, 0x0c, 0x12, 0x18, 0x24 }
        rates = {0x82, 0x84, 0x8b, 0x96, 0x0c, 0x12, 0x18, 0x24};
    }

    vector<uint8_t> toBytes() const {
        vector<uint8_t> bytes;
        bytes.push_back(tag_number);
        bytes.push_back(static_cast<uint8_t>(rates.size())); // length
        for (auto &r : rates) {
            bytes.push_back(r);
        }
        return bytes;
    }
};

#endif // TAGEDPARAM_H