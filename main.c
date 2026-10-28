/* ===== AKADEMI RPG : open world action RPG untuk GBA (Mode 3, 1 file) ===== */
typedef unsigned char u8; typedef signed char s8; typedef unsigned short u16; typedef unsigned int u32;
#define SW 240
#define SH 160
#define WT 128
#define ABS(a) ((a)<0?-(a):(a))
#define RGB(r,g,b) ((u16)((r)|((g)<<5)|((b)<<10)))
#define WHITE 0x7FFF
#ifdef HOST
#include <string.h>
static u16 h_vram[SW*SH]; static u16 h_keys=0x3FF;
#define VRAM h_vram
#define KEYIN h_keys
#define VSYNC()
#define EWRAM_BSS
static void dma_cpy(void*d,const void*s,u32 n){memcpy(d,s,n*2);}
static void dma_fill(void*d,u16 v,u32 n){u16*p=(u16*)d;while(n--)*p++=v;}
#else
#define REG16(a) (*(volatile u16*)(a))
#define REG32(a) (*(volatile u32*)(a))
#define VRAM ((u16*)0x06000000)
#define KEYIN REG16(0x04000130)
#define VSYNC() while(REG16(0x04000006)<160)
#define EWRAM_BSS __attribute__((section(".sbss")))
static volatile u16 dfv;
static void dma_cpy(void*d,const void*s,u32 n){REG32(0x040000D4)=(u32)s;REG32(0x040000D8)=(u32)d;REG32(0x040000DC)=0x80000000u|n;}
static void dma_fill(void*d,u16 v,u32 n){dfv=v;REG32(0x040000D4)=(u32)&dfv;REG32(0x040000D8)=(u32)d;REG32(0x040000DC)=0x81000000u|n;}
#endif
EWRAM_BSS static u16 bb[SW*SH];

/* ---------- DATA ---------- */
typedef struct{const char*n;u8 t,dmg,cd,spd;}Wt;  /* t:0 melee 1 ranged 2 magic ; spd=range/proj speed */
static const Wt WT_[15]={{"SWORD",0,10,28,30},{"GREATSWORD",0,18,50,40},{"DAGGER",0,6,14,22},{"AXE",0,14,38,32},{"HAMMER",0,20,56,34},
{"SPEAR",0,11,30,44},{"KNUCKLE",0,7,16,22},{"MACE",0,13,34,28},{"BOW",1,9,24,5},{"CROSSBOW",1,14,40,6},{"RIFLE",1,20,60,8},
{"PISTOL",1,7,16,6},{"STAFF",2,12,36,4},{"WAND",2,8,20,5},{"TOME",2,10,28,4}};
static const char*RN[4]={"COMMON","MAGIC","RARE","EPIC"};
static const u16 RC[4]={RGB(22,22,22),RGB(10,16,31),RGB(31,28,6),RGB(31,14,3)};
#define M(t) (1<<(t))
enum{K_SLASH,K_SHOT,K_BLAST,K_HEAL,K_BUFF,K_DASH,K_STEALTH,K_STUN,K_SHIELD};
typedef struct{const char*n;u8 ty;u16 pw,cd;u8 mp;u16 pr;}Sk;
typedef struct{const char*n;u8 hp,atk,def,spd;u16 m;const char*d;u16 c;}Cl;
static const Cl CL[12]={
{"NON CLASS",100,100,100,100,0x7FFF,"KNOWS ALL BASICS",RGB(22,22,24)},
{"GUNSLINGER",70,140,60,100,M(10)|M(11),"BIG DMG, FRAGILE",RGB(24,12,4)},
{"ARCHER",75,105,70,125,M(8)|M(9),"FAST SHOOTER",RGB(8,24,8)},
{"FIGHTER",100,110,100,100,M(0)|M(3),"BALANCED MELEE",RGB(26,6,6)},
{"STRIKER",80,120,70,125,M(6)|M(5),"AGILE BRUISER",RGB(28,18,4)},
{"ASSASSIN",70,130,60,130,M(2),"STEALTH SKIRMISH",RGB(10,6,16)},
{"DEFENDER",150,70,140,80,M(7)|M(4),"TANK AND CC",RGB(8,12,24)},
{"HEALER",70,70,70,95,M(12)|M(14),"RESTORES HP",RGB(28,28,28)},
{"SUPPORT",85,90,80,105,M(13)|M(14),"BUFF CONTROL DMG",RGB(26,20,6)},
{"MAGE",65,150,55,95,M(12)|M(13),"MAGIC DAMAGE",RGB(18,8,28)},
{"RANGER",75,135,70,110,M(9)|M(2)|M(5),"STRONG BUT FRAIL",RGB(6,18,10)},
{"KNIGHT",140,100,150,75,M(1)|M(0)|M(4),"SLOW ARMORED",RGB(18,20,26)}};
static const Sk SK[12][3]={
{{"BASIC CUT",K_SLASH,180,70,2,36},{"BASIC SHOT",K_SHOT,160,90,3,1},{"FIRST AID",K_HEAL,25,400,6,0}},
{{"SNIPE",K_SHOT,450,200,6,1},{"RAPID FIRE",K_SHOT,120,160,8,4},{"ROLL",K_DASH,0,200,3,48}},
{{"POWER SHOT",K_SHOT,300,150,5,1},{"ARROW RAIN",K_BLAST,220,300,10,60},{"EVADE",K_DASH,0,180,3,40}},
{{"HEAVY CUT",K_SLASH,300,150,5,44},{"WHIRLWIND",K_BLAST,200,260,8,44},{"WAR CRY",K_BUFF,30,500,8,0}},
{{"COMBO",K_SLASH,260,100,4,36},{"RUSH",K_DASH,200,170,4,56},{"SHOCKWAVE",K_BLAST,260,280,9,52}},
{{"BACKSTAB",K_SLASH,350,110,4,30},{"SHADOW",K_STEALTH,0,420,8,240},{"SMOKE BOMB",K_STUN,100,320,8,50}},
{{"SHIELD BASH",K_STUN,160,200,5,40},{"FORTIFY",K_SHIELD,0,500,9,300},{"SMASH",K_BLAST,240,260,8,48}},
{{"MEND",K_HEAL,45,200,10,0},{"SMITE",K_SHOT,200,130,5,1},{"BLESS",K_BUFF,40,600,10,2}},
{{"HASTE",K_BUFF,40,500,8,1},{"BIND",K_STUN,120,260,8,70},{"SPARK",K_SHOT,220,120,5,2}},
{{"FIREBALL",K_SHOT,320,130,7,1},{"NOVA",K_BLAST,320,320,14,64},{"BLINK",K_DASH,0,200,4,64}},
{{"HUNTER SHOT",K_SHOT,280,130,5,2},{"BEAR TRAP",K_STUN,180,300,8,56},{"BEAST ROAR",K_BUFF,30,500,8,0}},
{{"JUDGMENT",K_SLASH,320,170,6,48},{"CHARGE",K_DASH,220,200,5,56},{"SACRED WALL",K_SHIELD,0,520,10,320}}};
typedef struct{const char*n;s8 hp,atk,spd,cdr;u8 p[3];}Br; /* turunan class */
#define B(n,h,a,s,c,p0,p1,p2) {n,h,a,s,c,{p0,p1,p2}}
static const Br BRN[24]={
B("ALL ROUNDER",10,10,5,5,10,10,10),B("ARMS MASTER",0,20,0,10,30,0,0),
B("SNIPER",-5,25,-5,0,50,0,0),B("DUAL GUNNER",0,5,10,15,0,40,0),
B("HAWKEYE",0,15,5,0,40,0,0),B("STORM ARCHER",0,5,10,10,0,50,0),
B("BERSERKER",-10,25,5,0,40,0,20),B("WARDEN",20,0,0,0,0,20,40),
B("MONK",10,10,10,0,30,30,0),B("BRAWLER",5,20,0,10,50,0,0),
B("NIGHT BLADE",-5,20,10,0,50,0,0),B("POISONER",0,10,5,0,0,30,50),
B("BULWARK",30,0,-5,0,0,50,0),B("PROVOKER",10,10,0,10,40,0,40),
B("SAINT",10,0,0,10,60,0,0),B("ORACLE",0,20,5,0,0,40,20),
B("BUFFER",10,0,5,10,0,0,50),B("HEXER",0,15,0,0,0,60,0),
B("PYROMANCER",-5,25,0,0,50,0,0),B("FROST WIZARD",0,10,5,10,0,50,0),
B("TRAPPER",5,10,0,10,0,50,0),B("BEASTMASTER",15,10,5,0,0,0,40),
B("PALADIN",20,0,0,10,0,30,40),B("DRAGOON",0,20,10,0,50,0,0)};
static const Br B0={"",0,0,0,0,{0,0,0}};
typedef struct{const char*n;u16 col;u16 hp,atk;u8 spd,rng,sz;}Et;
static const Et ET[19]={
{"DUMMY",RGB(18,12,5),200,25,0,0,12},{"BOT",RGB(17,18,21),100,55,8,0,12},{"RIVAL",RGB(10,10,28),80,70,9,1,12},
{"SLIME",RGB(6,26,8),90,60,6,0,12},{"WOLF",RGB(17,15,13),80,80,15,0,12},{"GOBLIN",RGB(9,19,6),100,90,10,0,12},
{"WISP",RGB(28,28,10),70,100,8,1,10},{"SCORPION",RGB(25,17,4),100,100,10,0,12},{"MUMMY",RGB(25,25,19),190,90,6,0,12},
{"ICE IMP",RGB(12,24,31),80,110,10,1,12},{"YETI",RGB(29,29,31),210,120,8,0,16},{"TOAD",RGB(12,20,6),130,100,8,0,14},
{"WRAITH",RGB(20,6,26),90,130,10,1,12},
{"GOBLIN CHIEF",RGB(6,14,4),700,110,9,0,22},{"TREANT KING",RGB(4,10,4),900,120,7,0,26},{"SAND WYRM",RGB(28,22,8),1000,125,9,0,26},
{"BOG HYDRA",RGB(8,16,12),1100,130,8,0,28},{"FROST LICH",RGB(20,26,31),1000,135,8,0,24},{"ABYSS LORD",RGB(28,2,6),1500,150,10,0,30}};
static const u8 BPX[6]={82,22,106,64,64,112},BPY[6]={78,64,64,106,20,14},BLV[6]={25,38,42,46,50,58};
static const u8 BIOT[5][2]={{3,4},{5,6},{7,8},{9,10},{11,12}};
static const char*BIN[5]={"PLAINS","FOREST","DESERT","SNOWLAND","SWAMP"};
typedef struct{const char*n;u8 g,k,tg,need,min;u16 xp;u8 rar;}Qd; /* k:0 kill type,1 boss,2 chests */
#define NQ 16
static const Qd Q[NQ]={
{"DUMMY DRILL",4,0,0,5,0,60,0},{"BOT TRAINING",4,0,1,6,6,200,0},{"STUDENT DUEL",4,0,2,6,12,400,1},
{"SLIME PEST",0,0,3,8,20,500,0},{"WOLF HUNT",0,0,4,8,21,600,1},{"GOBLIN CHIEF",1,1,0,1,22,1500,2},
{"TREASURE HUNT",3,2,0,3,20,800,1},{"FOREST CLEANSE",0,0,5,12,24,1500,1},{"WISP LIGHTS",2,0,6,10,26,1800,1},
{"TREANT KING",2,1,1,1,30,4000,2},{"SCORPION NEST",0,0,7,12,32,3000,1},{"SAND WYRM",1,1,2,1,36,6000,2},
{"YETI CULL",0,0,10,10,40,6000,2},{"BOG HYDRA",2,1,3,1,42,9000,2},{"FROST LICH",1,1,4,1,46,12000,3},{"ABYSS LORD",1,1,5,1,56,30000,3}};
static const struct{u8 tx,ty,g;const char*n;u16 c;}NPC[5]={
{60,53,4,"INSTRUCTOR",RGB(20,6,20)},{58,65,0,"GUILD BOARD",RGB(28,22,6)},{62,67,1,"ELDER",RGB(24,24,28)},
{66,65,2,"HUNTER",RGB(8,22,8)},{70,67,3,"MERCHANT",RGB(8,18,28)}};

