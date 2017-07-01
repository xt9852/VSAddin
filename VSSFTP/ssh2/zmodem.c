/****************************************************************
Copyright(c) 2005 Tsinghua Tongfang Optical Disc Co., Ltd.
Developed by Storage Product Department, Beijing, China
*****************************************************************
Filename: zmodem.c
Description:
****************************************************************/

#include "zmodem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL 0
#endif

#define WAZOOMAX 8192            /* Max packet size (WaZOO)                 */
#define ZATTNLEN 32              /* Max length of attention string          */

BYTE hex[] = "0123456789abcdef"; /* Send a byte as two hex digits           */

#define Z_PUTHEX(c) {int i=(c);SENDBYTE(hex[((i)&0xF0)>>4]);SENDBYTE(hex[(i)&0xF]);}

static void *g_param = NULL;
static SEND_CALLBACK g_send_cb = NULL;
static RECV_CALLBACK g_recv_cb = NULL;

#ifndef CAPTION
int outputCallback(void *, const char *data, int);
#else
int outputCallback(void *parm, const char *data, int len)
{
    printf(data);
    return 0;
}
#endif


// 向网络发送字符串
void SendBuffer(char *buf)
{
    g_send_cb(g_param, buf, strlen(buf));
}

void SendData(char *buf, int len)
{
    g_send_cb(g_param, buf, len);
}

// 向网络发送1个字节
void SENDBYTE(BYTE n)
{
    char data[2] = "";
    data[0] = n;
    g_send_cb(g_param, data, 1);
}

// 向网络发送16进制字符
void SendHEX(BYTE n)
{
    Z_PUTHEX(n);
}

int Z_GetBytes(char *buf, int max)
{
    return g_recv_cb(g_param, buf, max);

}

/*--------------------------------------------------------------------------*/
/* Z GET BYTE                                                               */
/* Get a byte from the modem.                                               */
/* return TIMEOUT if no read within timeout tenths,                         */
/*--------------------------------------------------------------------------*/
BYTE Z_GetByte()
{
    char buf[2] = "";
    int ret = g_recv_cb(g_param, buf, 2);
    return (ret == 1) ? buf[0] : '\xff';
}

/*--------------------------------------------------------------------------*/
/* Z TIMED READ                                                             */
/* Read a character from the modem line with timeout.                       */
/*  Eat parity, XON and XOFF characters.                                    */
/*--------------------------------------------------------------------------*/
BYTE Z_Read()
{
    for (;;)
    {
        BYTE c = Z_GetByte(30000);
        if ((c & 0x7f) == XON);
        else if ((c & 0x7f) == XOFF);
        else return c;
    }
}

#define CRCTABLE

/*--------------------------------------------------------------------------*/
/* Z UPDATE CRC                                                             */
/* update CRC                                                               */
/* crctab calculated by Mark G. Mendel, Network Systems Corporation         */
/*--------------------------------------------------------------------------*/
#ifdef CRCTABLE

