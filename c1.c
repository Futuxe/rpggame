/* ============================================================
   EMBERFALL — THE EMBER DEPTHS  ·  native C port (raylib)
   1000x1000 surface world + five dungeon floors, the surface
   Corrupted Warden gate, Ember Seal boss gating, Doors-style
   hidden vaults, bows + inventory, out-of-combat regeneration,
   ray-traced line-of-sight lighting. All art is generated in
   code — no asset files.

   Build (MinGW-w64):
     gcc -O2 -o emberfall.exe emberfall.c -lraylib -lopengl32 -lgdi32 -lwinmm
   ============================================================ */
#include "raylib.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define PI 3.14159265358979f
#define TAU (PI*2.0f)
#define TS 32
#define MAXFLOOR 5
#define MAXMAP 384
#define MAXENEMIES 240
#define MAXARROWS 480
#define MAXDROPS 900
#define MAXPARTS 1400
#define MAXTEXTS 96
#define MAXROOMS 64
#define MAXTORCHES 6000
#define MAXLAVA 6000
#define MAXCHESTS 96
#define MAXSHRINES 16
#define MAXKEYS 512
#define MAXTREES 12000
#define MAXROCKS 3000
#define MAXFLOWERS 7000
#define MAXSEAL 4096
#define MAXVAULTS 4
#define MAXLIGHTBUF (960*540)

/* tiles */
enum { T_WATER=0, T_SAND, T_GRASS, T_CORRUPT, T_WALL, T_FLOOR, T_LAVA, T_STAIRSDOWN, T_STAIRSUP, T_SECRET };
static bool solidTile(int t){ return t==T_WATER || t==T_WALL || t==T_SECRET; }

/* ---------------- utils ---------------- */
static float clampf(float v, float a, float b){ return v<a?a:(v>b?b:v); }
static float lerpf(float a, float b, float t){ return a+(b-a)*t; }
static float distf(float ax, float ay, float bx, float by){ float dx=ax-bx, dy=ay-by; return sqrtf(dx*dx+dy*dy); }
static float rndf(float a, float b){ return a + (b-a)*((float)rand()/(float)RAND_MAX); }
static float rnd0(float a){ return a*((float)rand()/(float)RAND_MAX); }
static unsigned int hash2i(unsigned int x, unsigned int y){
  unsigned int n = (unsigned int)(sinf((float)x*127.1f + (float)y*311.7f)*43758.5453f*2654435761.0f);
  n ^= n >> 15; n *= 2246822519u; n ^= n >> 13; n *= 3266489917u; n ^= n >> 16;
  return n;
}
static float hash2(unsigned int x, unsigned int y){ return (hash2i(x,y) & 0xFFFFFF) / 16777215.0f; }
static float smoothf(float t){ return t*t*(3-2*t); }
static float vnoise(float x, float y){
  int xi=(int)floorf(x), yi=(int)floorf(y); float xf=x-xi, yf=y-yi;
  float a=hash2((unsigned)xi,(unsigned)yi), b=hash2((unsigned)xi+1,(unsigned)yi);
  float c=hash2((unsigned)xi,(unsigned)yi+1), d=hash2((unsigned)xi+1,(unsigned)yi+1);
  float u=smoothf(xf), v=smoothf(yf);
  return a*(1-u)*(1-v)+b*u*(1-v)+c*(1-u)*v+d*u*v;
}
static float fbm(float x, float y){ return vnoise(x,y)*0.55f + vnoise(x*2.13f,y*2.13f)*0.28f + vnoise(x*4.31f,y*4.31f)*0.17f; }

/* ---------------- world ---------------- */
static int mapW=1000, mapH=1000;
static unsigned char tiles[MAXMAP*MAXMAP];
static float tileVar[MAXMAP*MAXMAP];
static int tileAt(int tx,int ty){ return (tx<0||ty<0||tx>=mapW||ty>=mapH)?T_WALL:tiles[ty*mapW+tx]; }
static void setTile(int tx,int ty,int t){ if(tx>=0&&ty>=0&&tx<mapW&&ty<mapH) tiles[ty*mapW+tx]=(unsigned char)t; }
static int tileAtPx(float x,float y){ return tileAt((int)floorf(x/TS),(int)floorf(y/TS)); }

static const int VILLAGE_X=500, VILLAGE_Y=500, VILLAGE_R=13;
static const int CORRUPT_X=680, CORRUPT_Y=700, CORRUPT_R=60;

typedef struct { float x,y; int cr; bool corrupt,big; float sway; } Tree;
typedef struct { float x,y,r; bool corrupt; } Rock;
typedef struct { float x,y; Color c; float sway; } Flower;
typedef struct { float x,y; int sx,sy; float fl; } Torch;
typedef struct { float x,y; int w,h; Color roof,wall; bool door,stall,well; } Building;
typedef struct { float x,y; bool opened; int id; bool vault; } Chest;
typedef struct { float x,y; int fl; bool active,done; int left,id; } Shrine;
typedef struct { int x,y,w,h,cx,cy; bool seen; int id; } VaultDef;

