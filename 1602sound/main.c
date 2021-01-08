#include <STC89C5xRC.H>
#include <stdio.h>
#include <LCD1602.h>
#define All 6          //��������
#define Xtal	11030000
/* ������ֵ */
#define uchar	unsigned char
typedef unsigned char	uint8;
/* �޷���8λ���ͱ��� */
typedef signed char	int8;
/* �з���8λ���ͱ��� */
typedef unsigned short	uint16;
/* �޷���16λ���ͱ��� */
typedef signed short	int16;
/* �з���16λ���ͱ��� */
typedef unsigned int	uint32;
/* �޷���32λ���ͱ��� */
typedef signed int	int32;
/* �з���32λ���ͱ��� */
typedef float		fp32;
/* �����ȸ�������32λ���ȣ� */
typedef double		fp64;
/* ˫���ȸ�������64λ���ȣ� */
sbit	row1	= P0^0;
sbit	row2	= P0^1;
sbit	row3	= P0^2;
sbit	row4	= P0^3;
sbit	col1	= P0^4;
sbit	col2	= P0^5;
sbit	col3	= P0^6;
sbit	col4	= P0^7;
#define uint unsigned int
uchar	STH0;
/* ��ʱ��0������ֵ */
uchar	STL0;
uchar	T1RH;
/* ��ʱ��1������ֵ */
uchar	T1RL;
bit	FY		= 0;
/* ������ʱFY=1, �����ٵ���ʱFY=0 */
uchar	k;


sbit BUZZ = P3^7;  //��������������
sbit digTube = P2; //�������ʾ����

unsigned char Num = 1; //�ڼ��׸�
bit enable = 1;   //����������ʹ�ܱ�־
bit tmrflag = 0;  //��ʱ���ж���ɱ�־
unsigned char T0RH = 0xFF;  //T0����ֵ�ĸ��ֽ�
unsigned char T0RL = 0x00;  //T0����ֵ�ĵ��ֽ�
uint count = 1000;//��ʱ����ʱ1000��
unsigned char DispBuf[50];//Һ����ʾ�ַ�������
bit stopOver=0;


void Delay( uint16 count )
{
	uint8 i;
	while ( --count != 0 ) {
		for ( i = 0; i < 125; i++ )
			;
		/* ";" ��ʾ�����,CPU��ת�� */
	}
	/* i ��0�ӵ�125����12M������CPU��ź�ʱ1���� */
}

void PlayControl(unsigned char Num);//����ѡ����
void Play();//���ຯ��

unsigned char code LedChar[] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8,0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E};//�������ʾ�ַ�ת����
unsigned int code NoteFrequ[] = {  //����1-7����1-7�͸���1-7��ӦƵ���б�
	523,  587,  659,  698,  784,  880,  988,  //����1-7
	1047, 1175, 1319, 1397, 1568, 1760, 1976,  //����1-7
	262, 294, 330, 349, 392, 440, 494,300//����1-7  ���һ����������
};
unsigned int code NoteReload[] = { //����1-7�͸���1-7��Ӧ�Ķ�ʱ������ֵ
	65536 - (Xtal/12) / (523*2),  //����1
	65536 - (Xtal/12) / (587*2),  //2
	65536 - (Xtal/12) / (659*2),  //3
	65536 - (Xtal/12) / (698*2),  //4
	65536 - (Xtal/12) / (784*2),  //5
	65536 - (Xtal/12) / (880*2),  //6
	65536 - (Xtal/12) / (988*2),  //7
	65536 - (Xtal/12) / (1047*2), //����1
	65536 - (Xtal/12) / (1175*2), //2
	65536 - (Xtal/12) / (1319*2), //3
	65536 - (Xtal/12) / (1397*2), //4
	65536 - (Xtal/12) / (1568*2), //5
	65536 - (Xtal/12) / (1760*2), //6
	65536 - (Xtal/12) / (1976*2), //7
	65536 - (Xtal/12) / (262*2), //����1
	65536 - (Xtal/12) / (294*2), //2
	65536 - (Xtal/12) / (330*2), //3
	65536 - (Xtal/12) / (349*2), //4
	65536 - (Xtal/12) / (392*2), //5
	65536 - (Xtal/12) / (440*2), //6
	65536 - (Xtal/12) / (494*2), //7
	65536 - (Xtal/12) / (300*2), //��������
};