static WORD crctab[256] =
{
    0x0000,  0x1021,  0x2042,  0x3063,  0x4084,  0x50a5,  0x60c6,  0x70e7,
    0x8108,  0x9129,  0xa14a,  0xb16b,  0xc18c,  0xd1ad,  0xe1ce,  0xf1ef,
    0x1231,  0x0210,  0x3273,  0x2252,  0x52b5,  0x4294,  0x72f7,  0x62d6,
    0x9339,  0x8318,  0xb37b,  0xa35a,  0xd3bd,  0xc39c,  0xf3ff,  0xe3de,
    0x2462,  0x3443,  0x0420,  0x1401,  0x64e6,  0x74c7,  0x44a4,  0x5485,
    0xa56a,  0xb54b,  0x8528,  0x9509,  0xe5ee,  0xf5cf,  0xc5ac,  0xd58d,
    0x3653,  0x2672,  0x1611,  0x0630,  0x76d7,  0x66f6,  0x5695,  0x46b4,
    0xb75b,  0xa77a,  0x9719,  0x8738,  0xf7df,  0xe7fe,  0xd79d,  0xc7bc,
    0x48c4,  0x58e5,  0x6886,  0x78a7,  0x0840,  0x1861,  0x2802,  0x3823,
    0xc9cc,  0xd9ed,  0xe98e,  0xf9af,  0x8948,  0x9969,  0xa90a,  0xb92b,
    0x5af5,  0x4ad4,  0x7ab7,  0x6a96,  0x1a71,  0x0a50,  0x3a33,  0x2a12,
    0xdbfd,  0xcbdc,  0xfbbf,  0xeb9e,  0x9b79,  0x8b58,  0xbb3b,  0xab1a,
    0x6ca6,  0x7c87,  0x4ce4,  0x5cc5,  0x2c22,  0x3c03,  0x0c60,  0x1c41,
    0xedae,  0xfd8f,  0xcdec,  0xddcd,  0xad2a,  0xbd0b,  0x8d68,  0x9d49,
    0x7e97,  0x6eb6,  0x5ed5,  0x4ef4,  0x3e13,  0x2e32,  0x1e51,  0x0e70,
    0xff9f,  0xefbe,  0xdfdd,  0xcffc,  0xbf1b,  0xaf3a,  0x9f59,  0x8f78,
    0x9188,  0x81a9,  0xb1ca,  0xa1eb,  0xd10c,  0xc12d,  0xf14e,  0xe16f,
    0x1080,  0x00a1,  0x30c2,  0x20e3,  0x5004,  0x4025,  0x7046,  0x6067,
    0x83b9,  0x9398,  0xa3fb,  0xb3da,  0xc33d,  0xd31c,  0xe37f,  0xf35e,
    0x02b1,  0x1290,  0x22f3,  0x32d2,  0x4235,  0x5214,  0x6277,  0x7256,
    0xb5ea,  0xa5cb,  0x95a8,  0x8589,  0xf56e,  0xe54f,  0xd52c,  0xc50d,
    0x34e2,  0x24c3,  0x14a0,  0x0481,  0x7466,  0x6447,  0x5424,  0x4405,
    0xa7db,  0xb7fa,  0x8799,  0x97b8,  0xe75f,  0xf77e,  0xc71d,  0xd73c,
    0x26d3,  0x36f2,  0x0691,  0x16b0,  0x6657,  0x7676,  0x4615,  0x5634,
    0xd94c,  0xc96d,  0xf90e,  0xe92f,  0x99c8,  0x89e9,  0xb98a,  0xa9ab,
    0x5844,  0x4865,  0x7806,  0x6827,  0x18c0,  0x08e1,  0x3882,  0x28a3,
    0xcb7d,  0xdb5c,  0xeb3f,  0xfb1e,  0x8bf9,  0x9bd8,  0xabbb,  0xbb9a,
    0x4a75,  0x5a54,  0x6a37,  0x7a16,  0x0af1,  0x1ad0,  0x2ab3,  0x3a92,
    0xfd2e,  0xed0f,  0xdd6c,  0xcd4d,  0xbdaa,  0xad8b,  0x9de8,  0x8dc9,
    0x7c26,  0x6c07,  0x5c64,  0x4c45,  0x3ca2,  0x2c83,  0x1ce0,  0x0cc1,
    0xef1f,  0xff3e,  0xcf5d,  0xdf7c,  0xaf9b,  0xbfba,  0x8fd9,  0x9ff8,
    0x6e17,  0x7e36,  0x4e55,  0x5e74,  0x2e93,  0x3eb2,  0x0ed1,  0x1ef0
};

#define Z_UpdateCRC(cp, crc) ( crctab[((crc >> 8) & 255)] ^ (crc << 8) ^ cp)

#else

WORD Z_UpdateCRC(WORD c, WORD crc)
{
    int count;

    for (count = 8; --count >= 0; )
    {
        if (crc & 0x8000)
        {
            crc <<= 1;
            crc += (((c <<= 1) & 0400) != 0);
            crc ^= 0x1021;
        }
        else
        {
            crc <<= 1;
            crc += (((c <<= 1) & 0400) != 0);
        }
    }
    return crc;
}
#endif   

/*--------------------------------------------------------------------------*/
/* Z SEND HEX HEADER                                                        */
/* Send ZMODEM HEX header hdr of type type                                  */
/*--------------------------------------------------------------------------*/
void Z_SendHexHeader(BYTE type, BYTE *hdr)
{
    int n;
    WORD crc;

    SENDBYTE(ZPAD);
    SENDBYTE(ZPAD);
    SENDBYTE(ZDLE);
    SENDBYTE(ZHEX);
    Z_PUTHEX(type);

    crc = Z_UpdateCRC(type, 0);

    for (n = 4; --n >= 0;)
    {
        Z_PUTHEX((*hdr));
        crc = Z_UpdateCRC((*hdr++), crc);
    }

    crc = Z_UpdateCRC(0, crc);
    crc = Z_UpdateCRC(0, crc);
    Z_PUTHEX((crc >> 8));
    Z_PUTHEX(crc);

    /* Make it printable on remote machine */
    SENDBYTE('\x0d'); SENDBYTE('\x0a');

    /* Uncork the remote in case a fake XOFF has stopped data flow */
    if (type != ZFIN) SENDBYTE(021);
}

/*--------------------------------------------------------------------------*/
/* Z GET ZDL                                                                */
/* Read a byte, checking for ZMODEM escape encoding                         */
/* including CAN*5 which represents a quick abort                           */
/* 0x10,0x11,0x13,0x90,0x91,0x93 & 0x18 is special treatment                */
/*--------------------------------------------------------------------------*/
int Z_GetZDL(void)
{
    int c;

    c = Z_GetByte(1000);

    if (c != ZDLE) return c;

    c = Z_GetByte(1000);

    switch (c)
    {
    case CAN:
        c = Z_GetByte(1000);

        if (c != CAN) return c;

        c = Z_GetByte(1000);

        if (c != CAN) return c;

        c = Z_GetByte(1000);

        if (c != CAN) return c;

        return (GOTCAN);
    case ZCRCE:
    case ZCRCG:
    case ZCRCQ:
    case ZCRCW: return (c | GOTOR);
    case ZRUB0: return 0x7F;
    case ZRUB1: return 0xFF;
    default:
        if ((c & 0x60) == 0x40)
            return c ^ 0x40;
        else
            return ZZERROR;
    }
}

