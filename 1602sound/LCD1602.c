#include <LCD1602.h>
void wait(unsigned int t)//液晶等待
{
//  Lcd1602Rs=0;
//  Lcd1602Rw=1;
//  Lcd1602E=1;
//  //while(P0&0x80);
//  Lcd1602E=0;
  mdelay(t);	
}

/*--------------------------------------------------
＊函数功能：写一个字符
＊输入：所写字符
＊输出：无
＊调用模块：wait()
--------------------------------------------------*/
void LcdWrite(bit com, uchar a)	//com 标示命令还是数据
{
  wait(10);
  Lcd1602Rs	= com;
  Lcd1602Rw = 0;
  LCD1602PORT    = a; 
  Lcd1602E	= 1;
	wait(10);
  Lcd1602E	= 0;
  wait(10);
}
/*--------------------------------------------------
＊函数功能：初始化Lcd
＊输入：无
＊输出：无
＊调用模块:wait()
--------------------------------------------------*/
void	Intlcd(void)
{
  LcdWrite(LcdCom,0x01);//清屏
	wait(1000);
  LcdWrite(LcdCom,0x38);//置功能：2行，5x7字符
	wait(1000);
  LcdWrite(LcdCom,0x06);//置输入模式：地址增量，显示屏不移动
	wait(1000);
  LcdWrite(LcdCom,0x0c);//显示开，不显示光标，光标不闪烁 
	wait(1000);
  LcdWrite(LcdCom,0x01);//清屏
	wait(1000);
}
/*--------------------------------------------------
＊函数功能：在指定位置写一字符
＊输入：字符串写入地址行x列y,所写字符uc
＊输出：无
＊调用模块：Lcdwrite()
--------------------------------------------------
void   Display_one(uchar x,uchar y,uchar uc)
{
  LcdWrite(LcdCom,(x-1)*0x40+y-1+0x80);
  LcdWrite(LcdData,uc);
}
*/
/*--------------------------------------------------
＊函数功能：写缓冲区中的一串字符
＊输入：无
＊输出：无
＊调用模块：Lcdwrite()
--------------------------------------------------
void	Display_all(uchar *DisplayBuf)
{
  uchar	i;
  LcdWrite(LcdCom,0x80);
  for(i=0x00;i<=0x0f;i++)
  {
    LcdWrite(LcdData,DisplayBuf[i]);
  }
  LcdWrite(LcdCom,0xc0);  //第二行地址变换
  for(i=0x10;i<=0x1f;i++)
  {
    LcdWrite(LcdData,DisplayBuf[i]);
  }	
} */

/*--------------------------------------------------
＊函数功能：写缓冲区中的一串字符
＊输入：所写内容地址tab
＊输出：无
＊调用模块：Lcdwrite()
--------------------------------------------------*/
void	display_tab(uchar *tab)
{
  uchar	i;
  LcdWrite(LcdCom,0x80);
  for(i=0x00;i<=0x0f;i++)
  {
    LcdWrite(LcdData,tab[i]);
  }
  LcdWrite(LcdCom,0xc0);//第二行地址变换
  for(i=0x10;i<=0x1f;i++)
  {
    LcdWrite(LcdData,tab[i]);
  }
} 
/*--------------------------------------------------
＊函数功能：显示列表送缓冲区
＊输入：显示内容地址tab
＊输出：无
＊调用模块：无
--------------------------------------------------
void	Load_to_Lcd(uchar *tab,uchar *DisplayBuf)
{	
  uchar	i;
  for(i=0x00;i<=0x1f;i++)
  {
    DisplayBuf[i]=tab[i];
  }	
}*/
