#include <LCD1602.h>
void wait(unsigned int t)//Һ���ȴ�
{
//  Lcd1602Rs=0;
//  Lcd1602Rw=1;
//  Lcd1602E=1;
//  //while(P0&0x80);
//  Lcd1602E=0;
  mdelay(t);	
}

/*--------------------------------------------------
���������ܣ�дһ���ַ�
�����룺��д�ַ�
���������
������ģ�飺wait()
--------------------------------------------------*/
void LcdWrite(bit com, uchar a)	//com ��ʾ���������
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
���������ܣ���ʼ��Lcd
�����룺��
���������
������ģ��:wait()
--------------------------------------------------*/
void	Intlcd(void)
{
  LcdWrite(LcdCom,0x01);//����
	wait(1000);
  LcdWrite(LcdCom,0x38);//�ù��ܣ�2�У�5x7�ַ�
	wait(1000);
  LcdWrite(LcdCom,0x06);//������ģʽ����ַ��������ʾ�����ƶ�
	wait(1000);
  LcdWrite(LcdCom,0x0c);//��ʾ��������ʾ��꣬��겻��˸ 
	wait(1000);
  LcdWrite(LcdCom,0x01);//����
	wait(1000);
}
/*--------------------------------------------------
���������ܣ���ָ��λ��дһ�ַ�
�����룺�ַ���д���ַ��x��y,��д�ַ�uc
���������
������ģ�飺Lcdwrite()
--------------------------------------------------
void   Display_one(uchar x,uchar y,uchar uc)
{
  LcdWrite(LcdCom,(x-1)*0x40+y-1+0x80);
  LcdWrite(LcdData,uc);
}
*/
/*--------------------------------------------------
���������ܣ�д�������е�һ���ַ�
�����룺��
���������
������ģ�飺Lcdwrite()
--------------------------------------------------
void	Display_all(uchar *DisplayBuf)
{
  uchar	i;
  LcdWrite(LcdCom,0x80);
  for(i=0x00;i<=0x0f;i++)
  {
    LcdWrite(LcdData,DisplayBuf[i]);
  }
  LcdWrite(LcdCom,0xc0);  //�ڶ��е�ַ�任
  for(i=0x10;i<=0x1f;i++)
  {
    LcdWrite(LcdData,DisplayBuf[i]);
  }	
} */

/*--------------------------------------------------
���������ܣ�д�������е�һ���ַ�
�����룺��д���ݵ�ַtab
���������
������ģ�飺Lcdwrite()
--------------------------------------------------*/
void	display_tab(uchar *tab)
{
  uchar	i;
  LcdWrite(LcdCom,0x80);
  for(i=0x00;i<=0x0f;i++)
  {
    LcdWrite(LcdData,tab[i]);
  }
  LcdWrite(LcdCom,0xc0);//�ڶ��е�ַ�任
  for(i=0x10;i<=0x1f;i++)
  {
    LcdWrite(LcdData,tab[i]);
  }
} 
/*--------------------------------------------------
���������ܣ���ʾ�б��ͻ�����
�����룺��ʾ���ݵ�ַtab
���������
������ģ�飺��
--------------------------------------------------
void	Load_to_Lcd(uchar *tab,uchar *DisplayBuf)
{	
  uchar	i;
  for(i=0x00;i<=0x1f;i++)
  {
    DisplayBuf[i]=tab[i];
  }	
}*/
