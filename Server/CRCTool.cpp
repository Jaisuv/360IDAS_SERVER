#include "CRCTool.h"


CRCTool* CRCTool::instance = nullptr;
CRCTool* CRCTool::getInstance()
{
    if (CRCTool::instance == nullptr)
    {
        CRCTool::instance = new CRCTool();
    }
    return CRCTool::instance;
}
CRCTool::CRCTool()
{
    memset(crc_table, 0, sizeof(crc_table));
    this->crc = 0xffffffff;
}
void CRCTool::init_crc_table(void)
{
    unsigned int c;
    unsigned int i, j;

    for (i = 0; i < 256; i++) {
        c = (unsigned int)i;
        for (j = 0; j < 8; j++)
        {
            if (c & 1)
                c = 0xedb88320L ^ (c >> 1);
            else
                c = c >> 1;
        }
        crc_table[i] = c;
    }
    //0x555555762180
    cout << "crc生成多项式: " << crc_table << endl;
}
/*计算buffer的crc校验码*/
unsigned int CRCTool::calc_buf_crc(unsigned char* buffer, unsigned int size)
{
    //crc多项式为0
    if (crc_table[1] == 0)
    {
        init_crc_table();
    }
    unsigned int i;
    unsigned int crc_code = crc;
    //cout << "crc_code: " << crc_code <<endl;
    for (i = 0; i < size; i++)
    {
        crc_code = crc_table[(crc_code ^ buffer[i]) & 0xff] ^ (crc_code >> 8);
    }
    return crc_code;
}