/* ---------- FONT 3x5 ---------- */
static const char GC[]="ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.:-+/!?%><";
static const u16 GD[]={0b010101111101101,0b110101110101110,0b011100100100011,0b110101101101110,0b111100110100111,0b111100110100100,
0b011100101101011,0b101101111101101,0b111010010010111,0b001001001101010,0b101101110101101,0b100100100100111,0b101111111101101,
0b110101101101101,0b010101101101010,0b110101110100100,0b010101101111011,0b110101110101101,0b011100010001110,0b111010010010010,
0b101101101101111,0b101101101101010,0b101101111111101,0b101101010101101,0b101101010010010,0b111001010100111,
0b111101101101111,0b010110010010111,0b110001010100111,0b110001010001110,0b101101111001001,0b111100110001110,0b011100111101111,
0b111001010010010,0b111101111101111,0b111101111001110,
0b000000000000010,0b000010000010000,0b000000111000000,0b000010111010000,0b001001010100100,0b010010010000010,0b110001010000010,
0b101001010100101,0b100010001010100,0b001010100010001};

/* ---------- STATE ---------- */
enum{ST_TITLE,ST_PLAY,ST_MENU,ST_CLASS,ST_BRANCH,ST_DEAD};
typedef struct{u8 t,r,l;}Wp;
static struct{int x,y,dx,dy,hp,mp,lvl,xp,sp,cls,br,grad,acd,inv_t,regt,stl,shd,macc;int scd[3],skl[3],bt[3],bv[3];}P;
static Wp inv[10];static int ninv,eq;
static int st,frame,kd,kp,pk,CX,CY,gate_open,mtab,mc,cc,bc;
static u8 bdone[6],qs[NQ],qp[NQ];
static u8 opened[WT*WT/8];
static u16 tiles[13][256];
static u32 rs=1234567;
static u32 rnd(void){rs=rs*1664525u+1013904223u;return rs>>8;}
static int rr(int n){return (int)(rnd()%(u32)n);}
static u32 hh(u32 a,u32 b){u32 h=a*374761393u+b*668265263u+0x9E3779B9u;h=(h^(h>>13))*1274126177u;return h^(h>>16);}
static int sg(int v){return v>0?1:v<0?-1:0;}
static int dist(int dx,int dy){dx=ABS(dx);dy=ABS(dy);return dx>dy?dx+dy/2:dy+dx/2;}

/* ---------- DRAW ---------- */
static void px(int x,int y,u16 c){if((u32)x<SW&&(u32)y<SH)bb[y*SW+x]=c;}
static void rect(int x,int y,int w,int h,u16 c){
 if(x<0){w+=x;x=0;} if(y<0){h+=y;y=0;} if(x+w>SW)w=SW-x; if(y+h>SH)h=SH-y; if(w<=0||h<=0)return;
 for(int j=0;j<h;j++){u16*p=bb+(y+j)*SW+x; if(w>=10)dma_fill(p,c,w); else for(int i=0;i<w;i++)p[i]=c;}}
static void frect(int x,int y,int w,int h,u16 c){rect(x,y,w,1,c);rect(x,y+h-1,w,1,c);rect(x,y,1,h,c);rect(x+w-1,y,1,h,c);}
static void box(int x,int y,int w,int h,u16 f,u16 b){rect(x,y,w,h,f);frect(x,y,w,h,b);}
static void blit(int x,int y,int id){
 const u16*s=tiles[id];
 if(x>=0&&x+16<=SW){for(int j=0;j<16;j++){int yy=y+j;if((u32)yy<SH)dma_cpy(bb+yy*SW+x,s+j*16,16);}}
 else for(int j=0;j<16;j++)for(int i=0;i<16;i++)px(x+i,y+j,s[j*16+i]);}
