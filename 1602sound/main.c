#include <STC89C5xRC.H>
#include <stdio.h>
#include <LCD1602.h>
#define All 6          //歌曲总数
#define Xtal	11030000
/* 赋晶振值 */
#define uchar	unsigned char
typedef unsigned char	uint8;
/* 无符号8位整型变量 */
typedef signed char	int8;
/* 有符号8位整型变量 */
typedef unsigned short	uint16;
/* 无符号16位整型变量 */
typedef signed short	int16;
/* 有符号16位整型变量 */
typedef unsigned int	uint32;
/* 无符号32位整型变量 */
typedef signed int	int32;
/* 有符号32位整型变量 */
typedef float		fp32;
/* 单精度浮点数（32位长度） */
typedef double		fp64;
/* 双精度浮点数（64位长度） */
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
/* 定时器0计数初值 */
uchar	STL0;
uchar	T1RH;
/* 定时器1计数初值 */
uchar	T1RL;
bit	FY		= 0;
/* 放乐曲时FY=1, 电子琴弹奏时FY=0 */
uchar	k;


sbit BUZZ = P3^7;  //蜂鸣器控制引脚
sbit digTube = P2; //数码管显示控制

unsigned char Num = 1; //第几首歌
bit enable = 1;   //蜂鸣器发声使能标志
bit tmrflag = 0;  //定时器中断完成标志
unsigned char T0RH = 0xFF;  //T0重载值的高字节
unsigned char T0RL = 0x00;  //T0重载值的低字节
uint count = 1000;//定时器定时1000次
unsigned char DispBuf[50];//液晶显示字符缓存区
bit stopOver=0;


void Delay( uint16 count )
{
	uint8 i;
	while ( --count != 0 ) {
		for ( i = 0; i < 125; i++ )
			;
		/* ";" 表示空语句,CPU空转。 */
	}
	/* i 从0加到125，在12M晶体下CPU大概耗时1毫秒 */
}

void PlayControl(unsigned char Num);//歌曲选择函数
void Play();//演奏函数

unsigned char code LedChar[] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8,0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E};//数码管显示字符转换表
unsigned int code NoteFrequ[] = {  //中音1-7低音1-7和高音1-7对应频率列表
	523,  587,  659,  698,  784,  880,  988,  //中音1-7
	1047, 1175, 1319, 1397, 1568, 1760, 1976,  //高音1-7
	262, 294, 330, 349, 392, 440, 494,300//低音1-7  最后一个纯属凑数
};
unsigned int code NoteReload[] = { //中音1-7和高音1-7对应的定时器重载值
	65536 - (Xtal/12) / (523*2),  //中音1
	65536 - (Xtal/12) / (587*2),  //2
	65536 - (Xtal/12) / (659*2),  //3
	65536 - (Xtal/12) / (698*2),  //4
	65536 - (Xtal/12) / (784*2),  //5
	65536 - (Xtal/12) / (880*2),  //6
	65536 - (Xtal/12) / (988*2),  //7
	65536 - (Xtal/12) / (1047*2), //高音1
	65536 - (Xtal/12) / (1175*2), //2
	65536 - (Xtal/12) / (1319*2), //3
	65536 - (Xtal/12) / (1397*2), //4
	65536 - (Xtal/12) / (1568*2), //5
	65536 - (Xtal/12) / (1760*2), //6
	65536 - (Xtal/12) / (1976*2), //7
	65536 - (Xtal/12) / (262*2), //低音1
	65536 - (Xtal/12) / (294*2), //2
	65536 - (Xtal/12) / (330*2), //3
	65536 - (Xtal/12) / (349*2), //4
	65536 - (Xtal/12) / (392*2), //5
	65536 - (Xtal/12) / (440*2), //6
	65536 - (Xtal/12) / (494*2), //7
	65536 - (Xtal/12) / (300*2), //纯属凑数
};

unsigned char *SongNote, *SongBeat;//指针
unsigned char code XiaoXingXingNote[] = {   //小星星
	1, 1, 5, 5,    6, 6, 5,    4, 4, 3, 3,    2, 2, 1,
	5, 5, 4, 4,    3, 3, 2,    5, 5, 4, 4,    3, 3, 2 ,
	1, 1, 5, 5,    6, 6, 5,    4, 4, 3, 3,    2, 2, 1, 0xff
};

unsigned char code XiaoXingXingBeat[] = {
	4, 4, 4, 4,    4, 4, 8,    4, 4, 4, 4,    4, 4, 8,
	4, 4, 4, 4,    4, 4, 8,    4, 4, 4, 4,    4, 4, 8,
	4, 4, 4, 4,    4, 4, 8,    4, 4, 4, 4,    4, 4, 8,
};
unsigned char code TwoTigerNote[] = {    //两只老虎音符表
	1,   2,   3, 1,    1,   2,   3, 1,   3, 4, 5,   3, 4, 5,
	5,6, 5,4, 3, 1,    5,6, 5,4, 3, 1,   1, 5, 1,   1, 5, 1,0xff
};