unsigned char *SongNote, *SongBeat;//ָ��
unsigned char code XiaoXingXingNote[] = {   //С����
	1, 1, 5, 5,    6, 6, 5,    4, 4, 3, 3,    2, 2, 1,
	5, 5, 4, 4,    3, 3, 2,    5, 5, 4, 4,    3, 3, 2 ,
	1, 1, 5, 5,    6, 6, 5,    4, 4, 3, 3,    2, 2, 1, 0xff
};

unsigned char code XiaoXingXingBeat[] = {
	4, 4, 4, 4,    4, 4, 8,    4, 4, 4, 4,    4, 4, 8,
	4, 4, 4, 4,    4, 4, 8,    4, 4, 4, 4,    4, 4, 8,
	4, 4, 4, 4,    4, 4, 8,    4, 4, 4, 4,    4, 4, 8,
};
unsigned char code TwoTigerNote[] = {    //��ֻ�ϻ�������
	1,   2,   3, 1,    1,   2,   3, 1,   3, 4, 5,   3, 4, 5,
	5,6, 5,4, 3, 1,    5,6, 5,4, 3, 1,   1, 5, 1,   1, 5, 1,0xff
};

unsigned char code TwoTigerBeat[] = {    //��ֻ�ϻ����ı�4��ʾһ�ģ�1����1/4�ģ�8����2��
	4,   4,   4, 4,    4,   4,   4, 4,   4, 4, 8,   4, 4, 8,
	3,1, 3,1, 4, 4,    3,1, 3,1, 4, 4,   4, 4, 8,   4, 4, 8,
};
unsigned char code HappybirthdayNote[] = {  //���տ��ָ�
	5, 5, 6, 5, 8, 7, 22, 5, 5, 6, 5, 9, 8, 5, 5, 12, 10, 8, 7, 6, 11, 11,  10, 8, 9, 8,0xff
};
unsigned char code HappybirthdayBeat[] = {
	2, 2, 4, 4, 4, 4, 4, 2, 2,  4, 4, 4, 8, 2, 2, 4, 4, 4, 4, 8, 2, 2, 4, 4, 4, 8
};
unsigned char code         ZhiduanqingchangNote[] = { //ֽ���鳤
	//8, 9, 10, 9, 8, 6, 22, 11, 10, 9, 22, 7, 8, 9, 7, 7, 5, 22, 7, 9, 8, 8, 3, 4, 2, 3, 1, 1, 5, 2,  //ǰ��
	19, 3, 3, 3, 2, 3, 1, 2, 2, 2, 1, 2, 5, 1, 1, 1, 20, 1, 20, 19,     2, 2, 2, 3, 20, 19, 4, 4, 4, 3, 4, 1, 2, 2, 2, 1, 2, 5,
	1, 1, 1, 20, 1, 20, 3, 2, 2, 1, 1, 22, 22, 22, 1, 1, 2,             3, 3, 3, 22, 3, 2, 1, 21, 6, 5, 22, 5, 6, 7, 8, 3, 3, 22, 8, 7, 8,
	7, 3, 5, 5, 6, 8, 5, 6, 22, 6, 5, 4,5, 3, 2, 1, 20, 1,               3, 2, 2, 1, 2, 5, 1, 2, 5, 22, 22, 22, 1, 1, 2,
	3, 3, 2, 3, 22, 3, 2, 1, 5, 5, 3, 5, 22, 5, 6, 7,                   8, 8, 8, 8, 8, 7, 6, 7, 6, 3, 5, 5, 5, 6, 8, 5, 6, 22, 6, 5, 4,
	5, 3, 2, 1, 6, 1, 3, 2, 2, 1, 2, 1, 20, 1, 22,                      22, 20, 1, 3, 2, 2, 22, 2, 1, 2, 1, 1, 22, 20, 1, 1, 0xff
};
unsigned char code ZhiduanqingchangBeat[] = {
	//2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 8, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 8, 8, 12, 4, 5, 4, 8, 8, 4,   //ǰ��
	8, 2, 2, 2, 2, 2, 5, 2, 2, 2, 2, 2, 5, 2, 2, 2, 2, 2, 4, 2,       2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 4, 5, 2, 2, 2, 2, 2, 5,
	2, 2, 2, 2, 2, 5, 2, 2, 2, 2, 8, 4, 4, 2, 2, 2, 2,                4, 2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2,
	4, 2, 5, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 4, 2, 5, 1, 1,             2, 2, 2, 2, 2, 4, 2, 16, 16,4, 4, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,                   2, 2, 2, 2, 2, 2, 2, 2, 2, 4, 2, 4, 2, 2, 2, 4, 2, 2, 2, 2, 2,
	2, 4, 2, 5, 1, 1, 2, 2, 2, 2, 2, 4, 1, 12, 4,                     2, 1, 1, 2, 2, 8, 2, 1, 1, 2, 2, 8, 2, 2, 12, 16
};