static u8 shd_on;
static int slen(const char*s){int n=0;while(*s++)n++;return n;}
static int txt(int x,int y,const char*s,u16 c,int sc){
 for(;*s;s++){char ch=*s;if(ch>='a'&&ch<='z')ch-=32;
  if(ch!=' ')for(int k=0;GC[k];k++)if(GC[k]==ch){u16 g=GD[k];
   for(int r=0;r<5;r++)for(int q=0;q<3;q++)if((g>>(14-r*3-q))&1){
    if(sc==1){if(shd_on)px(x+q+1,y+r+1,0);px(x+q,y+r,c);}else rect(x+q*sc,y+r*sc,sc,sc,c);}
   break;}
  x+=4*sc;}
 return x;}
static int i2s(char*d,int v){char t[12];int n=0,k=0;if(v<0){d[k++]='-';v=-v;}if(v==0)t[n++]='0';while(v>0){t[n++]='0'+v%10;v/=10;}while(n>0)d[k++]=t[--n];d[k]=0;return k;}
static int num(int x,int y,int v,u16 c,int sc){char b[14];i2s(b,v);return txt(x,y,b,c,sc);}
static void bar(int x,int y,int w,int h,int v,int mx,u16 c){rect(x,y,w,h,RGB(3,3,3));if(mx>0&&v>0){int f=v*(w-2)/mx;if(f<1)f=1;rect(x+1,y+1,f,h-2,c);}}
/* toast */
static char mb[44];static int ml,mt;
static void m0(void){ml=0;mb[0]=0;}
static void mcat(const char*s){while(*s&&ml<42)mb[ml++]=*s++;mb[ml]=0;}
static void mnum(int v){char t[14];i2s(t,v);mcat(t);}
static void m1(void){mt=80;}
static void say(const char*a,const char*b){m0();mcat(a);if(b)mcat(b);m1();}
/* damage popups */
typedef struct{int x,y,v,t;u16 c;}Dn;static Dn dn[8];
static void pop(int x,int y,int v,u16 c){for(int i=0;i<8;i++)if(dn[i].t<=0){dn[i].x=x;dn[i].y=y;dn[i].v=v;dn[i].t=22;dn[i].c=c;return;}}

/* ---------- WORLD ---------- */
enum{T_GRASS,T_FGRASS,T_SAND,T_SNOW,T_SWAMP,T_ROAD,T_FLOOR,T_WALL,T_GATE,T_TREE,T_WATER,T_ROCK,T_CHEST};
static int cheb(int tx,int ty){int a=ABS(tx-64),b=ABS(ty-64);return a>b?a:b;}
static int biome(int tx,int ty){int dx=tx-64,dy=ty-64;if(cheb(tx,ty)<24)return 0;if(ABS(dx)>=ABS(dy))return dx>0?2:1;return dy<0?3:4;}
static int zlvl(int tx,int ty){return 8+cheb(tx,ty)*2/3;}
static int isop(int tx,int ty){int i=ty*WT+tx;return (opened[i>>3]>>(i&7))&1;}
static int tile_at(int tx,int ty){
 if(tx<=0||ty<=0||tx>=WT-1||ty>=WT-1)return T_ROCK;
 if(tx>=54&&tx<=73&&ty>=50&&ty<=61){
  if(tx==54||tx==73||ty==50||ty==61){if(ty==61&&(tx==63||tx==64))return gate_open?T_FLOOR:T_GATE;return T_WALL;}
  return T_FLOOR;}
 if(tx>=52&&tx<=75&&ty>=62&&ty<=73)return T_ROAD;
 int bi=biome(tx,ty);int g=bi==0?T_GRASS:bi==1?T_FGRASS:bi==2?T_SAND:bi==3?T_SNOW:T_SWAMP;
 for(int b=0;b<6;b++)if(ABS(tx-BPX[b])<=3&&ABS(ty-BPY[b])<=3)return g;
 if(ABS(tx-64)<=1||ABS(ty-64)<=1)return T_ROAD;
 if(tx>=44&&tx<=84&&ty>=42&&ty<=80)return g;
 int pc=bi==1?30:bi==4?24:bi==0?12:18;
 if((int)(hh(tx>>2,ty>>2)%100)<pc&&hh(tx,ty)%5!=0)return bi==4?T_WATER:(bi>=2?T_ROCK:T_TREE);
 if(hh(tx*3+1,ty*7+5)%150==0&&!isop(tx,ty))return T_CHEST;
 return g;}
static int blocked(int x,int y){int t=tile_at(x>>4,y>>4);return t==T_WALL||t==T_GATE||t==T_TREE||t==T_WATER||t==T_ROCK||t==T_CHEST;}
static int walk(int x,int y,int h){return !blocked(x-h,y-3)&&!blocked(x+h,y-3)&&!blocked(x-h,y+3)&&!blocked(x+h,y+3);}
static void mv(int*x,int*y,int dx,int dy,int h){if(dx&&walk(*x+dx,*y,h))*x+=dx;if(dy&&walk(*x,*y+dy,h))*y+=dy;}
static void mk(int id,u16 a,u16 b,int mode){
 u16*t=tiles[id];
 for(int j=0;j<16;j++)for(int i=0;i<16;i++){
  u16 c=a;if((i*7+j*13+id*5)%11==0)c=b;
  switch(mode){
   case 1:{int dx=i-8,dy=j-7;if(dx*dx+dy*dy<49)c=((dx+dy)%3==0)?RGB(4,14,4):RGB(6,20,6);else if(i>=7&&i<=9&&j>=12)c=RGB(14,8,3);}break;
   case 2:c=((j%4==0)||((i+(j/4%2)*4)%8==0))?RGB(8,8,9):RGB(18,17,18);break;
   case 3:c=((i+j*2)%8<2)?RGB(14,20,31):RGB(5,12,26);break;
   case 4:{int dx=i-8,dy=j-8;if(dx*dx+dy*dy<56)c=(dy<0)?RGB(20,20,22):RGB(13,13,16);}break;
   case 5:if(i>=2&&i<=13&&j>=5&&j<=13){c=(j==8||i==2||i==13||j==5||j==13)?RGB(12,7,2):RGB(24,15,5);if(i>=7&&i<=8&&j>=7&&j<=9)c=RGB(31,28,6);}break;
   case 6:c=(i%4==0||j%8==0)?RGB(10,6,3):RGB(22,15,7);break;}
  t[j*16+i]=c;}}
static void gen_tiles(void){
 mk(T_GRASS,RGB(9,21,8),RGB(7,18,6),0);mk(T_FGRASS,RGB(5,15,6),RGB(4,12,5),0);mk(T_SAND,RGB(28,24,14),RGB(25,21,11),0);
 mk(T_SNOW,RGB(29,30,31),RGB(24,26,30),0);mk(T_SWAMP,RGB(8,14,9),RGB(5,10,7),0);mk(T_ROAD,RGB(20,17,12),RGB(17,14,10),0);
 mk(T_FLOOR,RGB(22,22,24),RGB(19,19,22),0);mk(T_WALL,0,0,2);mk(T_GATE,0,0,6);mk(T_TREE,RGB(7,17,7),RGB(5,14,5),1);
 mk(T_WATER,0,0,3);mk(T_ROCK,RGB(10,16,9),RGB(8,13,7),4);mk(T_CHEST,RGB(9,21,8),RGB(7,18,6),5);}