unsigned char code TwoTigerBeat[] = {    //两只老虎节拍表，4表示一拍，1就是1/4拍，8就是2拍
	4,   4,   4, 4,    4,   4,   4, 4,   4, 4, 8,   4, 4, 8,
	3,1, 3,1, 4, 4,    3,1, 3,1, 4, 4,   4, 4, 8,   4, 4, 8,
};
unsigned char code HappybirthdayNote[] = {  //生日快乐歌
	5, 5, 6, 5, 8, 7, 22, 5, 5, 6, 5, 9, 8, 5, 5, 12, 10, 8, 7, 6, 11, 11,  10, 8, 9, 8,0xff
};
unsigned char code HappybirthdayBeat[] = {
	2, 2, 4, 4, 4, 4, 4, 2, 2,  4, 4, 4, 8, 2, 2, 4, 4, 4, 4, 8, 2, 2, 4, 4, 4, 8
};
unsigned char code         ZhiduanqingchangNote[] = { //纸短情长
	//8, 9, 10, 9, 8, 6, 22, 11, 10, 9, 22, 7, 8, 9, 7, 7, 5, 22, 7, 9, 8, 8, 3, 4, 2, 3, 1, 1, 5, 2,  //前奏
	19, 3, 3, 3, 2, 3, 1, 2, 2, 2, 1, 2, 5, 1, 1, 1, 20, 1, 20, 19,     2, 2, 2, 3, 20, 19, 4, 4, 4, 3, 4, 1, 2, 2, 2, 1, 2, 5,
	1, 1, 1, 20, 1, 20, 3, 2, 2, 1, 1, 22, 22, 22, 1, 1, 2,             3, 3, 3, 22, 3, 2, 1, 21, 6, 5, 22, 5, 6, 7, 8, 3, 3, 22, 8, 7, 8,
	7, 3, 5, 5, 6, 8, 5, 6, 22, 6, 5, 4,5, 3, 2, 1, 20, 1,               3, 2, 2, 1, 2, 5, 1, 2, 5, 22, 22, 22, 1, 1, 2,
	3, 3, 2, 3, 22, 3, 2, 1, 5, 5, 3, 5, 22, 5, 6, 7,                   8, 8, 8, 8, 8, 7, 6, 7, 6, 3, 5, 5, 5, 6, 8, 5, 6, 22, 6, 5, 4,
	5, 3, 2, 1, 6, 1, 3, 2, 2, 1, 2, 1, 20, 1, 22,                      22, 20, 1, 3, 2, 2, 22, 2, 1, 2, 1, 1, 22, 20, 1, 1, 0xff
};
unsigned char code ZhiduanqingchangBeat[] = {
	//2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 8, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 8, 8, 12, 4, 5, 4, 8, 8, 4,   //前奏
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


unsigned char code QingTianNote[] = {   //晴天
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
	/* 列线全部置低 */
	if ( (row1 == 0) || (row2 == 0) || (row3 == 0) || (row4 == 0) )
		/* 若有任一行线读回状态为低 */
	{
		Delay( 10 );
		/* 延时消抖 */
		if ( (row1 == 0) || (row2 == 0) || (row3 == 0) || (row4 == 0) )
			/* 再次读行线状态，若有任一行线读回状态为低 */
			return(1);
		/* 返回1，表明有键盘按下 */ else
			return(0);
		/* 返回0，表明无键盘按下 */
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
		/* 将列线1置低，其他列线置高 */
		if ( row1 == 0 )
			KeyTemp = 1;
		/* 若行线1读回状态为低，则表明按键1被按下 */
		if ( row2 == 0 )
			KeyTemp = 5;
		/* 若行线2读回状态为低，则表明按键5被按下 */
		if ( row3 == 0 )
			KeyTemp = 9;
		/* 若行线3读回状态为低，则表明按键9被按下 */
		if ( row4 == 0 )
			KeyTemp = 13;
		/* 若行线3读回状态为低，则表明按键13被按下 */
		col1 = 1;
		col2 = 0;
		col3 = 1;
		col4 = 1;
		if ( row1 == 0 )
			KeyTemp = 2;
		/* 若行线1读回状态为低，则表明按键2被按下 */
		if ( row2 == 0 )
			KeyTemp = 6;
		/* 若行线2读回状态为低，则表明按键6被按下 */
		if ( row3 == 0 )
			KeyTemp = 10;
		/* 若行线3读回状态为低，则表明按键10被按下 */
		if ( row4 == 0 )
			KeyTemp = 14;
		/* 若行线3读回状态为低，则表明按键14被按下 */
		col1 = 1;
		col2 = 1;
		col3 = 0;
		col4 = 1;
		if ( row1 == 0 )
			KeyTemp = 3;
		/* 若行线1读回状态为低，则表明按键1被按下 */
		if ( row2 == 0 )
			KeyTemp = 7;
		/* 若行线2读回状态为低，则表明按键8被按下 */
		if ( row3 == 0 )
			KeyTemp = 11;
		/* 若行线3读回状态为低，则表明按键15被按下 */
		if ( row4 == 0 )
			KeyTemp = 15;
		/* 若行线3读回状态为低，则表明按键15被按下 */
		col1 = 1;
		col2 = 1;
		col3 = 1;
		col4 = 0;
		if ( row1 == 0 )
			KeyTemp = 4;
		/* 若行线1读回状态为低，则表明按键4被按下 */
		if ( row2 == 0 )
			KeyTemp = 8;
		/* 若行线2读回状态为低，则表明按键8被按下 */
		if ( row3 == 0 )
			KeyTemp = 12;
		/* 若行线3读回状态为低，则表明按键12被按下 */
		if ( row4 == 0 )
			KeyTemp = 16;
		/* 若行线3读回状态为低，则表明按键16被按下 */
		return(KeyTemp);
	} else
		return(0);
	/* 无按键按下 */
}




void PlayControl(unsigned char Num)//歌曲选择函数 
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

/*控制蜂鸣器发声 */
void InterruptTimer0() interrupt 1 {
	TH0 = T0RH;   //重新加载重载值
	TL0 = T0RL;
	tmrflag = 1;
	if (enable)   //使能时反转蜂鸣器控制电平
		BUZZ = ~BUZZ;
	else          //未使能时关闭蜂鸣器
		BUZZ = 1;
}
/*通过定时器1暂停*/

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
	unsigned char beat;   //当前节拍索引
	unsigned char note;   //当前节拍对应的音符
	unsigned int time = 0;      //当前节拍计时
	unsigned int beatTime = 0;  //当前节拍总时间
	unsigned int soundTime = 0; //当前节拍需发声时间
	enable = 0;
	EA = 1;       //使能全局中断
	TMOD = 0x11;  //配置T0，T1工作在模式1
	TH0 = T0RH;
	TL0 = T0RL;
	ET0 = 1;      //使能T0中断
	ET1 = 1;      //使能T1中断
	TR1=0;        //关闭T1
	FY=1;		//开始放歌模式 
	TR0 = 1;	//启动T0 
	P2=0xFF;	// 数码管a-h位清零 ，本试验台数码管为共阳极 
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
			/* 调用键盘扫描函数 */
			if ( k != 0 ) {
				FY = 0; //改为电子琴模式 
				//P2 = LedChar[k]; //数码管置位
				if(k<=13){ 
				T0RH = NoteReload[k] >> 8; 
				T0RL = NoteReload[k];
				/*蜂鸣器振动频率控制*/ 
				
				T1RH	= 0xEC;
				T1RL	= 0x78;
				/*T1定时5ms*/
				
				TR0 = 1; //开T0
				TR1 = 0; //关T1
				count=1000;
				/* 开始计数 */
				sprintf(DispBuf,"    Key Down        Key = %2d    ",(unsigned int)k);
				display_tab(DispBuf);
				enable = 1;
				
				while ( KeyUp() == 0 );
				/* 若没有松开按键，则等待，等待期间弹奏该音符 */
				enable = 0;
				TR0	= 0;
				/* 若按键松开，则停止计数，不产生脉冲输出 */
				TR1=1;//开始计时5s
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
			if(FY==1) {/* 演奏函数 */
				//P2 = LedChar[Num]; //数码管显示当前歌曲序号
				while (!tmrflag);  //每次定时器中断完成后，检测并处理节拍
        		tmrflag = 0;
				if (time == 0) { //当前节拍播完则启动一个新节拍
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
					
					beatTime = (SongBeat[beat] * NoteFrequ[note]) >> 2;//计算节拍总时间，右移2位相当于除4，移位代替除法可以加快执行速度
					soundTime = beatTime - (beatTime >> 2);//计算发声时间，
					enable = 1;  //指示蜂鸣器开始发声
					if (note == 21)//简谱的0 等待
						enable =0;
					time++;
				} else { //当前节拍未播完则处理当前节拍
					if (time >= beatTime) { //当前持续时间到达节拍总时间时归零，
						//并递增节拍索引，以准备启动新节拍
						time = 0;
						beat++;
					} else { //当前持续时间未达到总时间时，
						time++;   //累加时间计数
						if (time == soundTime) { //到达发声时间后，指示关闭蜂鸣器，
							//插入0.25*总时间的静音间隔，
							enable = 0;         //用以区分连续的两个节拍
						}
					}
				}	
			}
		}
	}
}