/*--------------------------------------------------------------------------*/
/* Z GET BINARY HEADER                                                      */
/* Receive a binary style header (type and position)                        */
/*--------------------------------------------------------------------------*/
int Z_GetBinaryHeader(BYTE *hdr)
{
    int c, n;
    WORD crc;

    if ((c = Z_GetZDL()) & ~0xFF)
        return c;

    int Rxtype = c;

    crc = Z_UpdateCRC(c, 0);

    for (n = 4; --n >= 0;)
    {
        if ((c = Z_GetZDL()) & ~0xFF)
            return c;

        crc = Z_UpdateCRC(c, crc);
        *hdr++ = c;
    }

    if ((c = Z_GetZDL()) & ~0xFF)
        return c;

    crc = Z_UpdateCRC(c, crc);

    if ((c = Z_GetZDL()) & ~0xFF)
        return c;

    crc = Z_UpdateCRC(c, crc);

    if (crc & 0xFFFF)
        return ZZERROR;     //CRC error

    return Rxtype;
}

/*--------------------------------------------------------------------------*/
/* Z GET HEX                                                                */
/* Decode two lower case hex digits into an 8 bit byte value                */
/*--------------------------------------------------------------------------*/
int Z_GetHex(void)
{
    int ch, cl;

    ch = (int)Z_Read();
    ch -= '0';
    if (ch > 9) ch -= ('a' - ':');
    if (ch & 0xf0) return ZZERROR;

    cl = (int)Z_Read();
    cl -= '0';
    if (cl > 9) cl -= ('a' - ':');
    if (cl & 0xf0) return ZZERROR;

    return ((ch << 4) | cl);
}

/*--------------------------------------------------------------------------*/
/* Z GET HEX HEADER                                                         */
/* Receive a hex style header (type and position)                           */
/*--------------------------------------------------------------------------*/
int Z_GetHexHeader(BYTE *hdr)
{
    int c, n;
    WORD crc;

    if ((c = Z_GetHex()) < 0)
        return c;

    int Rxtype = c;

    crc = Z_UpdateCRC(c, 0);

    for (n = 4; --n >= 0;)
    {
        if ((c = Z_GetHex()) < 0)
            return c;

        crc = Z_UpdateCRC(c, crc);
        *hdr++ = c;
    }

    if ((c = Z_GetHex()) < 0)
        return c;

    crc = Z_UpdateCRC(c, crc);

    if ((c = Z_GetHex()) < 0)
        return c;

    crc = Z_UpdateCRC(c, crc);

    if (crc & 0xFFFF)
        return ZZERROR;     //CRC error

    c = Z_GetByte(1000);

    if (c == '\x0d')
        Z_GetByte(1000);


    return Rxtype;
}

/*--------------------------------------------------------------------------*/
/* Z PULL LONG FROM HEADER                                                  */
/* Recover a long integer from a header                                     */
/*--------------------------------------------------------------------------*/
DWORD Z_HeaderToLong(BYTE *hdr)
{
    DWORD l;
    l = hdr[ZP3];
    l = (l << 8) | hdr[ZP2];
    l = (l << 8) | hdr[ZP1];
    l = (l << 8) | hdr[ZP0];
    return l;
}

/*--------------------------------------------------------------------------*/
/* Z LONG TO HEADER                                                         */
/* Store long integer pos in Txhdr                                          */
/*--------------------------------------------------------------------------*/
void Z_LongToHeader(DWORD pos, BYTE *hdr)
{
    hdr[ZP0] = (BYTE)pos;
    hdr[ZP1] = (BYTE)(pos >> 8);
    hdr[ZP2] = (BYTE)(pos >> 16);
    hdr[ZP3] = (BYTE)(pos >> 24);
}