/* ---------- PLAYER STATS ---------- */
static const Br*brp(void){return P.br<0?&B0:&BRN[P.cls*2+P.br];}
static int wdmg(Wp w){return WT_[w.t].dmg*(100+w.l*7)/100*(100+w.r*30)/100;}
static int pmaxhp(void){return (40+P.lvl*9)*CL[P.cls].hp/100*(100+brp()->hp)/100;}
static int pmaxmp(void){return 20+P.lvl*2;}
static int pdef(void){return (P.lvl/2+3)*CL[P.cls].def/100*(100+(P.bt[2]>0?P.bv[2]:0))/100;}
static int pspeed(void){return 18*CL[P.cls].spd/100*(100+(P.bt[1]>0?P.bv[1]:0)+brp()->spd)/100;}
static int aff(int t){int m=CL[P.cls].m;if(m==0x7FFF)return 110;return ((m>>t)&1)?125:85;}
static int patk(void){int b=(6+P.lvl*2)*CL[P.cls].atk/100;b+=wdmg(inv[eq])*aff(inv[eq].t)/100;b=b*(100+brp()->atk)/100;if(P.bt[0]>0)b=b*(100+P.bv[0])/100;return b;}
static int pdmg(int pct){int d=patk()*pct/100;if(P.stl>0){d*=2;P.stl=0;}return d*(90+rr(21))/100;}
static int need(int l){return 12+l*l+6*l;}
static int spw(int s){return SK[P.cls][s].pw*(100+15*(P.skl[s]-1))/100*(100+brp()->p[s])/100;}
static int scdv(int s){int c=SK[P.cls][s].cd;if(P.skl[s]>=7)c=c*75/100;return c*(100-brp()->cdr)/100;}
static int spr(int s){const Sk*k=&SK[P.cls][s];int p=k->pr;if(k->ty!=K_SHOT&&k->ty!=K_BUFF&&k->ty!=K_HEAL&&P.skl[s]>=4)p=p*12/10;return p;}
static Wp mkw(int il,int bonus,int pref,int pct){
 Wp w;w.l=il>60?60:il;w.t=rr(15);
 if(pref>=0&&rr(100)<pct){int m=CL[pref].m;for(int k=0;k<40;k++){int t=rr(15);if((m>>t)&1){w.t=t;break;}}}
 int r=rr(100)+bonus;w.r=r>110?3:r>88?2:r>58?1:0;return w;}
static void addw(Wp w){
 if(ninv<10)inv[ninv++]=w;
 else{int worst=-1,wv=wdmg(w);for(int i=0;i<10;i++)if(i!=eq&&wdmg(inv[i])<wv){worst=i;wv=wdmg(inv[i]);}
  if(worst<0){say("PACK FULL",0);return;}inv[worst]=w;}
 m0();mcat("GOT ");mcat(RN[w.r]);mcat(" ");mcat(WT_[w.t].n);m1();}
static void gainxp(int v){
 if(!P.grad&&P.lvl>=20)return;
 P.xp+=v;
 while(P.lvl<60&&P.xp>=need(P.lvl)){
  P.xp-=need(P.lvl);P.lvl++;P.sp++;P.hp=pmaxhp();P.mp=pmaxmp();
  m0();mcat("LEVEL UP! LV ");mnum(P.lvl);m1();
  if(!P.grad&&P.lvl>=20){P.xp=0;break;}}
 if(P.lvl>=60)P.xp=0;}

/* ---------- ENEMIES / PROJECTILES ---------- */
typedef struct{int x,y,hp,mhp,lvl,atk,cd,stun,fl,acc,b;u8 t,on;}En;
#define NE 14
static En en[NE];
typedef struct{int x,y,vx,vy,dm,life,pier,lh;u8 on,fr,sz;u16 col;}Pj;
#define NP 24
static Pj pj[NP];
static struct{int x,y,w,h,t;u16 c;}fx;
static void spawn(int t,int lvl,int x,int y,int b){
 for(int i=0;i<NE;i++)if(!en[i].on){En*e=&en[i];e->on=1;e->t=t;e->lvl=lvl;e->x=x;e->y=y;e->b=b;
  e->mhp=e->hp=(20+lvl*9)*ET[t].hp/100;e->atk=(5+lvl*2)*ET[t].atk/100;e->cd=40;e->stun=0;e->fl=0;e->acc=0;return;}}
static void fire(int x,int y,int vx,int vy,int dm,int life,int pier,u16 col,int sz,int fr){
 for(int i=0;i<NP;i++)if(!pj[i].on){Pj*p=&pj[i];p->on=1;p->x=x;p->y=y;p->vx=vx;p->vy=vy;p->dm=dm;p->life=life;p->pier=pier;p->lh=-1;p->col=col;p->sz=sz;p->fr=fr;return;}}
static void qk(int t,int b){
 for(int q=0;q<NQ;q++){if(qs[q]!=1)continue;const Qd*d=&Q[q];
  if(d->k==0&&!b&&d->tg==t)qp[q]++;else if(d->k==1&&b&&d->tg==b-1)qp[q]=d->need;else continue;
  if(qp[q]>=d->need){qp[q]=d->need;qs[q]=2;say("QUEST DONE: ",Q[q].n);}}}
static void killed(En*e){
 e->on=0;int xp=(6+3*e->lvl)*(e->b?12:1);gainxp(xp);qk(e->t,e->b);
 int pref=P.grad?P.cls:-1;
 if(e->b){bdone[e->b-1]=1;addw(mkw(e->lvl,60,pref,60));addw(mkw(e->lvl,40,pref,30));}
 else if(rr(100)<(e->t==0?4:12))addw(mkw(e->lvl,0,pref,35));}
static void hit(En*e,int dm){
 u16 c=WHITE;if(rr(100)<12){dm*=2;c=RGB(31,26,4);}
 if(dm<1)dm=1;pop(e->x,e->y-ET[e->t].sz-6,dm,c);e->hp-=dm;e->fl=3;
 if(e->hp<=0)killed(e);}
static void area(int cx,int cy,int r,int dm,int stun){
 for(int i=0;i<NE;i++){En*e=&en[i];if(!e->on)continue;int hs=ET[e->t].sz/2;
  if(ABS(e->x-cx)<r+hs&&ABS(e->y-6-cy)<r+hs){if(stun)e->stun=stun;hit(e,dm);}}}
static void hurt(int d){
 if(P.inv_t>0||st!=ST_PLAY)return;
 d-=pdef();if(P.shd>0)d/=2;if(d<1)d=1;
 P.hp-=d;P.inv_t=10;P.regt=120;pop(P.x,P.y-24,d,RGB(31,6,6));
 if(P.hp<=0){P.hp=0;st=ST_DEAD;}}
static void fxset(int x,int y,int w,int h,u16 c){fx.x=x;fx.y=y;fx.w=w;fx.h=h;fx.c=c;fx.t=6;}
static void attack(void){
 if(P.acd>0)return;
 const Wt*t=&WT_[inv[eq].t];P.acd=t->cd*100/CL[P.cls].spd;
 int dm=pdmg(100);
 if(t->t==0){int r=t->spd;int cx=P.x+P.dx*r/2,cy=P.y-6+P.dy*r/2;area(cx,cy,r/2+8,dm,0);fxset(cx-r/2-8,cy-r/2-8,r+16,r+16,WHITE);}
 else fire(P.x,P.y-8,P.dx*t->spd,P.dy*t->spd,dm,60,t->t==2?1:0,t->t==2?CL[P.cls].c|RGB(8,8,8):RGB(31,31,22),t->t==2?5:3,1);}
