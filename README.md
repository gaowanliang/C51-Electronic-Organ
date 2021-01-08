# C51-Electronic-Organ
Analog electronic organ / music box system based on AT89C52  通过AT89C52单片机实现的模拟电子琴/八音盒系统

# Introduction / 简介

First, scan the matrix keyboard continuously. If no key is pressed, enter the octave box program and play through the buzzer. The program will control the tone of the buzzer by setting the initial value of the timer, then play according to the pre-defined tone and beat, and play the next tone after playing one tone. Define the ending character in the pre-defined tone table. If the end is detected, the next song will be played. If a key is pressed in the process of playing, you can enter the electronic organ program, and the user can play by himself. In the process of pressing the key, the initial value of the set timer controls the tone of the buzzer, and the specific set tone should be determined according to the pressed key. If the user does not press any more, the timer will start. When it reaches 5 seconds, it will return to the octave box program. At the same time, press the defined special key to realize the functions of previous song, next song, pause, etc. The above is the overall design idea according to the functions of the system.

首先对矩阵键盘进行持续扫描，如果没有按键按下，则进入八音盒程序，通过蜂鸣器进行播放，程序会通过置位定时器的初始值来控制蜂鸣器的音调，之后按照预先定义好的音调和节拍进行播放，当播放完一个音调之后再播放下一个音调。在预先定义好的音调表中定义结束符，如果监测到结束了，就播放下一曲。如果在播放的过程中，有按键按下，则进入电子琴程序，用户可以进行自行弹奏。在按键按下的过程中，置位定时器的初始值来控制蜂鸣器的音调，具体置位的音调要根据按下的按键进行决定。如果用户不再按下，则开始计时，当达到5秒后，重新回到八音盒程序。同时按下定义的特殊按键，可以实现上一首下一首，暂停等功能。以上是根据系统应具备的功能，提出的整体设计思路。

# File directory / 文件目录

main
├─ 1602sound
│    ├─ ExtDll.iex
│    ├─ LCD1602.LST
│    ├─ LCD1602.OBJ
│    ├─ LCD1602.c  <- 1602 LCD C program file / 1602 液晶显示C程序文件
│    ├─ LCD1602.h  <- 1602 LCD header file / 1602 液晶显示头文件
│    ├─ main
│    ├─ main.LST
│    ├─ main.M51
│    ├─ main.MAP
│    ├─ main.OBJ
│    ├─ main.SBR
│    ├─ main.c    <- main C program / 主C程序
│    ├─ main.hex  <- Compilation results / 编译结果
│    ├─ main.lnp
│    ├─ main.plg
│    ├─ main.uvgui.gaowanliang
│    ├─ main.uvgui_gaowanliang.bak
│    ├─ main.uvopt
│    ├─ main.uvproj  <- Keil Project
│    ├─ main_uvopt.bak
│    └─ main_uvproj.bak
└─ shixun.DSN <- Protues simulation circuit diagram / Protues 仿真电路图
