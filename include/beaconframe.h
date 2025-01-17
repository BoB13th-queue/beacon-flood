#include <cstdint>
#include <vector>
#include <cstring> // memcpy 등

class BeaconFrameHeader {
public:
    // (A) 802.11 무선 헤더 (Management/Beacon)
    uint16_t frame_control;    // ex: 0x0080 (리틀엔디안으로는 0x80, 0x00)
    uint16_t duration;         // 0x0000
    uint8_t  da[6];            // 기본적으로 ff:ff:ff:ff:ff:ff (Broadcast)
    uint8_t  sa[6];            // 00:11:22:33:44:55 (예시)
    uint8_t  bssid[6];         // 00:11:22:33:44:55 (예시)
    uint16_t seq_frag;         // 0x0000

    // (B) Beacon Fixed Parameters
    uint64_t timestamp;        // 8바이트 (일반적으로 0)
    uint16_t beacon_interval;  // 0x0064
    uint16_t capability_info;  // 0x0431 (혹은 0x0431, 문제 예시에선 0x0431 = 0x31,0x04)

    // 기본 생성자: 문제 예시 배열과 동일하게 초기화
    BeaconFrameHeader() {
        // Frame Control (0x8000 리틀엔디안 => 0x80,0x00)
        frame_control = 0x0080;
        duration      = 0x0000;
        // DA = ff:ff:ff:ff:ff:ff
        memset(da, 0xff, 6);
        // SA = 00:11:22:33:44:55
        sa[0] = 0x00; sa[1] = 0x11; sa[2] = 0x22;
        sa[3] = 0x33; sa[4] = 0x44; sa[5] = 0x55;
        // BSSID = 00:11:22:33:44:55
        memcpy(bssid, sa, 6);
        seq_frag = 0x0000;

        timestamp       = 0x0000000000000000ULL; // 8바이트
        beacon_interval = 0x0064; // (100TU)
        capability_info = 0x0431; // (문제 예시 = 0x31,0x04 -> 리틀엔디안 0x0431)
    }

    // BSSID, SA, DA 등을 바꾸고 싶다면 세터 함수를 제공하면 됨
    void setBssid(const uint8_t bssid_[6]) { memcpy(bssid, bssid_, 6); }
    void setSa(const uint8_t sa_[6])       { memcpy(sa, sa_, 6);       }
    void setDa(const uint8_t da_[6])       { memcpy(da, da_, 6);       }

    // 최종 바이트 배열로 직렬화
    std::vector<uint8_t> toBytes() const {
        std::vector<uint8_t> bytes;
        bytes.reserve(36);

        // (A) 무선 헤더 24바이트
        // 1) frame_control, duration (각각 2바이트, Little Endian)
        bytes.push_back(static_cast<uint8_t>(frame_control & 0xff));
        bytes.push_back(static_cast<uint8_t>((frame_control >> 8) & 0xff));
        bytes.push_back(static_cast<uint8_t>(duration & 0xff));
        bytes.push_back(static_cast<uint8_t>((duration >> 8) & 0xff));

        // 2) DA, SA, BSSID (각각 6바이트)
        for (int i = 0; i < 6; i++) bytes.push_back(da[i]);
        for (int i = 0; i < 6; i++) bytes.push_back(sa[i]);
        for (int i = 0; i < 6; i++) bytes.push_back(bssid[i]);

        // 3) seq_frag (2바이트, Little Endian)
        bytes.push_back(static_cast<uint8_t>(seq_frag & 0xff));
        bytes.push_back(static_cast<uint8_t>((seq_frag >> 8) & 0xff));

        // (B) Beacon Fixed Parameters (12바이트)
        // 1) Timestamp (8바이트, 리틀엔디안)
        uint64_t ts = timestamp;
        for (int i = 0; i < 8; i++) {
            bytes.push_back(static_cast<uint8_t>(ts & 0xff));
            ts >>= 8;
        }

        // 2) Beacon Interval (2바이트)
        bytes.push_back(static_cast<uint8_t>(beacon_interval & 0xff));
        bytes.push_back(static_cast<uint8_t>((beacon_interval >> 8) & 0xff));

        // 3) Capability Info (2바이트)
        bytes.push_back(static_cast<uint8_t>(capability_info & 0xff));
        bytes.push_back(static_cast<uint8_t>((capability_info >> 8) & 0xff));

        return bytes;
    }
};