static void cast(int s){
 const Sk*k=&SK[P.cls][s];if(P.scd[s]>0)return;
 if(P.mp<k->mp){say("NOT ENOUGH MP",0);return;}
 int pw=spw(s),p=spr(s);P.mp-=k->mp;P.scd[s]=scdv(s);u16 col=CL[P.cls].c|RGB(10,10,10);
 switch(k->ty){
  case K_SLASH:{int dm=pdmg(pw);int cx=P.x+P.dx*p/2,cy=P.y-6+P.dy*p/2,r=p/2+10;area(cx,cy,r,dm,0);fxset(cx-r,cy-r,2*r,2*r,col);}break;
  case K_BLAST:{int dm=pdmg(pw);area(P.x,P.y-6,p,dm,0);fxset(P.x-p,P.y-6-p,2*p,2*p,col);}break;
  case K_STUN:{int dm=pdmg(pw);area(P.x,P.y-6,p,dm,120);fxset(P.x-p,P.y-6-p,2*p,2*p,RGB(28,28,4));}break;
  case K_SHOT:{int dm=pdmg(pw);int n=p+(P.skl[s]>=10);
   for(int i=0;i<n;i++){int o=i*8-(n-1)*4;fire(P.x-P.dy*o,P.y-8+P.dx*o,P.dx*6,P.dy*6,dm,55,2,col,4,1);}}break;
  case K_HEAL:{int h=pmaxhp()*pw/100;P.hp+=h;if(P.hp>pmaxhp())P.hp=pmaxhp();pop(P.x,P.y-24,h,RGB(8,31,8));}break;
  case K_BUFF:P.bt[p]=600;P.bv[p]=pw;say("BUFF ACTIVE",0);break;
  case K_DASH:{for(int d=0;d<p;d+=4){if(!walk(P.x+P.dx*4,P.y+P.dy*4,5))break;P.x+=P.dx*4;P.y+=P.dy*4;}
   P.inv_t=15;if(pw>0){int dm=pdmg(pw);area(P.x,P.y-6,22,dm,0);fxset(P.x-22,P.y-28,44,44,col);}}break;
  case K_STEALTH:P.stl=p;say("STEALTH! NEXT HIT X2",0);break;
  case K_SHIELD:P.shd=p;say("GUARD UP",0);break;}}

/* ---------- QUESTS / NPC ---------- */
static void talk(int ni){
 int g=NPC[ni].g;
 for(int q=0;q<NQ;q++)if(Q[q].g==g&&qs[q]==2){qs[q]=3;gainxp(Q[q].xp);addw(mkw(P.lvl,Q[q].rar*35+10,P.grad?P.cls:-1,40));
  m0();mcat("REWARD: ");mcat(Q[q].n);mcat(" +");mnum(Q[q].xp);mcat("XP");m1();return;}
 for(int q=0;q<NQ;q++)if(Q[q].g==g&&qs[q]==0&&P.lvl>=Q[q].min){qs[q]=1;qp[q]=0;say("NEW QUEST: ",Q[q].n);return;}
 for(int q=0;q<NQ;q++)if(Q[q].g==g&&qs[q]==1){m0();mcat(Q[q].n);mcat(" ");mnum(qp[q]);mcat("/");mnum(Q[q].need);m1();return;}
 P.hp=pmaxhp();P.mp=pmaxmp();m0();mcat(NPC[ni].n);mcat(": REST. HP FULL");m1();}
static int interact(void){
 for(int i=0;i<5;i++){int nx=NPC[i].tx*16+8,ny=NPC[i].ty*16+12;if(ABS(P.x-nx)<28&&ABS(P.y-ny)<28){talk(i);return 1;}}
 for(int k=10;k<=20;k+=10){int tx=(P.x+P.dx*k)>>4,ty=(P.y-3+P.dy*k)>>4;
  if(tile_at(tx,ty)==T_CHEST){int ix=ty*WT+tx;opened[ix>>3]|=1<<(ix&7);
   addw(mkw(zlvl(tx,ty),25,P.grad?P.cls:-1,35));
   for(int q=0;q<NQ;q++)if(qs[q]==1&&Q[q].k==2){qp[q]++;if(qp[q]>=Q[q].need){qs[q]=2;say("QUEST DONE: ",Q[q].n);}}
   return 1;}}
 return 0;}

/* ---------- GAME FLOW ---------- */
static void newgame(void){
 P.x=63*16+8;P.y=56*16;P.dx=0;P.dy=1;P.lvl=0;P.xp=0;P.sp=0;P.cls=0;P.br=-1;P.grad=0;P.acd=0;P.inv_t=0;P.regt=0;P.stl=0;P.shd=0;P.macc=0;
 for(int i=0;i<3;i++){P.scd[i]=0;P.skl[i]=1;P.bt[i]=0;P.bv[i]=0;}
 P.hp=pmaxhp();P.mp=pmaxmp();gate_open=0;
 for(int i=0;i<(int)sizeof(opened);i++)opened[i]=0;
 for(int i=0;i<6;i++)bdone[i]=0;for(int i=0;i<NQ;i++){qs[i]=0;qp[i]=0;}
 for(int i=0;i<NE;i++)en[i].on=0;for(int i=0;i<NP;i++)pj[i].on=0;for(int i=0;i<8;i++)dn[i].t=0;
 ninv=1;eq=0;inv[0].t=0;inv[0].r=0;inv[0].l=0;mt=0;fx.t=0;
 st=ST_PLAY;say("WELCOME TO THE ACADEMY",0);}
static void areaname(const char**n,int*zl){
 int tx=P.x>>4,ty=P.y>>4;
 if(tx>=54&&tx<=73&&ty>=50&&ty<=61){*n="ACADEMY";*zl=P.lvl;return;}
 if(tx>=44&&tx<=84&&ty>=42&&ty<=80){*n="GUILD TOWN";*zl=0;return;}
 *n=BIN[biome(tx,ty)];*zl=zlvl(tx,ty);}
static void spawner(void){
 int cnt=0;for(int i=0;i<NE;i++)if(en[i].on)cnt++;
 if(!P.grad){
  if(cnt<5&&rr(12)==0){int tx=56+rr(16),ty=52+rr(8);int x=tx*16+8,y=ty*16+8;
   if(dist(x-P.x,y-P.y)>70){int t=P.lvl<7?0:P.lvl<14?1:(rr(2)?1:2);int l=P.lvl+rr(3)-1;if(l<0)l=0;spawn(t,l,x,y,0);}}
  return;}
 for(int b=0;b<6;b++){if(bdone[b])continue;int on=0;for(int i=0;i<NE;i++)if(en[i].on&&en[i].b==b+1)on=1;
  int bx=BPX[b]*16+8,by=BPY[b]*16+8;
  if(!on&&ABS(P.x-bx)<170&&ABS(P.y-by)<130)spawn(13+b,BLV[b],bx,by,b+1);}
 if(cnt<9&&rr(10)==0){int x=P.x+rr(361)-180,y=P.y+rr(281)-140;
  if(ABS(x-P.x)<125&&ABS(y-P.y)<85)return;
  int tx=x>>4,ty=y>>4;if(tx<2||ty<2||tx>WT-3||ty>WT-3)return;
  if(tx>=44&&tx<=84&&ty>=42&&ty<=80)return;
  if(!walk(x,y,6))return;
  int bi=biome(tx,ty);int l=zlvl(tx,ty)+rr(3)-1;spawn(BIOT[bi][rr(2)],l,x,y,0);}}
static void upd_enemies(void){
 for(int i=0;i<NE;i++){En*e=&en[i];if(!e->on)continue;const Et*t=&ET[e->t];
  int dx=P.x-e->x,dy=P.y-e->y;int d=dist(dx,dy);
  if(e->fl>0)e->fl--;
  if((!e->b&&d>320)||d>600){e->on=0;continue;}
  if(e->stun>0){e->stun--;continue;}
  if(e->cd>0)e->cd--;
  int ag=e->b?220:100;if(P.stl>0)ag=24;
  int rng=t->rng||e->b?1:0;
  if(t->spd&&d<ag&&(!t->rng||d>60)){
   e->acc+=t->spd;int n=e->acc/10;e->acc%=10;int ax=0,ay=0;{int a=ABS(dx),b=ABS(dy);ax=(a*2>b)?sg(dx):0;ay=(b*2>a)?sg(dy):0;}
   int h=t->sz/2-2;if(h<3)h=3;while(n--)mv(&e->x,&e->y,ax,ay,h);}
  if(d<t->sz/2+9&&e->cd==0){hurt(e->atk);e->cd=45;}
  else if(rng&&e->cd==0&&d<130&&d>24&&P.stl<=0){
   if(e->b){for(int k=0;k<8;k++){static const s8 DX[8]={1,1,0,-1,-1,-1,0,1},DY[8]={0,1,1,1,0,-1,-1,-1};
     fire(e->x,e->y-8,DX[k]*3,DY[k]*3,e->atk*6/10,70,0,RGB(31,8,8),4,0);}e->cd=100;}
   else{int a=ABS(dx),b=ABS(dy);int ax=(a*2>b)?sg(dx):0,ay=(b*2>a)?sg(dy):0;fire(e->x,e->y-8,ax*3,ay*3,e->atk,60,0,t->col|RGB(8,8,8),4,0);e->cd=75;}}}}