/*--------------------------------------------------------------------------*/
/* Z GET HEADER                                                             */
/* Read a ZMODEM header to hdr, either binary or hex.                       */
/*   On success, set Zmodem to 1 and return type of header.                 */
/*   Otherwise return negative on error                                     */
/*--------------------------------------------------------------------------*/
int Z_GetHeader(BYTE *hdr)
{
    int c, n, cancount;

    n = 10;   /* Max characters before start of frame */
    cancount = 5;

Again:
    c = Z_Read();
    switch (c)
    {
    case ZPAD: break; // This is what we want.
    case ZZRCDO:
    case ZZTIMEOUT: goto Done; //Timeout
    case CAN: if (--cancount <= 0) { c = ZCAN; goto Done; } //Cancel no break
    default:
Agn2:   if (--n <= 0) return ZCAN;
        if (c != CAN) cancount = 5;
        goto Again;
    }

    cancount = 5;
Splat:
    c = Z_Read();
    switch (c)
    {
    case ZDLE: break; // This is what we want.
    case ZPAD: goto Splat;
    case ZZRCDO:
    case ZZTIMEOUT: goto Done; //Timeout
    default: goto Agn2;
    }

    c = Z_Read();
    switch (c)
    {
    case ZBIN: //BIN head
        c = Z_GetBinaryHeader(hdr);
        break;
    case ZHEX: //HEX head
        c = Z_GetHexHeader(hdr);
        break;
    case CAN: //Cancel
        if (--cancount <= 0) { c = ZCAN; goto Done; }
        goto Agn2;
    case ZZRCDO:
    case ZZTIMEOUT: goto Done; //Timeout
    default: goto Agn2;
    }

Done:
    return c;
}

/*--------------------------------------------------------------------------*/
/* RZ RECEIVE DATA                                                          */
/* Receive array buf of max length with ending ZDLE sequence                */
/* and CRC.  Returns the ending character or error code.                    */
/*--------------------------------------------------------------------------*/
int RZ_ReceiveData(BYTE *buf, DWORD max, DWORD *received)
{
    int c, d;
    WORD crc;

    crc = 0;
    *received = 0;

    for (;;)
    {
        if ((c = Z_GetZDL()) & ~0xff)
        {
CRCfoo:
            switch (c)
            {
            case GOTCRCE:
            case GOTCRCG:
            case GOTCRCQ:
            case GOTCRCW: // C R C s
                crc = Z_UpdateCRC(((d = c) & 0xff), crc);
                if ((c = Z_GetZDL()) & ~0xff) goto CRCfoo;

                crc = Z_UpdateCRC(c, crc);
                if ((c = Z_GetZDL()) & ~0xff) goto CRCfoo;

                crc = Z_UpdateCRC(c, crc);
                if (crc & 0xffff) return ZZERROR;     //CRC error
                return d;
            case GOTCAN:    return ZCAN;    //Cancel
            case ZZTIMEOUT: return c;       //Timeout
            case ZZRCDO:    return c;       //No Carrier                    //没有载体
            default:        return c;       //Something bizarre             //有异常
            }
        }
        if (--max < 0) return ZZERROR;   //Long pkt
        *buf++ = c;
        (*received)++;
        crc = Z_UpdateCRC(c, crc);
        continue;
    }
}

/*--------------------------------------------------------------------------*/
/* RZ ACK BIBI                                                              */
/* Ack a ZFIN packet, let byegones be byegones                              */
/*--------------------------------------------------------------------------*/
void RZ_AckBibi(void)
{
    int n;
    BYTE hdr[4];

    Z_LongToHeader(0, hdr);

    for (n = 4; --n;)
    {
        Z_SendHexHeader(ZFIN, hdr);

        if (Z_GetByte(1000) == 'O')
        {
            Z_GetByte(1000); // Discard 2nd 'O'
            break;
        }
    }
}

/*--------------------------------------------------------------------------*/
/* RZ INIT RECEIVER                                                         */
/* Initialize for Zmodem receive attempt, try to activate Zmodem sender     */
/* Handles ZSINIT, ZFREECNT, and ZCOMMAND frames                            */
/*                                                                          */
/* Return codes:                                                            */
/*    ZFILE .... Zmodem filename received                                   */
/*    ZCOMPL ... transaction finished                                       */
/*    ERROR .... any other condition                                        */
/*--------------------------------------------------------------------------*/
int RZ_InitReceiver(p_file_info file)
{
    int n, errors = 0;
    BYTE  hdr[4];
    char  buf[WAZOOMAX + 1];
    BYTE  Attn[ZATTNLEN + 1];        /* String rx sends to tx on err            */
    DWORD received;

    for (n = 3; --n >= 0; )
    {
        Z_LongToHeader(0, hdr);
        hdr[ZF0] = CANFDX;
        Z_SendHexHeader(ZRINIT, hdr);

    AGAIN:
        switch (Z_GetHeader(hdr))
        {
        case ZRQINIT:     //USED
            Z_LongToHeader(0, hdr);
            hdr[ZF0] = CANFDX;
            Z_SendHexHeader(ZRINIT, hdr);
            goto AGAIN;
        case ZFILE:       //USED
            if (RZ_ReceiveData((BYTE*)buf, WAZOOMAX, &received) == GOTCRCW)
            {
                int len = strlen(buf);
                file->len = atol(&buf[len + 1]);
                strcpy_s(file->filename, sizeof(file->filename) - 1, buf);
                file->data = (BYTE*)malloc(file->len + 16);
                return ZFILE;
            }
            Z_SendHexHeader(ZNAK, hdr);
            goto AGAIN;
        case ZSINIT:      //not used
            if (RZ_ReceiveData(Attn, ZATTNLEN, &received) == GOTCRCW)
                Z_SendHexHeader(ZACK, hdr);
            else
                Z_SendHexHeader(ZNAK, hdr);
            goto AGAIN;
        case ZFREECNT:    //not used
            Z_LongToHeader(WAZOOMAX, hdr);
            Z_SendHexHeader(ZACK, hdr);
            goto AGAIN;
        case ZCOMMAND:    //not used
          /*-----------------------------------------*/
          /* Paranoia is good for you...             */
          /* Ignore command from remote, but lie and */
          /* say we did the command ok.              */
          /*-----------------------------------------*/
            if (RZ_ReceiveData((BYTE*)buf, WAZOOMAX, &received) == GOTCRCW)
            {
                Z_LongToHeader(0, hdr);

                do
                {
                    Z_SendHexHeader(ZCOMPL, hdr);
                } while (++errors < 10 && Z_GetHeader(hdr) != ZFIN);

                RZ_AckBibi();
                return ZCOMPL;
            }
            else Z_SendHexHeader(ZNAK, hdr);
            goto AGAIN;
        case ZCOMPL:      //not used
            goto AGAIN;
        case ZFIN:        //USED
            RZ_AckBibi();
            return ZCOMPL;
        case ZCAN:        //USED
        case ZZRCDO:
            return ZZERROR;
        case ZZTIMEOUT:   //USED
            return ZZERROR;
        }
    }
    return ZZERROR;
}

