
#include <stdint.h>
#define COLS 80
#define ROWS 25
static volatile uint16_t* const video=(volatile uint16_t*)0xB8000;
static int cx=0,cy=0;
static uint8_t attr=0x1f;
static inline uint8_t inb(uint16_t p){uint8_t v;__asm__ volatile("inb %1,%0":"=a"(v):"Nd"(p));return v;}
static inline void outb(uint16_t p,uint8_t v){__asm__ volatile("outb %0,%1"::"a"(v),"Nd"(p));}
static void scroll(){if(cy<ROWS)return;for(int y=1;y<ROWS;y++)for(int x=0;x<COLS;x++)video[(y-1)*COLS+x]=video[y*COLS+x];for(int x=0;x<COLS;x++)video[(ROWS-1)*COLS+x]=((uint16_t)attr<<8)|' ';cy=ROWS-1;}
static void put(char c){if(c=='\n'){cx=0;cy++;scroll();return;}if(c=='\r'){cx=0;return;}if(c=='\b'){if(cx>0){cx--;video[cy*COLS+cx]=((uint16_t)attr<<8)|' ';}return;}video[cy*COLS+cx]=((uint16_t)attr<<8)|(uint8_t)c;if(++cx==COLS){cx=0;cy++;scroll();}}
static void print(const char*s){while(*s)put(*s++);}
static void clear(){for(int i=0;i<COLS*ROWS;i++)video[i]=((uint16_t)attr<<8)|' ';cx=cy=0;}
static int eq(const char*a,const char*b){while(*a&&*a==*b){a++;b++;}return *a==*b;}
static int prefix(const char*a,const char*b){while(*b){if(*a++!=*b++)return 0;}return 1;}
static char line[128];static int len=0;static uint32_t commands=0;
static void prompt(){print("\nmeplay:/ $ ");}
static void num(uint32_t n){char b[12];int i=0;do{b[i++]='0'+n%10;n/=10;}while(n);while(i)put(b[--i]);}
static void run(){
 line[len]=0;commands++;
 if(eq(line,"help"))print("Commands: help, clear, cls, version, uname, about, echo TEXT,\n          ls, pwd, whoami, apps, settings, mem, uptime,\n          reboot, shutdown, color blue, color black\n");
 else if(eq(line,"clear")||eq(line,"cls"))clear();
 else if(eq(line,"version")||eq(line,"uname"))print("MePlay OS 1.0-dev | x86 32-bit | native C kernel\n");
 else if(eq(line,"about"))print("MePlay is a native experimental console OS.\nRunning on emulated x86 hardware, not inside JavaScript.\n");
 else if(prefix(line,"echo ")) {print(line+5);put('\n');}
 else if(eq(line,"echo"))put('\n');
 else if(eq(line,"ls"))print("system/   apps/   users/   games/  (virtual placeholders)\n");
 else if(eq(line,"pwd"))print("/\n");
 else if(eq(line,"whoami"))print("player\n");
 else if(eq(line,"apps"))print("Shell [running]   MeMart [planned]   Games [planned]\n");
 else if(eq(line,"settings"))print("Settings service not implemented yet.\n");
 else if(eq(line,"mem"))print("Kernel loaded in 32-bit protected mode. Memory manager: TODO.\n");
 else if(eq(line,"uptime")){print("Commands executed: ");num(commands);put('\n');}
 else if(eq(line,"color blue")){attr=0x1f;clear();}
 else if(eq(line,"color black")){attr=0x0f;clear();}
 else if(eq(line,"reboot")){print("Rebooting...\n");outb(0x64,0xfe);for(;;)__asm__ volatile("hlt");}
 else if(eq(line,"shutdown")){print("Halting MePlay OS. Restart VM to boot again.\n");__asm__ volatile("cli");for(;;)__asm__ volatile("hlt");}
 else if(len){print("Unknown command: ");print(line);print("\nType help for available commands.\n");}
 len=0;prompt();
}
static const char unshift[128]={
 [2]='1',[3]='2',[4]='3',[5]='4',[6]='5',[7]='6',[8]='7',[9]='8',[10]='9',[11]='0',
 [12]='-',[13]='=',[16]='q',[17]='w',[18]='e',[19]='r',[20]='t',[21]='y',[22]='u',[23]='i',[24]='o',[25]='p',
 [26]='[',[27]=']',[30]='a',[31]='s',[32]='d',[33]='f',[34]='g',[35]='h',[36]='j',[37]='k',[38]='l',
 [39]=';',[40]='\'',[41]='`',[43]='\\',[44]='z',[45]='x',[46]='c',[47]='v',[48]='b',[49]='n',[50]='m',
 [51]=',',[52]='.',[53]='/',[57]=' '
};
static int shift=0;
static void key(uint8_t scan){
 int released=scan&0x80;uint8_t code=scan&0x7f;
 if(code==42||code==54){shift=!released;return;}
 if(released)return;
 if(code==28){put('\n');run();return;}
 if(code==14){if(len){len--;put('\b');}return;}
 if(code==1){len=0;print("^C");prompt();return;}
 char c=unshift[code];if(!c||len>=126)return;
 if(shift&&c>='a'&&c<='z')c-=32;
 line[len++]=c;put(c);
}
void kernel_main(void){
 outb(0x21,0xff);outb(0xa1,0xff);
 clear();
 print("==========================================================================\n");
 print("                       MePlay OS - Native Shell V1\n");
 print("==========================================================================\n");
 print("Custom C kernel | x86 protected mode | QEMU / v86\n");
 print("Type 'help' to see available commands.\n");
 prompt();
 uint8_t e0=0;
 for(;;){
   if(inb(0x64)&1){uint8_t k=inb(0x60);if(k==0xe0){e0=1;continue;}if(e0){e0=0;continue;}key(k);}
   __asm__ volatile("pause");
 }
}