static void upd_proj(void){
 for(int i=0;i<NP;i++){Pj*p=&pj[i];if(!p->on)continue;
  p->x+=p->vx;p->y+=p->vy;
  if(--p->life<=0||blocked(p->x,p->y)){p->on=0;continue;}
  if(p->fr){for(int j=0;j<NE;j++){En*e=&en[j];if(!e->on||j==p->lh)continue;int hs=ET[e->t].sz/2+3;
    if(ABS(e->x-p->x)<hs&&ABS(e->y-6-p->y)<hs+4){hit(e,p->dm);p->lh=j;if(p->pier--<=0)p->on=0;break;}}}
  else if(ABS(P.x-p->x)<8&&ABS(P.y-8-p->y)<10){hurt(p->dm);p->on=0;}}}
static void update_play(void){
 if(!P.grad&&P.lvl>=20){st=ST_CLASS;cc=0;return;}
 if(P.grad&&P.br<0&&P.lvl>=30){st=ST_BRANCH;bc=0;return;}
 if(kp&8){st=ST_MENU;mtab=0;mc=0;return;}
 int mx=0,my=0;if(kd&16)mx=1;if(kd&32)mx=-1;if(kd&64)my=-1;if(kd&128)my=1;
 if(mx||my){P.dx=mx;P.dy=my;P.macc+=pspeed();int n=P.macc/10;P.macc%=10;while(n--)mv(&P.x,&P.y,mx,my,5);}
 if(kp&1){if(!interact())attack();}else if(kd&1)attack();
 if(kp&2)cast(0);if(kp&512)cast(1);if(kp&256)cast(2);
 if(kp&4){eq=(eq+1)%ninv;m0();mcat("EQUIP: ");mcat(RN[inv[eq].r]);mcat(" ");mcat(WT_[inv[eq].t].n);m1();}
 if(P.acd>0)P.acd--;if(P.inv_t>0)P.inv_t--;if(P.stl>0)P.stl--;if(P.shd>0)P.shd--;
 for(int i=0;i<3;i++){if(P.scd[i]>0)P.scd[i]--;if(P.bt[i]>0)P.bt[i]--;}
 if(P.regt>0)P.regt--;else if(frame%15==0&&P.hp<pmaxhp()){P.hp+=1+pmaxhp()/80;if(P.hp>pmaxhp())P.hp=pmaxhp();}
 if(frame%6==0&&P.mp<pmaxmp())P.mp+=1+P.lvl/10;if(P.mp>pmaxmp())P.mp=pmaxmp();
 if(fx.t>0)fx.t--;if(mt>0)mt--;
 for(int i=0;i<8;i++)if(dn[i].t>0){dn[i].t--;dn[i].y--;}
 spawner();upd_enemies();upd_proj();}

/* ---------- DRAW SCREENS ---------- */
static void body(int sx,int sy,u16 col,int sz,int fl){
 u16 dk=(col>>1)&0x3DEF;if(fl)col=WHITE;
 rect(sx-sz/2-1,sy-sz-1,sz+2,sz+2,dk);rect(sx-sz/2,sy-sz,sz,sz,col);
 px(sx-sz/4,sy-sz*2/3,WHITE);px(sx+sz/4-1,sy-sz*2/3,WHITE);px(sx-sz/4,sy-sz*2/3+1,0);px(sx+sz/4-1,sy-sz*2/3+1,0);}
static void draw_world(void){
 int cx=P.x-120,cy=P.y-90;if(cx<0)cx=0;if(cy<0)cy=0;if(cx>WT*16-SW)cx=WT*16-SW;if(cy>WT*16-SH)cy=WT*16-SH;CX=cx;CY=cy;
 int tx0=cx>>4,ty0=cy>>4,ox=-(cx&15),oy=-(cy&15);
 for(int j=0;j<=10;j++)for(int i=0;i<=15;i++)blit(ox+i*16,oy+j*16,tile_at(tx0+i,ty0+j));
 for(int i=0;i<5;i++){int sx=NPC[i].tx*16+8-cx,sy=NPC[i].ty*16+12-cy;if(sx<-20||sx>260||sy<-10||sy>180)continue;
  body(sx,sy,NPC[i].c,10,0);rect(sx-3,sy-17,6,4,RGB(28,22,17));
  if(ABS(P.x-(sx+cx))<40&&ABS(P.y-(sy+cy))<40){shd_on=1;txt(sx-slen(NPC[i].n)*2,sy-26,NPC[i].n,RGB(31,31,8),1);shd_on=0;}
  else txt(sx-1,sy-26,"!",RGB(31,28,4),1);}
 for(int i=0;i<NE;i++){En*e=&en[i];if(!e->on)continue;int sx=e->x-cx,sy=e->y-cy,sz=ET[e->t].sz;if(sx<-30||sx>270||sy<-10||sy>190)continue;
  rect(sx-sz/2,sy-1,sz,3,RGB(2,5,2));
  body(sx,sy,e->stun>0?RGB(28,28,4):ET[e->t].col,sz,e->fl);
  if(e->b)rect(sx-5,sy-sz-5,10,3,RGB(31,26,4));
  rect(sx-9,sy-sz-8,18,3,RGB(8,0,0));if(e->hp>0)rect(sx-9,sy-sz-8,e->hp*18/e->mhp+1,3,RGB(28,4,4));
  if(e->b){shd_on=1;txt(sx-slen(ET[e->t].n)*2,sy-sz-16,ET[e->t].n,RGB(31,12,6),1);shd_on=0;}}
 {int sx=P.x-cx,sy=P.y-cy;
  if(!(P.inv_t>0&&(frame&1))){u16 c=CL[P.cls].c;if(P.stl>0)c=(c>>1)&0x3DEF;
   rect(sx-6,sy-1,12,3,RGB(2,5,2));rect(sx-4,sy-4,3,3,RGB(4,3,3));rect(sx+1,sy-4,3,3,RGB(4,3,3));
   rect(sx-5,sy-12,10,9,c);rect(sx-4,sy-19,8,7,RGB(28,22,17));rect(sx-4,sy-19,8,2,RGB(8,5,3));
   px(sx-2+P.dx,sy-15,0);px(sx+1+P.dx,sy-15,0);
   rect(sx+P.dx*8-1,sy-9+P.dy*8-1,3,3,RC[inv[eq].r]);
   if(P.shd>0)frect(sx-9,sy-22,18,24,RGB(10,28,31));
   if(P.bt[0]>0||P.bt[1]>0||P.bt[2]>0)frect(sx-8,sy-21,16,22,RGB(31,24,4));}}
 for(int i=0;i<NP;i++)if(pj[i].on)rect(pj[i].x-CX-pj[i].sz/2,pj[i].y-CY-pj[i].sz/2,pj[i].sz,pj[i].sz,pj[i].col);
 if(fx.t>0)frect(fx.x-CX,fx.y-CY,fx.w,fx.h,fx.c);
 shd_on=1;for(int i=0;i<8;i++)if(dn[i].t>0)num(dn[i].x-CX-4,dn[i].y-CY,dn[i].v,dn[i].c,1);shd_on=0;}