/*--------------------------------------------------------------------------*/
/* RZ RECEIVE FILE                                                          */
/* Receive one file; assumes file name frame is preloaded in Recbuf         */
/*--------------------------------------------------------------------------*/
int RZ_ReceiveFile(p_file_info file)
{
    int c, n = 10;//retry times;
    BYTE hdr[4];
    BYTE buf[WAZOOMAX + 1];
    DWORD pos = 0L;
    DWORD len = 0L;

    for (;;)
    {
        Z_LongToHeader(pos, hdr);
        Z_SendHexHeader(ZRPOS, hdr);
NxtHdr:
        switch (c = Z_GetHeader(hdr))
        {
        case ZDATA: // Data Packet
            if (Z_HeaderToLong(hdr) != pos)
            {
                if (--n < 0) return ZZERROR;
                continue;
            }
MoreData:
            switch (c = RZ_ReceiveData(file->data + pos, file->len - pos, &len))
            {
            case ZCAN:
            case ZZRCDO:  return ZZERROR; //CAN or CARRIER
            case ZZERROR: // CRC error
                if (--n < 0) return ZZERROR;
                continue;
            case ZZTIMEOUT: //Timeout
                if (--n < 0) return ZZERROR;
                continue;
            case GOTCRCW: // End of frame
                n = 10;
                pos += len;
                Z_LongToHeader(pos, hdr);
                Z_SendHexHeader(ZACK, hdr);
                goto NxtHdr;
            case GOTCRCQ: // Zack expected
                n = 10;
                pos += len;
                Z_LongToHeader(pos, hdr);
                Z_SendHexHeader(ZACK, hdr);
                goto MoreData;
            case GOTCRCG: // Non-stop
                n = 10;
                pos += len;
                goto MoreData;
            case GOTCRCE: // Header to follow
                n = 10;
                pos += len;
                goto NxtHdr;
            }
        case ZNAK:
        case ZZTIMEOUT: // Packed was probably garbled
            if (--n < 0) return ZZERROR;
            continue;
        case ZFILE: // Sender didn't see our ZRPOS yet
            RZ_ReceiveData(buf, WAZOOMAX, &len);
            continue;
        case ZEOF: // End of the file
            if (Z_HeaderToLong(hdr) != pos) continue;
            return c;
        case ZZERROR: // Too much garbage in header search error
            if (--n < 0) return ZZERROR;
            continue;
        default:
            return ZZERROR;
        }
    }
}

/*--------------------------------------------------------------------------*/
/* RZFILES                                                                  */
/* Receive a batch of files using ZMODEM protocol                           */
/*--------------------------------------------------------------------------*/
int RZ_ReceiveBatch(p_file_info file)
{
    int c;
    p_file_info last = file;

    for (;;)
    {
        switch (RZ_InitReceiver(file))
        {
        case ZCOMPL:   free(file); last->next = NULL; return ZZOK;
        case ZFILE:    break;
        default:       return ZZERROR;
        }

        switch (c = RZ_ReceiveFile(file))
        {
        case ZEOF:
            SendHEX((BYTE)(file->len >> 24));
            SendHEX((BYTE)(file->len >> 16));
            SendHEX((BYTE)(file->len >> 8));
            SendHEX((BYTE)(file->len));
            SendBuffer(" bytes received!!!\x0d\x0a");
        case ZSKIP:
            if (NULL == file->next)
            {
                last = file;
                file->next = (p_file_info)malloc(sizeof(file_info));
                file->next->next = NULL;
            }
            file = file->next;
            break;
        default: return c;
        }
    }
}