unsigned char code YiLuXiangBeiNote[] = {   //????
	22,22,4,4,3,2,1,  2,3,3,22,22,  22,22,4,4,3,2,1,  2,1,1,22,1,2,5,
	5,1,6,5,5,3,  5,1,6,5,5,3,  3,2,2,6,5,5,3,  3,2,2,22,22,
	22,22,4,4,3,2,1,  2,3,3,22,22,  22,22,4,4,3,2,1,  2,1,1,22,1,3,7,  7,8,22,8,8,9,8,7,   
	7,8,22,8,8,9,8,9,  10,9,22,8,8,9,8,9,  10,11,10,9,22,5,5,9,  9,8,8,10,10,9,7,6,  6,5,5,22,5,5,8,
	8,4,22,8,7,7,8,9,  9,10,10,22,5,5,9,9,8,8,10,10,9,7,6,6,5,5,3,9,9,8,22,5,5,6,1,1,4,3,2,1,1,1,22,5,5,9,1,22,5,
	5,6,1,1,4,3,2,1, 0xff
};
unsigned char code YiLuXiangBeiBeat[] = {
	4 ,2 ,2,2,2,2,2,  2,2,4,4 ,4 ,  4 ,2 ,2,2,2,2,2,  2,2,4,2 ,2,2,2,  
	4,4,2,2,2,2,  4,4,2,2,2,2,  2,2,4,2,2,2,2,  3,1,4,4 ,4 ,  
	4 ,2 ,2,2,2,2,2,  2,2,4,4 ,4 ,  4 ,2 ,2,2,2,2,2,  3,1,4,2 ,2,2,2,  2,2,2 ,2,2,2,2,2,  
	2,2,2 ,2,2,2,2,2,  2 ,2,2 ,2,2,2,2,2,  2 ,2 ,2 ,2,2 ,2,2,2,   3,1,2,2 ,2 ,2,2,2,  3,1,4,2,2,2,2,   
	2,2,2 ,2,2,2,2,2,  3,1 ,4,2,2,2,2,	3,1,2,2,2,2,2,2,	2,2,2,1,1,1,3,2,2,	2,1,1,3,1,2,2,3,1,12,
	8,2,2,2,2,	10,2,2,	2,2,3,1,2,2,4,	8	 
};


unsigned char code QingTianNote[] = {   //����
	 3,3,3,3,2,3,2,1, 19,1,2,3,4,3,2,1,2,  3,3,3,3,2,3,2,1,21,  
	 1,1,1,1,21,1,1,1,1,1,1,21,1,1,  1,1,1,1,21,1,1,1,1,1,1,5,5,5,  22,5,5,5,5,5,5,5,5,5,5,5,5,4,3,3,  3,21,1,1,1,1,  20,21,1,5,4,3,1,1,  1,22,1,1,1,1,3,1,  20,21,1,5,4,3,1,2,
	 2,22,22,  3,2,4,3,3,1,5,7,  8,7,5,1,1,1,1,6,6,  22,6,5,5,5,5,4,3,  2,3,4,3,3, 3,4,5,3,3,4,5,7, 9,7,8,8,8,22,8,  8,5,5,6,5,4,4,2,3,  4,5,6,1,6,7,7,  3,2,4,3,3,1,5,7,
	 8,7,5,1,1,1,1,6,6, 22,6,5,5,5,5,4,3,  2,3,4,3,3, 3,4,5,3,3,4,5,7,  9,7,8,8,8,22,8,8,5,5,6,5,4,20,21,  1,2,3,2,2,3,1,1, 0xff
};