static void draw_hud(void){
 shd_on=1;
 bar(4,4,70,7,P.hp,pmaxhp(),RGB(28,4,4));bar(4,12,70,5,P.mp,pmaxmp(),RGB(6,12,31));
 bar(4,18,70,3,P.xp,P.lvl>=60?1:need(P.lvl),RGB(10,28,10));
 int x=txt(78,5,"LV ",WHITE,1);num(x,5,P.lvl,RGB(31,28,8),1);
 txt(78,13,CL[P.cls].n,CL[P.cls].c|RGB(8,8,8),1);
 const char*an;int zl;areaname(&an,&zl);
 x=txt(130,5,an,RGB(24,28,31),1);if(zl>0){x=txt(x+4,5,"LV",RGB(20,20,24),1);num(x+1,5,zl,RGB(20,20,24),1);}
 if(!P.grad){x=txt(130,13,"GRADUATE AT LV 20",RGB(31,20,6),1);}
 for(int q=0;q<NQ;q++)if(qs[q]==1||qs[q]==2){txt(130,21,Q[q].n,qs[q]==2?RGB(8,31,8):RGB(31,31,31),1);
  if(qs[q]==1){x=num(130,29,qp[q],WHITE,1);x=txt(x,29,"/",WHITE,1);num(x,29,Q[q].need,WHITE,1);}else txt(130,29,"TURN IN!",RGB(8,31,8),1);break;}
 if(P.sp>0){x=txt(4,24,"SP ",RGB(31,28,8),1);num(x,24,P.sp,RGB(31,28,8),1);}
 for(int s=0;s<3;s++){int bx=164+s*25,by=142;box(bx,by,22,16,RGB(3,3,6),RGB(14,14,18));
  if(P.scd[s]>0){int f=P.scd[s]*14/(scdv(s)+1)+1;rect(bx+1,by+15-f,20,f,RGB(8,8,14));}
  txt(bx+3,by+2,s==0?"B":s==1?"L":"R",WHITE,1);num(bx+11,by+2,P.skl[s],RGB(31,28,8),1);
  if(P.mp<SK[P.cls][s].mp)rect(bx+2,by+10,18,2,RGB(31,4,4));}
 if(mt>0){int w=ml*4+6;rect((SW-w)/2,118,w,9,0);txt((SW-w)/2+3,120,mb,WHITE,1);}
 shd_on=0;}
static void draw_title(void){
 rect(0,0,SW,SH,RGB(3,3,10));
 for(int i=0;i<40;i++)px((i*53+frame/4)%SW,(i*29)%110,RGB(20,20,28));
 txt(64,22,"AKADEMI",RGB(31,26,6),4);txt(96,46,"RPG",RGB(31,12,6),4);
 txt(62,76,"OPEN WORLD ACTION RPG FOR GBA",RGB(20,24,31),1);
 txt(40,92,"DPAD MOVE   A ATTACK/TALK/CHEST",RGB(24,24,24),1);
 txt(40,100,"B L R SKILLS   SELECT SWAP WEAPON",RGB(24,24,24),1);
 txt(40,108,"START MENU: STATUS SKILL WEAPON QUEST",RGB(24,24,24),1);
 txt(40,116,"DIE = LOSE EVERYTHING AND RESTART",RGB(31,10,10),1);
 if((frame/20)&1)txt(84,138,"PRESS START",WHITE,1);}
static void statbar(int y,const char*l,int v,u16 c){txt(86,y,l,WHITE,1);rect(110,y,80,5,RGB(4,4,6));rect(110,y,v*80/160,5,c);num(194,y,v,WHITE,1);}
static void draw_class(void){
 rect(0,0,SW,SH,RGB(2,2,7));txt(8,3,"GRADUATION! CHOOSE YOUR CLASS",RGB(31,28,8),1);
 for(int i=0;i<12;i++){if(i==cc)rect(4,13+i*11,74,9,RGB(8,8,18));txt(8,15+i*11,CL[i].n,i==cc?WHITE:RGB(16,16,20),1);}
 const Cl*c=&CL[cc];txt(86,14,c->n,c->c|RGB(6,6,6),2);
 statbar(32,"HP",c->hp,RGB(28,6,6));statbar(40,"ATK",c->atk,RGB(31,18,4));statbar(48,"DEF",c->def,RGB(8,16,31));statbar(56,"SPD",c->spd,RGB(8,28,10));
 txt(86,66,c->d,RGB(24,24,28),1);txt(86,76,"WEAPONS:",RGB(31,28,8),1);
 int x=86,y=84;if(c->m==0x7FFF)txt(x,y,"ALL WEAPONS (X1.1)",WHITE,1);
 else for(int t=0;t<15;t++)if((c->m>>t)&1){x=txt(x,y,WT_[t].n,RGB(24,31,24),1)+4;}
 txt(86,96,"SKILLS:",RGB(31,28,8),1);
 for(int s=0;s<3;s++)txt(86,104+s*8,SK[cc][s].n,RGB(24,28,31),1);
 txt(86,134,"UP/DOWN SELECT  A OK",RGB(20,20,24),1);txt(86,144,"RESETS SKILLS TO LV 1",RGB(20,12,12),1);}
static void draw_branch(void){
 rect(0,0,SW,SH,RGB(2,2,7));txt(8,4,"LV 30! CHOOSE YOUR PATH",RGB(31,28,8),1);txt(8,12,CL[P.cls].n,CL[P.cls].c|RGB(8,8,8),1);
 for(int o=0;o<2;o++){const Br*b=&BRN[P.cls*2+o];int y=26+o*60;box(8,y,224,54,o==bc?RGB(6,6,16):RGB(2,2,5),o==bc?WHITE:RGB(10,10,14));
  txt(14,y+5,b->n,o==bc?RGB(31,28,8):RGB(18,18,20),2);
  int x=txt(14,y+20,"HP ",WHITE,1);x=num(x,y+20,b->hp,WHITE,1);x=txt(x+6,y+20,"ATK ",WHITE,1);x=num(x,y+20,b->atk,WHITE,1);
  x=txt(x+6,y+20,"SPD ",WHITE,1);x=num(x,y+20,b->spd,WHITE,1);x=txt(x+6,y+20,"CD -",WHITE,1);num(x,y+20,b->cdr,WHITE,1);
  txt(14,y+30,"SKILL POWER BONUS %:",RGB(24,28,31),1);
  for(int s=0;s<3;s++){x=txt(14+s*70,y+40,SK[P.cls][s].n,RGB(20,20,24),1);txt(x+3,y+40,"+",WHITE,1);num(x+8,y+40,b->p[s],RGB(8,31,8),1);}}
 txt(60,150,"UP/DOWN  A CONFIRM",RGB(20,20,24),1);}