/*--------------------------------------------------------------------------*/
/* GET ZMODEM                                                               */
/* Receive a batch of files.                                                */
/* returns 0 for good xfer, other for bad                                   */
/* can be called from f_upload or to get mail from a WaZOO Opus             */
/*--------------------------------------------------------------------------*/
int zmodem_get(p_file_info file)
{
    int ret = RZ_ReceiveBatch(file);

    if (ret != ZZOK)
    {
        SendBuffer("Download error!!!\x0d\x0a");
    }

    return ret;
}

int ZS_GetByte(BYTE c, BYTE *o1, BYTE *o2)
{
    static BYTE lastsent;

    switch (c)
    {
    case 015:
    case 0215:  /*--------------------------------------------------*/
                /*                                                  */
                /*--------------------------------------------------*/
        if ((lastsent & 0x7F) != '@') goto SendIt;
    case 020:
    case 021:
    case 023:
    case 0220:
    case 0221:
    case 0223:
    case ZDLE:  /*--------------------------------------------------*/
                /* Quoted characters                                */
                /*--------------------------------------------------*/
        *o1 = ZDLE;
        c ^= 0x40;
        *o2 = c;
        lastsent = c;
        return 2;

    default:    /*--------------------------------------------------*/
                /* Normal character output                          */
SendIt:         /*--------------------------------------------------*/
        *o1 = c;
        lastsent = c;
        return 1;
    }
}

/*--------------------------------------------------------------------------*/
/* ZS SEND BYTE                                                             */
/* Send character c with ZMODEM escape sequence encoding.                   */
/* Escape XON, XOFF. Escape CR following @ (Telenet net escape)             */
/*--------------------------------------------------------------------------*/
void ZS_SendByte(BYTE c)
{
    BYTE o1;
    BYTE o2;

    if (1 == ZS_GetByte(c, &o1, &o2))
    {
        SENDBYTE(o1);
    }
    else
    {
        SENDBYTE(o1);
        SENDBYTE(o2);
    }
}

/*--------------------------------------------------------------------------*/
/* ZS SEND BINARY HEADER                                                    */
/* Send ZMODEM binary header hdr of type type                               */
/*--------------------------------------------------------------------------*/
void ZS_SendBinaryHeader(BYTE type, BYTE *hdr)
{
    WORD crc;
    int  n;

    SENDBYTE(ZPAD);
    SENDBYTE(ZDLE);
    SENDBYTE(ZBIN);
    ZS_SendByte(type);

    crc = Z_UpdateCRC(type, 0);

    for (n = 4; --n >= 0;)
    {
        ZS_SendByte(*hdr);
        crc = Z_UpdateCRC(((unsigned char)(*hdr++)), crc);
    }

    crc = Z_UpdateCRC(0, crc);
    crc = Z_UpdateCRC(0, crc);
    ZS_SendByte((BYTE)(crc >> 8));
    ZS_SendByte((BYTE)(crc));
}

/*--------------------------------------------------------------------------*/
/* ZS GET RECEIVER INFO                                                     */
/* Get the receiver's init parameters                                       */
/*--------------------------------------------------------------------------*/
int ZS_GetReceiverInfo(int *buf_len)
{
    int n;
    BYTE hdr[4];

    for (n = 10; --n >= 0; )
    {
        switch (Z_GetHeader(hdr))
        {
        case ZCHALLENGE:    /*--------------------------------------*/
                            /* Echo receiver's challenge number     */
                            /*--------------------------------------*/
            Z_SendHexHeader(ZACK, hdr);
            continue;
        case ZCOMMAND:      /*--------------------------------------*/
                            /* They didn't see our ZRQINIT          */
                            /*--------------------------------------*/
            Z_LongToHeader(0, hdr);
            Z_SendHexHeader(ZRQINIT, hdr);
            continue;
        case ZRINIT:        /*--------------------------------------*/
                            /*                                      */
                            /*--------------------------------------*/
            *buf_len = ((WORD)hdr[ZP1] << 8) | hdr[ZP0];
            return ZZOK;
        case ZCAN:
        case ZZRCDO:
        case ZZTIMEOUT:     /*--------------------------------------*/
                            /*                                      */
                            /*--------------------------------------*/
            return ZZERROR;
        case ZRQINIT:       /*--------------------------------------*/
                            /*                                      */
                            /*--------------------------------------*/
            if (hdr[ZF0] == ZCOMMAND) continue;
        default:            /*--------------------------------------*/
                            /*                                      */
                            /*--------------------------------------*/
            Z_SendHexHeader(ZNAK, hdr);
            continue;
        }
    }
    return ZZERROR;
}

