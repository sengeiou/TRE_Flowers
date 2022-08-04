#ifndef UTF8_H_
#define UTF8_H_

//将len个字节的UTF8编码转换成Unicode
int encUtf8ToUcs2Len(const unsigned char* utf8, int utf8_len, unsigned char *unic);
//将len个字节的Unicode编码转换成UTF8
int encUcs2ToUtf8Len(const unsigned char* unic, int unic_len, unsigned char *utf8);

//将len个字节的UTF8编码转换成gbk
int encUtf8ToGbkLen(const unsigned char* utf8, int utf8_len, unsigned char *gbk);
//将len个字节的gbk编码转换成UTF8
int encGbkToUtf8Len(const unsigned char* gbk, int gbk_len, unsigned char *utf8);

#endif /* UTF8_H_ */
