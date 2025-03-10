#ifndef BEACONFRAME_H
#define BEACONFRAME_H

#include <cstdint>
#include <vector>
#include <string>
#include <cstring>   // memcpy, memset
#include <stdexcept> // invalid_argument
#include <cstdio>    // sprintf (C 스타일 포맷 출력용)
#include <random>    // random_device, mt19937 등

using namespace std;

class BeaconFrameHeader {
private:
    /**
     * "XX:XX:XX:XX:XX:XX" 형태의 MAC 주소 문자열을 파싱하여 arr(6바이트)에 저장
     * invalid_argument 문자열 길이 or 형식이 잘못됐을 경우
     */
    void string2array(const string &str, uint8_t *arr) {
        // 예: "00:11:22:33:44:55" → 길이 17이어야 정상(각 2자리+콜론*5)
        if (str.size() != 17)
            throw invalid_argument("Invalid MAC string format: " + str);

        for (int i = 0; i < 6; i++) {
            // i*3 위치에서 2자리(16진수) 파싱
            arr[i] = static_cast<uint8_t>(
                stoi(str.substr(i * 3, 2), nullptr, 16)
            );
        }
    }

    /**
     * MAC 배열(6바이트)을 "XX:XX:XX:XX:XX:XX" 형태의 대문자 문자열로 변환
     */
    string macArrayToString(const uint8_t mac[6]) const {
        char buf[18]; // "XX:XX:XX:XX:XX:XX" + '\0' = 18
        // 대문자/소문자 취향껏
        sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X",
                     mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        return string(buf);
    }

public:
    // (A) 802.11 무선 헤더 (Management/Beacon)
    uint16_t frame_control;
    uint16_t duration;      
    uint8_t  da[6];         
    uint8_t  sa[6];         
    uint8_t  bssid[6];      
    uint16_t seq_frag;      

    // (B) Beacon Fixed Parameters
    uint64_t timestamp;        
    uint16_t beacon_interval;  
    uint16_t capability_info;  

    // 기본 생성자: 문제 예시 배열과 동일하게 초기화
    BeaconFrameHeader() {
        // Frame Control (0x8000 리틀엔디안 => 0x80,0x00)
        frame_control = 0x0080;
        duration      = 0x0000;
        // DA = ff:ff:ff:ff:ff:ff
        memset(da, 0xff, 6);
        // SA = 00:11:22:33:44:55
        setRandomBssid();
        // BSSID = SA와 동일
        memcpy(bssid, sa, 6);
        seq_frag = 0x0000;

        timestamp       = 0x0000000000000000ULL; // 8바이트
        beacon_interval = 0x0064; // (100TU)
        capability_info = 0x0431;
    }

    // 세터(Setter) 함수들
    void setBssid(const uint8_t bssid_[6]) {
        memcpy(bssid, bssid_, 6);
    }
    void setBssid(const string &bssid_) {
        string2array(bssid_, bssid);
    }

    void setRandomBssid() {
        static random_device rd;
        static mt19937 gen(rd());
        static uniform_int_distribution<> dist(0, 255);

        for (int i = 0; i < 6; i++) {
            sa[i] = static_cast<uint8_t>(dist(gen));
        }
    }

    void setSa(const uint8_t sa_[6]) {
        memcpy(sa, sa_, 6);
    }
    void setSa(const string &sa_) {
        string2array(sa_, sa);
    }

    void setDa(const uint8_t da_[6]) {
        memcpy(da, da_, 6);
    }
    void setDa(const string &da_) {
        string2array(da_, da);
    }

    // MAC 주소 문자열 get 함수
    string getBssidString() const { return macArrayToString(bssid); }
    string getSaString()    const { return macArrayToString(sa);    }
    string getDaString()    const { return macArrayToString(da);    }

    // 최종 바이트 배열로 직렬화
    vector<uint8_t> toBytes() const {
        vector<uint8_t> bytes;
        bytes.reserve(36); 
        // (A) 무선 헤더 24바이트
        // 1) frame_control, duration (각 2바이트, LE)
        bytes.push_back(static_cast<uint8_t>(frame_control & 0xff));
        bytes.push_back(static_cast<uint8_t>((frame_control >> 8) & 0xff));
        bytes.push_back(static_cast<uint8_t>(duration & 0xff));
        bytes.push_back(static_cast<uint8_t>((duration >> 8) & 0xff));

        // 2) DA, SA, BSSID (각각 6바이트)
        for (int i = 0; i < 6; i++) bytes.push_back(da[i]);
        for (int i = 0; i < 6; i++) bytes.push_back(sa[i]);
        for (int i = 0; i < 6; i++) bytes.push_back(bssid[i]);

        // 3) seq_frag (2바이트, LE)
        bytes.push_back(static_cast<uint8_t>(seq_frag & 0xff));
        bytes.push_back(static_cast<uint8_t>((seq_frag >> 8) & 0xff));

        // (B) Beacon Fixed Parameters (12바이트)
        // 1) Timestamp (8바이트, LE)
        uint64_t ts = timestamp;
        for (int i = 0; i < 8; i++) {
            bytes.push_back(static_cast<uint8_t>(ts & 0xff));
            ts >>= 8;
        }
        // 2) Beacon Interval (2바이트, LE)
        bytes.push_back(static_cast<uint8_t>(beacon_interval & 0xff));
        bytes.push_back(static_cast<uint8_t>((beacon_interval >> 8) & 0xff));
        // 3) Capability Info (2바이트, LE)
        bytes.push_back(static_cast<uint8_t>(capability_info & 0xff));
        bytes.push_back(static_cast<uint8_t>((capability_info >> 8) & 0xff));

        return bytes;
    }
};

#endif // BEACONFRAME_H
