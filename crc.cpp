#include "crc.h"

uint8_t CRC8(const void *data, size_t length)
{
    uint8_t m_crc = 0;
    const uint8_t *m_data = (uint8_t *)data;

    while (length--) {
        m_crc = CRC8_TABLE[m_crc ^ *m_data++];
    }

    return m_crc;
}
