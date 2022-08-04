#include <stdlib.h>
#include "fatfs.h"
#include "utf8.h"

/*******************************************************************************
* 名称: encUcs2ToUtf8
* 功能: 将1个unicode转为utf8
* 形参:
* 返回:
* 说明: unicode为ucs2编码(Big endian)
*******************************************************************************/
static int encUcs2ToUtf8One(const unsigned char *ucs2, unsigned char *utf8)
{
	unsigned long unic = *ucs2<<8|*(ucs2+1);
	if (unic <= 0x0000007F) {
		// * U-00000000 - U-0000007F:  0xxxxxxx
		*utf8 = (unic & 0x7F);
		return 1;
	} else if (unic >= 0x00000080 && unic <= 0x000007FF) {
		// * U-00000080 - U-000007FF:  110xxxxx 10xxxxxx
		*(utf8 + 1) = (unic & 0x3F) | 0x80;
		*utf8 = ((unic >> 6) & 0x1F) | 0xC0;
		return 2;
	} else if (unic >= 0x00000800 && unic <= 0x0000FFFF) {
		// * U-00000800 - U-0000FFFF:  1110xxxx 10xxxxxx 10xxxxxx
		*(utf8 + 2) = (unic & 0x3F) | 0x80;
		*(utf8 + 1) = ((unic >> 6) & 0x3F) | 0x80;
		*utf8 = ((unic >> 12) & 0x0F) | 0xE0;
		return 3;
	}
	return 0;
}

/*******************************************************************************
* 名称: encGetUtf8Size
* 功能: 获取utf8字符长度
* 形参:
* 返回:
* 说明:
*******************************************************************************/
int encGetUtf8Size(const unsigned char* utf8)
{
	int i =0,ret =0;

	if(NULL == utf8)
		return 0;
	for(i = 0;i<8;i++)
	{
		if(((*utf8)&(0x80>>i)))
			ret++;
		else
			break;
	}
	return ret;
}

/*******************************************************************************
* 名称: encUtf8ToUcs2One
* 功能: 将一个字符的UTF8编码转换成Unicode
* 形参:
* 返回: 成功则返回该字符的UTF8编码所占用的字节数; 失败则返回0.
* 说明: Unicode为UCS-2(Big endian)
*******************************************************************************/
static int encUtf8ToUcs2One(const unsigned char* utf8, unsigned char *Unic)
{
    // b1 表示UTF-8编码的pInput中的高字节, b2 表示次高字节, ...
    char b1, b2, b3;

    *Unic = 0x0; // 把 *Unic 初始化为全零
    int utfbytes = encGetUtf8Size(utf8);
    unsigned char *pOutput = Unic;

    switch ( utfbytes )
    {
        case 0:
            *pOutput     = 0;
            *(pOutput+1) = *utf8;
            utfbytes    += 1;
            break;
        case 2:
            b1 = *utf8;
            b2 = *(utf8 + 1);
            if ( (b2 & 0xE0) != 0x80 )
                return 0;
            *(pOutput+1)     = (b1 << 6) + (b2 & 0x3F);
            *pOutput = (b1 >> 2) & 0x07;
            break;
        case 3:
            b1 = *utf8;
            b2 = *(utf8 + 1);
            b3 = *(utf8 + 2);
            if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80) )
                return 0;
            *(pOutput+1) = (b2 << 6) + (b3 & 0x3F);
            *pOutput = (b1 << 4) + ((b2 >> 2) & 0x0F);
            break;
        default:
            return 0;
            break;
    }
    return utfbytes;
}

/*******************************************************************************
* 名称: encUtf8ToUcs2Len
* 功能: 将len个字节的UTF8编码转换成Unicode
* 形参:
* 返回: 返回该字符的UTF8编码所占用的字节数
* 说明: Unicode为UCS-2(Big endian)
*******************************************************************************/
int encUtf8ToUcs2Len(const unsigned char* utf8, int utf8_len, unsigned char *unic)
{
	const unsigned char* pin = utf8;
	unsigned char* pout = unic;
	int lenin = 0;
	int lenout = 0;
	while(lenin<utf8_len)
	{
		lenin += encUtf8ToUcs2One(pin+lenin,pout+lenout);
		lenout += 2;
	}
	return lenout;
}

/*******************************************************************************
* 名称: encUcs2ToUtf8Len
* 功能: 将len个字节的Unicode编码转换成UTF8
* 形参:
* 返回: 返回该字符的Unicode编码所占用的字节数
* 说明: Unicode为UCS-2(Big endian)
*******************************************************************************/
int encUcs2ToUtf8Len(const unsigned char* unic, int unic_len, unsigned char *utf8)
{
	const unsigned char* pin = unic;
	unsigned char* pout = utf8;
	int lenin = 0;
	int lenout = 0;
	while(lenin<unic_len)
	{
		lenout += encUcs2ToUtf8One(pin+lenin,pout+lenout);
		lenin += 2;
	}
	return lenout;
}

/*******************************************************************************
* 名称: encUtf8ToGbkLen
* 功能: 将len个字节的UTF8编码转换成gbk
* 形参:
* 返回: 该字符的GBK编码所占用的字节数
* 说明: 跳过编码失败的字符
*******************************************************************************/
int encUtf8ToGbkLen(const unsigned char* utf8, int utf8_len, unsigned char *gbk)
{
	const unsigned char* pin = utf8;
	unsigned char* pout = gbk;
	unsigned char gbkbuf[2] = {0,0};
	unsigned short gbkvalue;
	int lenin = 0;
	int lenout = 0;
	int lenutf8 = 0;
	while(lenin<utf8_len)
	{
		lenutf8 = encUtf8ToUcs2One(pin+lenin,gbkbuf);
		lenin += lenutf8;
		if(lenutf8==1)    //ASCII码
		{
			pout[lenout] = gbkbuf[1];
			lenout++;
		}
		else
		{
			gbkvalue = ff_convert(gbkbuf[0]<<8|gbkbuf[1],0);
			pout[lenout] = gbkvalue>>8;
			lenout++;
			pout[lenout] = gbkvalue&0x00FF;
			lenout++;
		}
	}
	return lenout;
}

/*******************************************************************************
* 名称: encGbkToUtf8Len
* 功能: 将len个字节的gbk编码转换成UTF8
* 形参:
* 返回: 返回该字符的utf8编码所占用的字节数
* 说明: 跳过编码失败的字符
*******************************************************************************/
int encGbkToUtf8Len(const unsigned char* gbk, int gbk_len, unsigned char *utf8)
{
	unsigned char* pout = utf8;
	int lenin = 0;
	int lenout = 0;
	unsigned short ucs2value;
	unsigned char ucs2[2] = {0,0};
	while(lenin<gbk_len)
	{
		if(gbk[lenin]<0x80)   //ASCII码
		{
			ucs2value = ff_convert(gbk[lenin],1);
			lenin += 1;
		}
		else
		{
			ucs2value = ff_convert(gbk[lenin]<<8|gbk[lenin+1],1);
			lenin += 2;
		}
		ucs2[0] = ucs2value>>8;
		ucs2[1] = ucs2value&0x00FF;
		lenout += encUcs2ToUtf8One(ucs2,pout+lenout);
	}
	return lenout;
}
