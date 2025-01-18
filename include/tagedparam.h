#ifndef TAGEDPARAM_H
#define TAGEDPARAM_H

#include <cstdint>
#include <vector>

using namespace std;

/**
 * @brief DS Parameter Set (채널 정보 등)
 *
 * 일반적으로 태그 번호(0x03), 길이(0x01), 그리고 채널(예: 6) 정보를 갖는다.
 * 실제 직렬화 시 바이트 배열은 [0x03, 0x01, 0x06] 형태가 된다.
 */
class DSParameterSet {
public:
    uint8_t tag_number;  // 0x03
    uint8_t length;      // 0x01
    uint8_t channel;     // 0x06

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

/**
 * @brief Supported Rates 태그
 *
 *  예) [0x01, 0x08, 0x82, 0x84, 0x8b, 0x96, 0x0c, 0x12, 0x18, 0x24]
 *   - 첫 바이트(0x01)는 태그 번호
 *   - 두 번째 바이트(0x08)는 길이
 *   - 그 뒤 8바이트가 실제 속도값들
 */
class SupportedRates {
public:
    uint8_t tag_number;           // 보통 0x01
    vector<uint8_t> rates;   // 실제 지원 속도 목록

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