static Tree trees[MAXTREES]; static int nTrees=0;
static int treeHead[MAXMAP*MAXMAP]; static int treeNext[MAXTREES];
static Rock rocks[MAXROCKS]; static int nRocks=0;
static Flower flowers[MAXFLOWERS]; static int nFlowers=0;
static Torch torches[MAXTORCHES]; static int nTorches=0;
static Building buildings[16]; static int nBuildings=0;
static Chest chests[MAXCHESTS]; static int nChests=0;
static Shrine shrines[MAXSHRINES]; static int nShrines=0;
static int lavas[MAXLAVA][2]; static int nLavas=0;
static float entranceX=0, entranceY=0;
static float portalX=0, portalY=0; static bool portalOn=false;

/* Ember Seal: sealed tiles packed as ty*MAXMAP+tx */
static int sealTiles[MAXSEAL]; static int nSealTiles=0; static bool sealOpen=true;
static VaultDef vaults[MAXVAULTS]; static int nVaults=0;

static const char *FLNAMES[MAXFLOOR+1]={ "", "The Bone Hollows", "The Molten Gaol", "The Wailing Crypt", "The Ashen Deep", "The Ember Throne" };

static int openedChestKeys[MAXKEYS]; static int nOpened=0;
static int doneShrineKeys[MAXKEYS]; static int nDone=0;
static bool keyIn(int *arr,int n,int k){ for(int i=0;i<n;i++) if(arr[i]==k) return true; return false; }

/* rooms */
typedef struct { int x,y,w,h,cx,cy; } Room;
static Room rooms[MAXROOMS]; static int nRooms=0;
static Room bossRoom; static float dstartX=0, dstartY=0;

static void resetWorldArrays(void){
  nTrees=nRocks=nFlowers=nTorches=nBuildings=nChests=nShrines=nLavas=nRooms=0;
  portalOn=false;
}

/* tree spatial buckets: O(1) lookup per tile */
static void buildTreeGrid(void){
  for(int i=0;i<mapW*mapH;i++) treeHead[i]=-1;
  for(int i=nTrees-1;i>=0;i--){
    int tx=(int)(trees[i].x/TS), ty=(int)(trees[i].y/TS);
    if(tx<0||ty<0||tx>=mapW||ty>=mapH) continue;
    int idx=ty*mapW+tx;
    treeNext[i]=treeHead[idx]; treeHead[idx]=i;
  }
}

/* half-resolution height field so a million tiles generate instantly */
static float hmap[(1000/2+2)*(1000/2+2)];
static int hw=0, hh=0;
static float hs(int x,int y){
  float fx=x*0.5f, fy=y*0.5f;
  int x0=(int)fx, y0=(int)fy; float tx=fx-x0, ty=fy-y0;
  float a=hmap[y0*hw+x0], b=hmap[y0*hw+x0+1];
  float c=hmap[(y0+1)*hw+x0], d=hmap[(y0+1)*hw+x0+1];
  return a*(1-tx)*(1-ty)+b*tx*(1-ty)+c*(1-tx)*ty+d*tx*ty;
}

