#include "CRCCheck.h"
#include <stdio.h>
#include <string.h>
CRCcheck* CRCcheck::instance = nullptr;
CRCcheck* CRCcheck::GetInstance()
{
    if (CRCcheck::instance == nullptr)
    {
        CRCcheck::instance = new CRCcheck();
    }
    return CRCcheck::instance;
}
/** 计算CRC校验码（使用查表法）
 * @brief doCheck
 * @param data
 * @param length
 * @return
 */
unsigned int CRCcheck::doCheck(char* data, int length)
{
    unsigned int crc = 0xFFFFFFFF; // 初始值为全1
    
    for (int i = 0; i < length; i++) {
        unsigned char index = (crc ^ data[i]) & 0xFF;
        crc = (crc >> 8) ^ crcTable[index];
    }

    crc ^= 0xFFFFFFFF; // 最后异或0xFFFFFFFF
    return crc;
}

CRCcheck::CRCcheck()
{
    unsigned int polynomial = 0xEDB88320; // CRC32多项式
    memset(this->crcTable, 0, sizeof(this->crcTable));
    for (unsigned int i = 0; i < 256; i++) {
        unsigned int crc = i;
        for (int j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ polynomial;
            }
            else {
                crc >>= 1;
            }
        }
        this->crcTable[i] = crc;
    }
}