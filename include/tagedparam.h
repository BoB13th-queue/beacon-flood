#ifndef TAGEDPARAM_H
#define TAGEDPARAM_H

#include <cstdint>
#include <vector>

using namespace std;

class DSParameterSet {
public:
    uint8_t tag_number;  
    uint8_t length;      
    uint8_t channel;     

    DSParameterSet()
        : tag_number(0x03),
          length(0x01),
          channel(6)
    {
    }

    // 직렬화
    vector<uint8_t> toBytes() const {
        vector<uint8_t> bytes;
        bytes.reserve(3); // tag_number + length + channel = 3바이트
        bytes.push_back(tag_number);
        bytes.push_back(length);
        bytes.push_back(channel);
        return bytes;
    }
};

class SupportedRates {
public:
    uint8_t tag_number;      
    vector<uint8_t> rates;   

    SupportedRates()
        : tag_number(0x01),
          rates{0x82, 0x84, 0x8b, 0x96, 0x0c, 0x12, 0x18, 0x24}
    {
    }

    // 직렬화
    vector<uint8_t> toBytes() const {
        vector<uint8_t> bytes;
        bytes.reserve(2 + rates.size()); // (tag_number, length) + rates
        bytes.push_back(tag_number);
        bytes.push_back(static_cast<uint8_t>(rates.size())); // length
        // 실제 속도값들
        bytes.insert(bytes.end(), rates.begin(), rates.end());
        return bytes;
    }
};

#endif // TAGEDPARAM_H
