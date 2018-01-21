#ifndef _SNIFFER_H
#define _SNIFFER_H

typedef enum _encrytion_mode {
    ENCRY_NONE           = 1,
    ENCRY_WEP,
    ENCRY_TKIP,
    ENCRY_CCMP
} ENCYTPTION_MODE;

struct RxControl {
    signed rssi:8;
    unsigned rate:4;
    unsigned is_group:1;
    unsigned:1;
    unsigned sig_mode:2;
    unsigned legacy_length:12;
    unsigned damatch0:1;
    unsigned damatch1:1;
    unsigned bssidmatch0:1;
    unsigned bssidmatch1:1;
    unsigned MCS:7;
    unsigned CWB:1;
    unsigned HT_length:16;
    unsigned Smoothing:1;
    unsigned Not_Sounding:1;
    unsigned:1;
    unsigned Aggregation:1;
    unsigned STBC:2;
    unsigned FEC_CODING:1;
    unsigned SGI:1;
    unsigned rxend_state:8;
    unsigned ampdu_cnt:8;
    unsigned channel:4;
    unsigned:12;
};
 
struct Ampdu_Info
{
  uint16 length;
  uint16 seq;
  uint8  address3[6];
};

struct sniffer_buf {
    struct RxControl rx_ctrl;
    uint8_t  buf[36];
    uint16_t cnt;
    struct Ampdu_Info ampdu_info[1];
};

struct sniffer_buf2{
    struct RxControl rx_ctrl;
    uint8 buf[112];
    uint16 cnt;
    uint16 len; //length of packet
};

struct ieee80211_frame_ctrl {
	u8 protocol_version:2;
	u8 type:2;
	u8 subtype:4;
	u8 to_ds:1;
	u8 from_ds:1;
	u8 more_fragments:1;
	u8 retry:1;
	u8 power_mgmt:1;
	u8 more_data:1;
	u8 protected_frame:1;
	u8 order:1;
};

#define ETH_ALEN 6
struct ieee80211_hdr {
	struct ieee80211_frame_ctrl frame_control;
	u16 duration_id;
	u8 addr1[ETH_ALEN];
	u8 addr2[ETH_ALEN];
	u8 addr3[ETH_ALEN];
	u16 seq_ctrl;
	u8 addr4[ETH_ALEN];
};

void promiscuous_rx_cb(uint8 *buf, uint16 buf_len);

#endif