unsigned char code QingTianBeat[] = {
  4,4,4,4,2,2,4,8,4,4,4,4,4,4,4,2,2,4,4,4,4,2,2,4,6,2,2,2,2,2,2,4,2,2,2,2,2,2,4,2,
  2,2,2,2,2,4,2,2,2,2,2,2,4,2,2,2,2,2,2,4,2,2,2,2,2,2,2,2,2,16,8,2,2,2,2,4,4,4,4,4,4,4,4,8,8,2,2,2,2,4,4,4,4,4,4,4,4,4,4,16,8,8,                                        
  4,4,4,4,4,4,4,4,4,4,2,2,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,16,4,4,4,4,4,4,4,4,4,4,4,4,8,4,4,4,4,4,4,2,2,4,4,4,4,4,4,4,6,2,8,4,4,4,4,4,4,4,4,4,4,2,2,4,4,4,4,4,4,    
  4,4,4,4,4,4,4,4,4,4,4,4,16,4,4,4,4,4,4,4,4,4,4,4,4,8,4,4,4,4,4,4,4,4,4,4,4,4,4,4,8,4,4,                                                                            
};
char* code SongName[]={
	"    QingTian    ",
	"  YiLuXiangBei  ",
	"ZhiDuanQingChang",
	"  HappyBirthday ",
	"    TwoTigers   ",
	"  Little Star   "
};

uint8 KeyDown( void )
{
	col1 = 0;
	col2 = 0;
	col3 = 0;
	col4 = 0;
	/* ����ȫ���õ� */
	if ( (row1 == 0) || (row2 == 0) || (row3 == 0) || (row4 == 0) )
		/* ������һ���߶���״̬Ϊ�� */
	{
		Delay( 10 );
		/* ��ʱ���� */
		if ( (row1 == 0) || (row2 == 0) || (row3 == 0) || (row4 == 0) )
			/* �ٴζ�����״̬��������һ���߶���״̬Ϊ�� */
			return(1);
		/* ����1�������м��̰��� */ else
			return(0);
		/* ����0�������޼��̰��� */
	} else
		return(0);
}
uint8 KeyUp( void )
{
	col1 = 0;
	col2 = 0;
	col3 = 0;
	col4 = 0;
	if ( (row1 == 1) && (row2 == 1) && (row3 == 1) && (row4 == 1) ) {
		Delay( 10 );
		if ( (row1 == 1) && (row2 == 1) && (row3 == 1) && (row4 == 1) )
			return(1);
		else
			return(0);
	} else
		return(0);
}
uint8   KeyNum( void )
{
	uint8 KeyTemp;
	KeyTemp = 0;
	if ( KeyDown() == 1 ) {
		col1 = 0;
		col2 = 1;
		col3 = 1;
		col4 = 1;
		/* ������1�õͣ����������ø� */
		if ( row1 == 0 )
			KeyTemp = 1;
		/* ������1����״̬Ϊ�ͣ����������1������ */
		if ( row2 == 0 )
			KeyTemp = 5;
		/* ������2����״̬Ϊ�ͣ����������5������ */
		if ( row3 == 0 )
			KeyTemp = 9;
		/* ������3����״̬Ϊ�ͣ����������9������ */
		if ( row4 == 0 )
			KeyTemp = 13;
		/* ������3����״̬Ϊ�ͣ����������13������ */
		col1 = 1;
		col2 = 0;
		col3 = 1;
		col4 = 1;
		if ( row1 == 0 )
			KeyTemp = 2;
		/* ������1����״̬Ϊ�ͣ����������2������ */
		if ( row2 == 0 )
			KeyTemp = 6;
		/* ������2����״̬Ϊ�ͣ����������6������ */
		if ( row3 == 0 )
			KeyTemp = 10;
		/* ������3����״̬Ϊ�ͣ����������10������ */
		if ( row4 == 0 )
			KeyTemp = 14;
		/* ������3����״̬Ϊ�ͣ����������14������ */
		col1 = 1;
		col2 = 1;
		col3 = 0;
		col4 = 1;
		if ( row1 == 0 )
			KeyTemp = 3;
		/* ������1����״̬Ϊ�ͣ����������1������ */
		if ( row2 == 0 )
			KeyTemp = 7;
		/* ������2����״̬Ϊ�ͣ����������8������ */
		if ( row3 == 0 )
			KeyTemp = 11;
		/* ������3����״̬Ϊ�ͣ����������15������ */
		if ( row4 == 0 )
			KeyTemp = 15;
		/* ������3����״̬Ϊ�ͣ����������15������ */
		col1 = 1;
		col2 = 1;
		col3 = 1;
		col4 = 0;
		if ( row1 == 0 )
			KeyTemp = 4;
		/* ������1����״̬Ϊ�ͣ����������4������ */
		if ( row2 == 0 )
			KeyTemp = 8;
		/* ������2����״̬Ϊ�ͣ����������8������ */
		if ( row3 == 0 )
			KeyTemp = 12;
		/* ������3����״̬Ϊ�ͣ����������12������ */
		if ( row4 == 0 )
			KeyTemp = 16;
		/* ������3����״̬Ϊ�ͣ����������16������ */
		return(KeyTemp);
	} else
		return(0);
	/* �ް������� */
}