/* ---------- surface generation ---------- */
static void genSurface(void){
  mapW=1000; mapH=1000;
  resetWorldArrays();
  hw=(mapW>>1)+2; hh=(mapH>>1)+2;
  for(int y=0;y<hh;y++)for(int x=0;x<hw;x++)
    hmap[y*hw+x]=fbm(x*0.15f+13.7f, y*0.15f+5.1f);
  for(int y=0;y<mapH;y++)for(int x=0;x<mapW;x++){
    float h=hs(x,y);
    int t = h<0.345f ? T_WATER : T_GRASS;
    if(distf((float)x,(float)y,VILLAGE_X,VILLAGE_Y) < VILLAGE_R) t = T_GRASS;
    if(distf((float)x,(float)y,CORRUPT_X,CORRUPT_Y) < 44) t = T_GRASS;
    if(t==T_GRASS){
      float d=distf((float)x,(float)y,CORRUPT_X,CORRUPT_Y);
      float edge = CORRUPT_R*(0.72f+0.5f*vnoise(x*0.045f,y*0.045f));
      if(d<edge) t=T_CORRUPT;
    }
    tiles[y*mapW+x]=(unsigned char)t;
    tileVar[y*mapW+x]=hash2((unsigned)x*3u,(unsigned)y*7u);
  }
  for(int y=0;y<mapH;y++)for(int x=0;x<mapW;x++){
    int i=y*mapW+x;
    if(tiles[i]!=T_GRASS && tiles[i]!=T_CORRUPT) continue;
    if(tileAt(x-1,y)==T_WATER||tileAt(x+1,y)==T_WATER||tileAt(x,y-1)==T_WATER||tileAt(x,y+1)==T_WATER)
      tiles[i]=T_SAND;
  }
  const int ex=CORRUPT_X, ey=CORRUPT_Y;
  for(int y=1;y<mapH-1;y++)for(int x=1;x<mapW-1;x++){
    int t=tiles[y*mapW+x];
    float px=x*TS+TS/2.0f, py=y*TS+TS/2.0f;
    bool inVillage = distf((float)x,(float)y,VILLAGE_X,VILLAGE_Y)<VILLAGE_R;
    bool nearPortal = distf((float)x,(float)y,(float)ex,(float)ey)<5.4f;
    float n=vnoise(x*0.16f+40, y*0.16f+9);
    if((t==T_GRASS||t==T_CORRUPT) && !inVillage && !nearPortal){
      if(n>0.80f && hash2((unsigned)x*9u,(unsigned)y*4u)<0.10f && nTrees<MAXTREES){
        Tree tr={ px+rndf(-6,6), py+rndf(-6,6), 9, t==T_CORRUPT, hash2((unsigned)x,(unsigned)y)>0.6f, rnd0(TAU) };
        trees[nTrees++]=tr;
      } else if(n<0.18f && hash2((unsigned)x*5u,(unsigned)y*2u)<0.02f && nRocks<MAXROCKS){
        Rock rk={ px+rndf(-5,5), py+rndf(-5,5), rndf(5,8), t==T_CORRUPT };
        rocks[nRocks++]=rk;
      }
    }
    if(t==T_GRASS && hash2((unsigned)x*7u,(unsigned)y*3u)<0.008f && !inVillage && !nearPortal && nFlowers<MAXFLOWERS){
      Color cs[5]={ {242,209,107,255},{226,109,143,255},{201,226,109,255},{226,162,60,255},{185,143,226,255} };
      Flower fl={ px+rndf(-12,12), py+rndf(-12,12), cs[rand()%5], rnd0(TAU) };
      flowers[nFlowers++]=fl;
    }
  }
  int hx=VILLAGE_X, hy=VILLAGE_Y;
  buildings[nBuildings++] = (Building){ (hx-4)*TS,(hy-5)*TS, 3*TS,(int)(2.6f*TS), (Color){138,75,50,255}, (Color){216,196,154,255}, true,false,false };
  buildings[nBuildings++] = (Building){ (hx+2)*TS,(int)((hy-4.6f)*TS), (int)(2.4f*TS),(int)(2.2f*TS), (Color){110,106,143,255}, (Color){207,196,168,255}, true,false,false };
  buildings[nBuildings++] = (Building){ (hx-1)*TS,(hy+4)*TS, (int)(2.2f*TS),2*TS, (Color){78,122,92,255}, (Color){211,200,171,255}, true,false,false };
  buildings[nBuildings++] = (Building){ (int)((hx+4.3f)*TS),(int)((hy+1.2f)*TS), (int)(1.4f*TS),(int)(1.1f*TS), (Color){179,64,47,255}, (Color){122,90,58,255}, false,true,false };
  buildings[nBuildings++] = (Building){ (int)((hx-0.1f)*TS),(int)((hy-0.7f)*TS), TS,TS, (Color){85,85,85,255}, (Color){136,136,136,255}, false,false,true };
  /* dungeon entrance in the heart of the corruption — clear 5x5, no trees */
  for(int dy=-2;dy<=2;dy++)for(int dx=-2;dx<=2;dx++){
    int t=tileAt(ex+dx,ey+dy);
    if(t==T_WATER || t==T_GRASS) setTile(ex+dx,ey+dy, (dx==0&&dy==0)?T_STAIRSDOWN:T_CORRUPT);
  }
  setTile(ex,ey,T_STAIRSDOWN);
  entranceX=ex*TS+TS/2.0f; entranceY=ey*TS+TS/2.0f;
  for(int y=0;y<mapH;y++)for(int x=0;x<mapW;x++){
    int t=tiles[y*mapW+x];
    if((t==T_GRASS||t==T_CORRUPT) && distf((float)x,(float)y,(float)ex,(float)ey)<6) tiles[y*mapW+x]=T_CORRUPT;
  }
  /* brazier torches ringing the portal */
  float off[4][2]={{-2.2f,-2.2f},{2.2f,-2.2f},{-2.2f,2.2f},{2.2f,2.2f}};
  for(int i=0;i<4;i++){
    Torch t={ (ex+off[i][0])*TS, (ey+off[i][1])*TS, 0,0, rnd0(TAU) };
    torches[nTorches++]=t;
  }
  /* a few village lanterns */
  float vt[3][2]={{-3.6f,-3.4f},{3.4f,-3.2f},{-0.4f,3.6f}};
  for(int i=0;i<3;i++){
    Torch t={ (hx+vt[i][0])*TS, (hy+vt[i][1])*TS, 0,0, rnd0(TAU) };
    torches[nTorches++]=t;
  }
  buildTreeGrid();
}
