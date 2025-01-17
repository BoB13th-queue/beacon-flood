#include <cstdint>
#include <vector>

using namespace std;

class RadiotapHeader {
public:
    // [필드] radiotap version, pad
    uint8_t it_version; 
    uint8_t it_pad;     

    // [필드] radiotap 전체 길이 (Little Endian)
    uint16_t it_len;    

    // [필드] it_present (어떤 필드들이 들어있는지 비트로 표현)
    uint32_t it_present; 

    // [필드] 추가 옵션들 (Flags, TX flags, Padding 등)
    // 원본 예시엔 9~13번째 바이트가 아래와 같은 순서
    uint8_t  flags;    // (예: 0x02 = preamble)
    uint16_t tx_flags; // 2바이트 (low/high)
    uint16_t padding;  // 예시에서는 마지막 2바이트를 padding처럼 사용

    // 기본 생성자: 문제에서 제시된 바이트 배열에 대응하는 초기값
    RadiotapHeader() {
        it_version = 0x00;
        it_pad     = 0x00;
        it_len     = 0x000c; // 12바이트
        it_present = 0x00008004;

        flags      = 0x02;   // preamble
        tx_flags   = 0x0000;
        padding    = 0x0000;
    }

    // radiotap_header를 바이트 벡터로 직렬화
    vector<uint8_t> toBytes() const {
        vector<uint8_t> bytes;
        // 1) 버전, pad
        bytes.push_back(it_version);
        bytes.push_back(it_pad);

        // 2) it_len (Little Endian)
        bytes.push_back(static_cast<uint8_t>(it_len & 0x00ff));
        bytes.push_back(static_cast<uint8_t>((it_len >> 8) & 0x00ff));

        // 3) it_present (4바이트, Little Endian)
        bytes.push_back(static_cast<uint8_t>( it_present        & 0xff));
        bytes.push_back(static_cast<uint8_t>((it_present >> 8)  & 0xff));
        bytes.push_back(static_cast<uint8_t>((it_present >> 16) & 0xff));
        bytes.push_back(static_cast<uint8_t>((it_present >> 24) & 0xff));

        // 4) flags (1바이트)
        bytes.push_back(flags);

        // 5) tx_flags (2바이트, Little Endian)
        bytes.push_back(static_cast<uint8_t>(tx_flags & 0x00ff));
        bytes.push_back(static_cast<uint8_t>((tx_flags >> 8) & 0x00ff));

        // 6) padding (2바이트)
        bytes.push_back(static_cast<uint8_t>(padding & 0x00ff));
        bytes.push_back(static_cast<uint8_t>((padding >> 8) & 0x00ff));

        return bytes;
    }
};