void PlayControl(unsigned char Num)//����ѡ���� 
{
	switch(Num) {
		case 6:
			SongNote = XiaoXingXingNote;
			SongBeat =  XiaoXingXingBeat;
			//display_tab("  Now Playing     Little Star  ");
			break;
		case 5:
			SongNote = TwoTigerNote;
			SongBeat =TwoTigerBeat;
			//display_tab("  Now Playing      TwoTigers   ");
			break;
		case 4:
			SongNote = HappybirthdayNote;
			SongBeat =HappybirthdayBeat;
			//display_tab("  Now Playing    Happybirthday ");
			break;
		case 3:
			SongNote = ZhiduanqingchangNote;
			SongBeat =ZhiduanqingchangBeat;
			//display_tab("  Now Playing   zhiduanqingchang");
			break;
		case 2:
			SongNote = YiLuXiangBeiNote;
			SongBeat =YiLuXiangBeiBeat;
			break;				  
		case 1:
			SongNote = QingTianNote;
			SongBeat = QingTianBeat;
			break;	
		/*case 4:
			SongNote = XiaoXingXingNote;
			SongBeat =  XiaoXingXingBeat;
			//display_tab("  Now Playing     Little Star  ");
			break;
		case 3:
			SongNote = TwoTigerNote;
			SongBeat =TwoTigerBeat;
			//display_tab("  Now Playing      TwoTigers   ");
			break;
		case 2:
			SongNote = HappybirthdayNote;
			SongBeat =HappybirthdayBeat;
			//display_tab("  Now Playing    Happybirthday ");
			break;
		case 1:
			SongNote = ZhiduanqingchangNote;
			SongBeat =ZhiduanqingchangBeat;
			//display_tab("  Now Playing   zhiduanqingchang");
			break;*/
		default:
			break;
	}
}

/*���Ʒ��������� */
void InterruptTimer0() interrupt 1 {
	TH0 = T0RH;   //���¼�������ֵ
	TL0 = T0RL;
	tmrflag = 1;
	if (enable)   //ʹ��ʱ��ת���������Ƶ�ƽ
		BUZZ = ~BUZZ;
	else          //δʹ��ʱ�رշ�����
		BUZZ = 1;
}
/*ͨ����ʱ��1��ͣ*/

void CountDown(uint8 s){
	sprintf(DispBuf,"   Count Down    %2d second left ",(unsigned int)s);
	display_tab(DispBuf);
}

void InterruptTimer1() interrupt 3 { 
	TH1	= T1RH;
	TL1	= T1RL;
	count--;
	FY=0;
	if(!count) {
		TR1=0;
		FY=1;
		enable = 1;
		TR0	= 1;
		stopOver=1;
		//displaySong();
	}

}

void displaySong(void){
	k=Num-1;
	sprintf(DispBuf,"  Now Playing   %s",SongName[k]);
	display_tab(DispBuf);
}