/*--------------------------------------------------------------------------*/
/* ZS SEND DATA                                                             */
/* Send binary array buf with ending ZDLE sequence frameend                 */
/*--------------------------------------------------------------------------*/
void ZS_SendData(BYTE *data, DWORD pos, DWORD end, BYTE frameend)
{
    BYTE b;
    BYTE o1;
    BYTE o2;
    WORD crc = 0;
    int i = 0;

    char *buf = (char*)malloc(2 * (end - pos));

    while (pos < end)
    {
        b = data[pos++];

        if (1 == ZS_GetByte(b, &o1, &o2))
        {
            buf[i++] = o1;
        }
        else
        {
            buf[i++] = o1;
            buf[i++] = o2;
        }

        crc = Z_UpdateCRC(b, crc);
    }

    SendData(buf, i);

    free(buf);

    SENDBYTE(ZDLE);
    SENDBYTE(frameend);
    crc = Z_UpdateCRC(frameend, crc);

    crc = Z_UpdateCRC(0, crc);
    crc = Z_UpdateCRC(0, crc);
    ZS_SendByte((BYTE)(crc >> 8));
    ZS_SendByte((BYTE)(crc));
}

void ZS_SendFileName(char *name, DWORD len)
{
    BYTE *p;
    WORD crc = 0;
    char str[128] = ""; // filesize date

    sprintf_s(str, sizeof(str) - 1, "%u 0", len);

    for (;;)
    {
        ZS_SendByte(*name);
        crc = Z_UpdateCRC(((unsigned char)*name), crc);
        if (*name); else break;
        name++;
    }
    p = (BYTE*)str;
    for (;;)
    {
        ZS_SendByte(*p);
        crc = Z_UpdateCRC(((unsigned char)*p), crc);
        if (*p); else break;
        p++;
    }
    SENDBYTE(ZDLE);
    SENDBYTE(ZCRCW);
    crc = Z_UpdateCRC(ZCRCW, crc);

    crc = Z_UpdateCRC(0, crc);
    crc = Z_UpdateCRC(0, crc);
    ZS_SendByte((BYTE)(crc >> 8));
    ZS_SendByte((BYTE)(crc));
}

/*--------------------------------------------------------------------------*/
/* ZS SYNC WITH RECEIVER                                                    */
/* Respond to receiver's complaint, get back in sync with receiver          */
/*--------------------------------------------------------------------------*/
int ZS_SyncWithReceiver(DWORD *pos)
{
    int c;
    int num_errs = 7;
    BYTE hdr[4];

    while (1)
    {
        c = Z_GetHeader(hdr);

        switch (c)
        {
        case ZZTIMEOUT:  if ((num_errs--) >= 0) break;
        case ZCAN:
        case ZABORT:
        case ZFIN:
        case ZZRCDO:/*-----------------------------------------*/
                    /*                                         */
                    /*-----------------------------------------*/
            return ZZERROR;
        case ZRPOS: /*-----------------------------------------*/
                    /*                                         */
                    /*-----------------------------------------*/
            *pos = Z_HeaderToLong(hdr);
            return c;
        case ZSKIP: /*-----------------------------------------*/
                    /*                                         */
                    /*-----------------------------------------*/
        case ZRINIT:/*-----------------------------------------*/
                    /*                                         */
                    /*-----------------------------------------*/
            return c;
        case ZACK:  /*-----------------------------------------*/
                    /*                                         */
                    /*-----------------------------------------*/
            return c;
        default:    /*-----------------------------------------*/
                    /*                                         */
                    /*-----------------------------------------*/
            ZS_SendBinaryHeader(ZNAK, hdr);
            continue;
        }
    }
}

/*--------------------------------------------------------------------------*/
/* ZS SEND FILE DATA                                                        */
/* Send the data in the file                                                */
/*--------------------------------------------------------------------------*/
int ZS_SendFileData(BYTE *data, DWORD pos, DWORD end, int buf_len, int blk)
{
    int c;
    int blklen, maxblklen;
    BYTE e;
    BYTE hdr[4];

    maxblklen = (buf_len && (blk > buf_len)) ? buf_len : blk;
    blklen = maxblklen;

SomeMore:
    Z_LongToHeader(pos, hdr);
    Z_SendHexHeader(ZDATA, hdr);

    do
    {
        c = blklen;

        if ((pos + c) > end)
            c = end - pos;

        e =  (c < blklen) ? ZCRCE : ZCRCG;

        ZS_SendData(data, pos, pos + c, e);

        pos += c;

    } while (e == ZCRCG);

    while (1)
    {
        Z_LongToHeader(pos, hdr);
        Z_SendHexHeader(ZEOF, hdr);

        switch (c = ZS_SyncWithReceiver(&pos))
        {
        case ZACK:      /*-----------------------------------------*/
                        /*                                         */
                        /*-----------------------------------------*/
            continue;
        case ZRPOS:     /*-----------------------------------------*/
                        /* Resume at this position...              */
                        /*-----------------------------------------*/
            goto SomeMore;
        case ZRINIT:    /*-----------------------------------------*/
                        /* Receive init                            */
                        /*-----------------------------------------*/
            return ZZOK;
        case ZSKIP:     /*-----------------------------------------*/
                        /* Request to skip the current file        */
                        /*-----------------------------------------*/
            return c;
        default:        /*-----------------------------------------*/
                        /*                                         */
                        /*-----------------------------------------*/
            return ZZERROR;
        }
    }
}

