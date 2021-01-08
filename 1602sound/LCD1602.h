#ifndef _LCD1602_H_
#define _LCD1602_H_
#define null     0
#include <STC89C5xRC.H>
#define uchar unsigned char        
#define uint unsigned int  
#define LcdData 1
#define LcdCom  0  
sbit  Lcd1602Rs = P2^4;
sbit  Lcd1602Rw = P2^5;
sbit  Lcd1602E   = P2^6;
#define LCD1602PORT P1
#define mdelay(t) while(t--);

/*œ‘ æœ‡πÿ
#define Clr_dsp	  0x01h;
#define Return	  0x02h;
#define	InMode	  0x06h;
#define	CorMode   0x0ch;
#define CloseCur  0x0ch;
#define	OpenCur	  0x0eh;
#define	CurShift  0x10h;
#define	FuncSet	  0x38h;
#define	CGAddRSet 0x40h;
#define	DDAddRSet 0x80h;
#define Busy	  0x80h;
*/ 
extern void	Intlcd(void);
extern void	Load_to_Lcd(uchar *tab,uchar *DisplayBuf);
extern void	display_tab(uchar *tab);
extern void	Display_all(uchar *DisplayBuf);



#endif