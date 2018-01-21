#include "esp_common.h"
#include "sniffer.h"

void ICACHE_FLASH_ATTR
sniffer_wifi_promiscuous_rx(uint8 *buf, uint16 buf_len)
{
    uint8 *data;
    uint16 i;
    uint16 len;
    uint16 cnt = 0;

    if (buf_len == sizeof(struct RxControl)) {
        struct RxControl * rxCtrl = (struct RxControl *) buf;
        os_printf("[%sM%d%s] rssi%d, len%d\n", rxCtrl->CWB ? "H4" : "H2", rxCtrl->MCS, rxCtrl->FEC_CODING ? "L " : "",
                rxCtrl->rssi, rxCtrl->HT_length);
        return;
    } else if (buf_len == sizeof(struct sniffer_buf2)) {
        return; //manage pack
    } else {
        struct sniffer_buf * sniffer = (struct sniffer_buf *) buf;
        data = buf + sizeof(struct RxControl);

        struct ieee80211_hdr *hdr = (struct ieee80211_hdr *) data;
        if (sniffer->cnt == 1) {
            len = sniffer->ampdu_info[0].length;
            //get ieee80211_hdr/data len to do user task
            os_printf("[len] %d, rssi %d\n", len, sniffer->rx_ctrl.rssi);
        } else {
            int i;
            os_printf("rx ampdu %d\n", sniffer->cnt);
            for (i = 0; i < sniffer->cnt; i++) {
                hdr->seq_ctrl = sniffer->ampdu_info[i].seq;
                memcpy(&hdr->addr3, sniffer->ampdu_info[i].address3, 6);
                len = sniffer->ampdu_info[i].length;
                //get ieee80211_hdr/data len to do user task
                os_printf("[LEN] %d, RSSI %d\n", len, sniffer->rx_ctrl.rssi);
            }
        }
    }
}
