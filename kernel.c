/* MePlay OS V1: freestanding 32-bit x86 kernel, no libc or host OS. */
#include <stdint.h>
#define W 320
#define H 200
static volatile uint8_t *const fb=(volatile uint8_t*)0xA0000;
static inline uint8_t inb(uint16_t p){uint8_t v;__asm__ volatile("inb %1,%0":"=a"(v):"Nd"(p));return v;}
static inline void outb(uint16_t p,uint8_t v){__asm__ volatile("outb %0,%1"::"a"(v),"Nd"(p));}
static void rect(int x,int y,int w,int h,uint8_t c){for(int j=0;j<h;j++)for(int i=0;i<w;i++){int px=x+i,py=y+j;if(px>=0&&px<W&&py>=0&&py<H)fb[py*W+px]=c;}}
/* Five-column uppercase font. Glyph rows are stored as 5-bit masks. */
static const uint8_t glyphs[41][7]={
{14,17,17,31,17,17,17}, /* A */ {30,17,17,30,17,17,30}, {14,17,16,16,16,17,14},
{30,17,17,17,17,17,30}, {31,16,16,30,16,16,31}, {31,16,16,30,16,16,16},
{14,17,16,23,17,17,15}, {17,17,17,31,17,17,17}, {31,4,4,4,4,4,31},
{7,2,2,2,18,18,12}, {17,18,20,24,20,18,17}, {16,16,16,16,16,16,31},
{17,27,21,21,17,17,17}, {17,25,21,19,17,17,17}, {14,17,17,17,17,17,14},
{30,17,17,30,16,16,16}, {14,17,17,17,21,18,13}, {30,17,17,30,20,18,17},
{15,16,16,14,1,1,30}, {31,4,4,4,4,4,4}, {17,17,17,17,17,17,14},
{17,17,17,17,17,10,4}, {17,17,17,21,21,21,10}, {17,17,10,4,10,17,17},
{17,17,10,4,4,4,4}, {31,1,2,4,8,16,31},
{14,17,19,21,25,17,14}, {4,12,4,4,4,4,14}, {14,17,1,2,4,8,31},
{30,1,1,14,1,1,30}, {2,6,10,18,31,2,2}, {31,16,30,1,1,17,14},
{14,16,30,17,17,17,14}, {31,1,2,4,8,8,8}, {14,17,17,14,17,17,14},
{14,17,17,15,1,1,14}, {0,0,0,0,0,0,0}, /* space */
{0,4,4,4,4,0,4}, /* ! */ {0,0,31,0,31,0,0}, /* = */
{0,0,0,31,0,0,0}, /* - */ {0,0,0,0,0,12,12} /* . */};
static int indexof(char c){if(c>='A'&&c<='Z')return c-'A';if(c>='0'&&c<='9')return c-'0'+26;if(c=='!')return 37;if(c=='=')return 38;if(c=='-')return 39;if(c=='.')return 40;return 36;}
static void ch(int x,int y,char c,uint8_t color,int scale){const uint8_t *g=glyphs[indexof(c)];for(int row=0;row<7;row++)for(int col=0;col<5;col++)if(g[row]&(1<<(4-col)))rect(x+col*scale,y+row*scale,scale,scale,color);}
static void str(int x,int y,const char *s,uint8_t color,int scale){while(*s){ch(x,y,*s++,color,scale);x+=6*scale;}}
static void panel(int x,int y,int w,int h,uint8_t bg){rect(x+2,y+3,w,h,233);rect(x,y,w,h,bg);rect(x,y,w,2,15);}
static int selected=0,screen=0,volume=6,theme=0,installed=0,boots=1;
static const char *labels[]={"GAMES","MEMART","MEVATAR","SETTINGS","STORAGE","ALBUM","NEWS","SYSTEM"};
static const uint8_t colors[]={39,50,44,43,54,42,52,46};
static void draw_home(void){
 rect(0,0,W,H,theme?24:231);rect(0,0,W,25,15);rect(0,25,W,2,43);
 rect(10,5,13,13,46);str(29,7,"MEPLAY",24,2);str(242,9,"OS V1",42,1);
 str(11,33,"HOME MENU",25,2);str(226,36,"SELECT APP",25,1);
 for(int i=0;i<8;i++){int x=13+(i%4)*76,y=54+(i/4)*62;rect(x+2,y+3,67,51,233);rect(x,y,67,51,selected==i?43:15);rect(x+3,y+3,61,45,colors[i]);rect(x+17,y+8,33,22,15);str(x+22,y+14,i==0?"G":i==1?"M":i==2?"U":i==3?"S":i==4?"D":i==5?"A":i==6?"N":"I",colors[i],2);str(x+7,y+36,labels[i],25,1);}
 rect(0,182,W,18,15);str(9,188,"ARROWS MOVE  ENTER OPEN  ESC HOME",25,1);
}
static void draw_app(void){rect(0,0,W,H,theme?24:231);rect(0,0,W,26,15);str(10,8,"MEPLAY",24,2);str(222,10,labels[screen-1],42,1);panel(13,36,294,130,15);str(25,47,labels[screen-1],25,2);
 switch(screen){
 case 1:str(25,78,"INSTALLED GAMES",24,1);str(25,100,installed?"COSMIC KART - READY":"NO GAMES INSTALLED",25,1);str(25,128,"OPEN MEMART TO INSTALL",42,1);break;
 case 2:str(25,78,"COSMIC KART",24,2);str(25,107,installed?"ALREADY INSTALLED":"PRESS ENTER TO INSTALL",25,1);str(25,132,"LOCAL DEMO APP",42,1);break;
 case 3:str(25,79,"PLAYER ONE",24,2);str(25,111,"MEVATAR EDITOR COMING SOON",25,1);break;
 case 4:str(25,76,"LEFT RIGHT CHANGE VOLUME",24,1);str(25,95,"VOLUME",25,1);rect(88,95,volume*16,8,43);str(25,118,"ENTER TO CHANGE THEME",42,1);break;
 case 5:str(25,80,"VIRTUAL DISK SUPPORT",24,1);str(25,105,"NOT YET IMPLEMENTED",25,1);break;
 case 6:str(25,87,"NO SCREENSHOTS YET",24,1);break;
 case 7:str(25,87,"WELCOME TO MEPLAY OS V1!",24,1);break;
 case 8:str(25,78,"CUSTOM X86 KERNEL",24,1);str(25,97,"32 BIT PROTECTED MODE",25,1);str(25,116,"QEMU AND V86 COMPATIBLE",42,1);break;
 }
 rect(0,182,W,18,15);str(10,188,"ESC HOME   ENTER ACTION",25,1);
}
static void draw(void){if(!screen)draw_home();else draw_app();}
static void key(uint8_t k){if(k==1){screen=0;draw();return;}if(!screen){if(k==0x4b&&selected%4)selected--;if(k==0x4d&&selected%4<3)selected++;if(k==0x48&&selected>=4)selected-=4;if(k==0x50&&selected<4)selected+=4;if(k==0x1c)screen=selected+1;}else if(screen==2&&k==0x1c)installed=1;else if(screen==4){if(k==0x4b&&volume)volume--;if(k==0x4d&&volume<10)volume++;if(k==0x1c)theme=!theme;}draw();}
void kernel_main(void){(void)boots;outb(0x21,0xff);outb(0xa1,0xff);draw();uint8_t last=0;for(;;){uint8_t status=inb(0x64);if(status&1){uint8_t k=inb(0x60);if(!(k&0x80)&&k!=0xe0){if(k!=last)key(k);last=k;}else if(k&0x80)last=0;}__asm__ volatile("pause");}}
