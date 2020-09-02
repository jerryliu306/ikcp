#ifndef __H_COMMON_H__
#define __H_COMMON_H__

#include <unistd.h>

/* encode 8 bits unsigned int */
char *kcp_encode8u(char *p, unsigned char c)
{
	*(unsigned char*)p++ = c;
	return p;
}

/* decode 8 bits unsigned int */
const char *kcp_decode8u(const char *p, unsigned char *c)
{
	*c = *(unsigned char*)p++;
	return p;
}

/* encode 16 bits unsigned int (lsb) */
char *kcp_encode16u(char *p, unsigned short w)
{
#if IWORDS_BIG_ENDIAN
	*(unsigned char*)(p + 0) = (w & 255);
	*(unsigned char*)(p + 1) = (w >> 8);
#else
	*(unsigned short*)(p) = w;
#endif
	p += 2;
	return p;
}

/* decode 16 bits unsigned int (lsb) */
const char *kcp_decode16u(const char *p, unsigned short *w)
{
#if IWORDS_BIG_ENDIAN
	*w = *(const unsigned char*)(p + 1);
	*w = *(const unsigned char*)(p + 0) + (*w << 8);
#else
	*w = *(const unsigned short*)p;
#endif
	p += 2;
	return p;
}

/* encode 32 bits unsigned int (lsb) */
char *kcp_encode32u(char *p, IUINT32 l)
{
#if IWORDS_BIG_ENDIAN
	*(unsigned char*)(p + 0) = (unsigned char)((l >>  0) & 0xff);
	*(unsigned char*)(p + 1) = (unsigned char)((l >>  8) & 0xff);
	*(unsigned char*)(p + 2) = (unsigned char)((l >> 16) & 0xff);
	*(unsigned char*)(p + 3) = (unsigned char)((l >> 24) & 0xff);
#else
	*(IUINT32*)p = l;
#endif
	p += 4;
	return p;
}

/* decode 32 bits unsigned int (lsb) */
const char *kcp_decode32u(const char *p, IUINT32 *l)
{
#if IWORDS_BIG_ENDIAN
	*l = *(const unsigned char*)(p + 3);
	*l = *(const unsigned char*)(p + 2) + (*l << 8);
	*l = *(const unsigned char*)(p + 1) + (*l << 8);
	*l = *(const unsigned char*)(p + 0) + (*l << 8);
#else 
	*l = *(const IUINT32*)p;
#endif
	p += 4;
	return p;
}

//////////////////////////////////////////
int printPackage(const char *data) {
	IUINT32 ts, sn, len, una, conv;
	IUINT16 wnd;
	IUINT8 cmd, frg;
    
	data = kcp_decode32u(data, &conv);
	data = kcp_decode8u(data, &cmd);
	data = kcp_decode8u(data, &frg);
	data = kcp_decode16u(data, &wnd);
	data = kcp_decode32u(data, &ts);
	data = kcp_decode32u(data, &sn);
	data = kcp_decode32u(data, &una);
	data = kcp_decode32u(data, &len);

    printf("--------------\n");
    printf("conv=%d\n", conv);
    printf("cmd=%d\n", cmd);
    printf("frg=%d\n", frg);
    printf("wnd=%d\n", wnd);
    printf("ts=%d\n", ts);
    printf("sn=%d\n", sn);
    printf("una=%d\n", una);
    printf("len=%d\n", len);
    printf("user data=%s\n", data);
    printf("--------------\n");
    return 0;
}

void isleep(int us) {
    usleep(1000*us);
}

int iclock()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec*1000 + tv.tv_usec/1000;  //毫秒
}

#endif
