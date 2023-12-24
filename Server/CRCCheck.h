#pragma once
class CRCcheck
{
public:
    static CRCcheck* GetInstance();
    /** 计算CRC校验码（使用查表法）
     * @brief doCheck
     * @param data
     * @param length
     * @return
     */
    unsigned int doCheck(char* data, int length);
private:
    CRCcheck();
    static CRCcheck* instance;//实例
    unsigned int crcTable[256];//CRC校验表
};