static void draw_menu(void){
 rect(0,0,SW,SH,RGB(2,2,7));static const char*TN[4]={"STATUS","SKILLS","WEAPONS","QUESTS"};
 for(int i=0;i<4;i++){rect(4+i*58,3,56,10,i==mtab?RGB(8,8,20):RGB(3,3,8));txt(8+i*58,6,TN[i],i==mtab?WHITE:RGB(14,14,18),1);}
 int x,y=20;
 if(mtab==0){
  x=txt(8,y,CL[P.cls].n,CL[P.cls].c|RGB(8,8,8),2);if(P.br>=0)txt(x+6,y+3,brp()->n,RGB(31,28,8),1);
  y+=16;x=txt(8,y,"LEVEL ",WHITE,1);num(x,y,P.lvl,RGB(31,28,8),1);
  y+=9;x=txt(8,y,"XP ",WHITE,1);x=num(x,y,P.xp,WHITE,1);x=txt(x,y,"/",WHITE,1);num(x,y,need(P.lvl),WHITE,1);
  y+=9;x=txt(8,y,"HP ",WHITE,1);x=num(x,y,P.hp,RGB(31,12,12),1);x=txt(x,y,"/",WHITE,1);num(x,y,pmaxhp(),WHITE,1);
  y+=9;x=txt(8,y,"MP ",WHITE,1);x=num(x,y,P.mp,RGB(12,16,31),1);x=txt(x,y,"/",WHITE,1);num(x,y,pmaxmp(),WHITE,1);
  y+=9;x=txt(8,y,"ATK ",WHITE,1);num(x,y,patk(),WHITE,1);
  y+=9;x=txt(8,y,"DEF ",WHITE,1);num(x,y,pdef(),WHITE,1);
  y+=9;x=txt(8,y,"SPEED ",WHITE,1);num(x,y,pspeed(),WHITE,1);
  y+=9;x=txt(8,y,"SKILL POINTS ",WHITE,1);num(x,y,P.sp,RGB(31,28,8),1);
  y+=12;x=txt(8,y,RN[inv[eq].r],RC[inv[eq].r],1);txt(x+4,y,WT_[inv[eq].t].n,RC[inv[eq].r],1);
  y+=9;x=txt(8,y,"WEAPON DMG ",WHITE,1);num(x,y,wdmg(inv[eq]),WHITE,1);
  if(P.cls)txt(8,y+9,((CL[P.cls].m>>inv[eq].t)&1)?"CLASS WEAPON +25%":"OFF-CLASS WEAPON -15%",RGB(24,24,28),1);}
 else if(mtab==1){
  x=txt(8,y,"SKILL POINTS ",WHITE,1);num(x,y,P.sp,RGB(31,28,8),1);txt(110,y,"A: UPGRADE",RGB(20,20,24),1);
  for(int s=0;s<3;s++){int yy=34+s*36;if(s==mc)rect(4,yy-3,232,33,RGB(6,6,16));
   x=txt(8,yy,SK[P.cls][s].n,s==mc?WHITE:RGB(18,18,22),2);
   x=txt(x+6,yy+3,"LV ",WHITE,1);x=num(x,yy+3,P.skl[s],RGB(31,28,8),1);txt(x+4,yy+3,P.skl[s]>=7?"STAGE III":P.skl[s]>=4?"STAGE II":"STAGE I",RGB(8,28,10),1);
   x=txt(8,yy+13,"POWER ",WHITE,1);x=num(x,yy+13,spw(s),WHITE,1);x=txt(x,yy+13,"%  CD ",WHITE,1);x=num(x,yy+13,scdv(s)/30,WHITE,1);
   x=txt(x,yy+13,"S  MP ",WHITE,1);num(x,yy+13,SK[P.cls][s].mp,RGB(12,16,31),1);
   txt(8,yy+21,P.skl[s]<4?"STAGE II AT 4: RANGE UP":P.skl[s]<7?"STAGE III AT 7: COOLDOWN -25%":P.skl[s]<10?"MAX AT 10: EXTRA EFFECT":"MAXED",RGB(20,20,24),1);}}
 else if(mtab==2){
  txt(8,y,"A EQUIP   SELECT DISCARD",RGB(20,20,24),1);
  for(int i=0;i<ninv;i++){int yy=32+i*11;if(i==mc)rect(4,yy-2,232,10,RGB(6,6,16));
   x=txt(8,yy,i==eq?"E ":"  ",RGB(8,31,8),1);x=txt(x,yy,RN[inv[i].r],RC[inv[i].r],1);x=txt(x+4,yy,WT_[inv[i].t].n,RC[inv[i].r],1);
   x=txt(x+4,yy,"IL",RGB(20,20,24),1);x=num(x+1,yy,inv[i].l,RGB(20,20,24),1);x=txt(x+4,yy,"DMG",WHITE,1);x=num(x+1,yy,wdmg(inv[i]),WHITE,1);
   if(P.cls)txt(x+4,yy,((CL[P.cls].m>>inv[i].t)&1)?"+":"-",((CL[P.cls].m>>inv[i].t)&1)?RGB(8,31,8):RGB(31,8,8),1);}}
 else{
  int n=0;for(int q=0;q<NQ;q++)if(qs[q]==1||qs[q]==2){int yy=22+n*14;n++;
   x=txt(8,yy,Q[q].n,qs[q]==2?RGB(8,31,8):WHITE,1);
   if(qs[q]==2)txt(x+6,yy,"READY: TURN IN",RGB(8,31,8),1);else{x=num(x+6,yy,qp[q],RGB(31,28,8),1);x=txt(x,yy,"/",WHITE,1);num(x,yy,Q[q].need,WHITE,1);}
   txt(8,yy+6,Q[q].g==0?"FROM GUILD BOARD":Q[q].g==1?"FROM ELDER":Q[q].g==2?"FROM HUNTER":Q[q].g==3?"FROM MERCHANT":"FROM INSTRUCTOR",RGB(18,18,22),1);}
  if(!n)txt(8,y,"NO ACTIVE QUESTS. TALK TO NPCS",RGB(20,20,24),1);}
 txt(8,150,"L/R TAB   UP/DOWN   START CLOSE",RGB(20,20,24),1);}
static void update_menu(void){
 if(kp&8){st=ST_PLAY;return;}
 if(kp&512){mtab=(mtab+3)%4;mc=0;}if(kp&256){mtab=(mtab+1)%4;mc=0;}
 int lim=mtab==1?3:mtab==2?ninv:1;
 if(kp&64)mc=(mc+lim-1)%lim;if(kp&128)mc=(mc+1)%lim;
 if(mtab==1&&(kp&1)&&P.sp>0&&P.skl[mc]<10){P.skl[mc]++;P.sp--;}
 if(mtab==2){if(kp&1)eq=mc;
  if((kp&4)&&ninv>1){for(int i=mc;i<ninv-1;i++)inv[i]=inv[i+1];ninv--;if(eq==mc)eq=0;else if(eq>mc)eq--;if(mc>=ninv)mc=ninv-1;}}}
static void update_class(void){
 if(kp&64)cc=(cc+11)%12;if(kp&128)cc=(cc+1)%12;
 if(kp&1){P.cls=cc;P.grad=1;gate_open=1;for(int i=0;i<3;i++){P.skl[i]=1;P.scd[i]=0;}P.sp=P.lvl;P.hp=pmaxhp();P.mp=pmaxmp();
  for(int i=0;i<NE;i++)en[i].on=0;for(int i=0;i<NP;i++)pj[i].on=0;
  if(cc){addw(mkw(20,45,cc,100));eq=ninv-1;}
  st=ST_PLAY;P.y+=0;say("GRADUATED! THE GATE IS OPEN",0);}}
static void update_branch(void){
 if(kp&(64|128))bc^=1;
 if(kp&1){P.br=bc;P.hp=pmaxhp();st=ST_PLAY;say("PATH CHOSEN: ",BRN[P.cls*2+bc].n);}}
static void draw_dead(void){
 rect(0,0,SW,SH,RGB(8,0,0));txt(60,40,"YOU DIED",RGB(31,6,6),4);
 txt(44,76,"ALL PROGRESS HAS BEEN LOST",WHITE,1);
 int x=txt(80,90,"REACHED LV ",RGB(24,24,24),1);num(x,90,P.lvl,RGB(31,28,8),1);
 if((frame/20)&1)txt(78,120,"PRESS A TO RESTART",WHITE,1);}
static void init(void){gen_tiles();st=ST_TITLE;}
static void frame_step(void){
 u16 k=(u16)(~KEYIN)&0x3FF;kp=k&~pk;pk=k;kd=k;frame++;rs^=(u32)frame*2654435761u;
 switch(st){
  case ST_TITLE:draw_title();if(kp&8){rs^=frame*7919u;newgame();}break;
  case ST_PLAY:update_play();if(st==ST_PLAY||st==ST_DEAD){draw_world();draw_hud();}break;
  case ST_MENU:update_menu();draw_menu();break;
  case ST_CLASS:update_class();draw_class();break;
  case ST_BRANCH:update_branch();draw_branch();break;
  case ST_DEAD:draw_dead();if(kp&1){st=ST_TITLE;}break;}
 if(st==ST_PLAY&&0)draw_hud();
 VSYNC();dma_cpy(VRAM,bb,SW*SH);}
int main(void){
#ifndef HOST
 REG16(0x04000000)=0x0403;
#endif
 init();for(;;)frame_step();return 0;}