void main()
{
	unsigned char beat;   //��ǰ��������
	unsigned char note;   //��ǰ���Ķ�Ӧ������
	unsigned int time = 0;      //��ǰ���ļ�ʱ
	unsigned int beatTime = 0;  //��ǰ������ʱ��
	unsigned int soundTime = 0; //��ǰ�����跢��ʱ��
	enable = 0;
	EA = 1;       //ʹ��ȫ���ж�
	TMOD = 0x11;  //����T0��T1������ģʽ1
	TH0 = T0RH;
	TL0 = T0RL;
	ET0 = 1;      //ʹ��T0�ж�
	ET1 = 1;      //ʹ��T1�ж�
	TR1=0;        //�ر�T1
	FY=1;		//��ʼ�Ÿ�ģʽ 
	TR0 = 1;	//����T0 
	P2=0xFF;	// �����a-hλ���� ��������̨�����Ϊ������ 
	Intlcd();
  display_tab("Electronic Piano     Group 3    ");
	Delay(3000);
	PlayControl(Num);
	Num=1;
	displaySong();
	//sprintf(DispBuf,"  Now Playing   %s",SongName[Num-1]);
	//display_tab(DispBuf);
	enable = 1;
	while ( 1 ) {
		if ( KeyDown() ) {
			k = KeyNum();
			/* ���ü���ɨ�躯�� */
			if ( k != 0 ) {
				FY = 0; //��Ϊ������ģʽ 
				//P2 = LedChar[k]; //�������λ
				if(k<=13){ 
				T0RH = NoteReload[k] >> 8; 
				T0RL = NoteReload[k];
				/*��������Ƶ�ʿ���*/ 
				
				T1RH	= 0xEC;
				T1RL	= 0x78;
				/*T1��ʱ5ms*/
				
				TR0 = 1; //��T0
				TR1 = 0; //��T1
				count=1000;
				/* ��ʼ���� */
				sprintf(DispBuf,"    Key Down        Key = %2d    ",(unsigned int)k);
				display_tab(DispBuf);
				enable = 1;
				
				while ( KeyUp() == 0 );
				/* ��û���ɿ���������ȴ����ȴ��ڼ䵯������� */
				enable = 0;
				TR0	= 0;
				/* �������ɿ�����ֹͣ������������������� */
				TR1=1;//��ʼ��ʱ5s
				}else if(k==14){
					enable = ~enable;
					display_tab("Electronic Piano    Pausing     ");
					while ( KeyUp() == 0 );
					if(enable){
							FY=1;
							displaySong();
						}
					//FY=~FY;
				}else if(k>=15){
					if(k==15){
				   	while ( KeyUp() == 0 );
					if(Num == All)
						Num = 1;
					else
						Num++;
					
				}else{
				while ( KeyUp() == 0 );
					if(Num == 1)
						Num = All;
					else
						Num--;
					}
					PlayControl(Num);
					beat=0;
					time = 0;      
					beatTime = 0; 
					soundTime = 0; 
					Delay(100);
					displaySong();
					FY=1; 
					continue; 
				}
			}
		} else {
			if(stopOver){
				displaySong();
				stopOver=0;
			}
			if(TR1){
				if(count==800) CountDown(4);
				else if(count==600)	CountDown(3);
				else if(count==400) CountDown(2);
				else if(count==200) CountDown(1);		
			}
			if(FY==1) {/* ���ຯ�� */
				//P2 = LedChar[Num]; //�������ʾ��ǰ�������
				while (!tmrflag);  //ÿ�ζ�ʱ���ж���ɺ󣬼�Ⲣ�������
        		tmrflag = 0;
				if (time == 0) { //��ǰ���Ĳ���������һ���½���
					note = SongNote[beat] - 1;
					if(note == 0xfe) {
						if(Num == All)
							Num = 1;
						else
							Num++;
						PlayControl(Num);
						beat=0;
						time = 0;      
						beatTime = 0; 
						soundTime = 0; 
						Delay(100);
						displaySong();
						continue; 
					}
					
					T0RH = NoteReload[note] >> 8;
					T0RL = NoteReload[note];
					
					beatTime = (SongBeat[beat] * NoteFrequ[note]) >> 2;//���������ʱ�䣬����2λ�൱�ڳ�4����λ����������Լӿ�ִ���ٶ�
					soundTime = beatTime - (beatTime >> 2);//���㷢��ʱ�䣬
					enable = 1;  //ָʾ��������ʼ����
					if (note == 21)//���׵�0 �ȴ�
						enable =0;
					time++;
				} else { //��ǰ����δ��������ǰ����
					if (time >= beatTime) { //��ǰ����ʱ�䵽�������ʱ��ʱ���㣬
						//������������������׼�������½���
						time = 0;
						beat++;
					} else { //��ǰ����ʱ��δ�ﵽ��ʱ��ʱ��
						time++;   //�ۼ�ʱ�����
						if (time == soundTime) { //���﷢��ʱ���ָʾ�رշ�������
							//����0.25*��ʱ��ľ��������
							enable = 0;         //����������������������
						}
					}
				}	
			}
		}
	}
}