/*--------------------------------------------------------------------------*/
/* ZS SEND FILE                                                             */
/* Send ZFILE frame and begin sending ZDATA frame                           */
/*--------------------------------------------------------------------------*/
int ZS_SendFile(char *filename, BYTE *data, DWORD len, int buf_len)
{
    int c;
    BYTE hdr[4];
    DWORD pos;

    for (;;)
    {
        hdr[ZF0] = LZCONV;    /* Default file conversion mode */
        hdr[ZF1] = LZMANAG;   /* Default file management mode */
        hdr[ZF2] = LZTRANS;   /* Default file transport mode */
        hdr[ZF3] = 0;
        Z_SendHexHeader(ZFILE, hdr);
        ZS_SendFileName(filename, len);

Again:
        switch (c = Z_GetHeader(hdr))
        {
        case ZRINIT:    /*-----------------------------------------*/
                        /*                                         */
                        /*-----------------------------------------*/
            goto Again;
        case ZCAN:
        case ZCRC:
        case ZZRCDO:
        case ZZTIMEOUT:
        case ZFIN:
        case ZABORT:     /*----------------------------------------*/
                         /*                                        */
                         /*----------------------------------------*/
            return ZZERROR;
        case ZSKIP:     /*-----------------------------------------*/
                        /* Other system wants to skip this file    */
                        /*-----------------------------------------*/
            return c;
        case ZRPOS:     /*-----------------------------------------*/
                        /* Resend from this position...            */
                        /*-----------------------------------------*/
            pos = Z_HeaderToLong(hdr);
            return ZS_SendFileData(data, pos, len, buf_len, 1024);
        }
    }
}

/*--------------------------------------------------------------------------*/
/* ZS END SEND                                                              */
/* Say BIBI to the receiver, try to do it cleanly                           */
/*--------------------------------------------------------------------------*/
void ZS_EndSend()
{
    BYTE hdr[4];

    while (1)
    {
        Z_LongToHeader(0, hdr);
        Z_SendHexHeader(ZFIN, hdr);

        switch (Z_GetHeader(hdr))
        {
        case ZFIN:      /*-----------------------------------------*/
                        /*                                         */
                        /*-----------------------------------------*/
            SENDBYTE('O');
            SENDBYTE('O');
        case ZCAN:
        case ZZRCDO:
        case ZZTIMEOUT: /*-----------------------------------------*/
                        /*                                         */
                        /*-----------------------------------------*/
            return;
        }
    }
}

/*--------------------------------------------------------------------------*/
/* SEND ZMODEM (send a file)                                                */
/*   returns 0 for good xfer, other for bad                                 */
/*   sends one file per call; 'fsent' flags start and end of batch          */
/*--------------------------------------------------------------------------*/
int zmodem_put(p_file_info file)
{
    int ret;
    int buf_len = 0;
    BYTE hdr[4];

    Z_LongToHeader(0, hdr);
    Z_SendHexHeader(ZRQINIT, hdr);

    if (ZS_GetReceiverInfo(&buf_len) == ZZERROR)
    {
        return ZZERROR;
    }

    do
    {
        ret = ZS_SendFile(file->filename, file->data, file->len, buf_len);

        file = file->next;

    } while (NULL != file);

    ZS_EndSend();

    return ret;
}

int zmodem_set(void *param, SEND_CALLBACK send_cb, RECV_CALLBACK recv_cb)
{
    if (NULL == param || NULL == send_cb || NULL == recv_cb)
    {
        return -1;
    }

    g_param = param;
    g_send_cb = send_cb;
    g_recv_cb = recv_cb;

    return 0;
}

int file_get(const char *local_filename, int *len)
{
    int ret;
    file_info file = { 0 };

    ret = zmodem_get(&file);

    if (ZZOK == ret)
    {
        FILE *fp = NULL;
        fopen_s(&fp, local_filename, "wb+");

        if (NULL != fp)
        {
            fwrite(file.data, 1, file.len, fp);
            fclose(fp);
        }

        *len = file.len;
    }

    if (NULL != file.data)
    {
        free(file.data);
    }

    return ret;
}

int file_put(const char *local_filename, const char *remote_filename, int *len)
{
    if (NULL == local_filename || NULL == remote_filename || NULL == len)
    {
        return -1;
    }

    int ret;
    file_info file = { 0 };
    strcpy_s(file.filename, sizeof(file.filename) - 1, remote_filename);

    FILE *fp = NULL;
    fopen_s(&fp, local_filename, "rb");

    if (NULL != fp)
    {
        fseek(fp, 0, SEEK_END);
        file.len = ftell(fp);
        file.data = (BYTE*)malloc(file.len + 16);
        fseek(fp, 0, SEEK_SET);
        fread(file.data, 1, file.len, fp);
        fclose(fp);
        *len = file.len;

        ret = zmodem_put(&file);

        free(file.data);
    }
    else
    {
        ret = -2;
    }

    return ret;
}
