/* ---------------- entities & state ---------------- */
typedef struct {
  float x,y,r,speed,hp,maxHp;
  int level, xp, gold, potions;
  float dmg; int armor, swordBonus;
  int swordLvl, armorLvl, lightLvl;
  int bow, arrows, bowLvl, weapon;      /* weapon: 0 sword, 1 bow */
  float faceX, faceY, walkT;
  bool moving;
  float atkCd, swingT, swingDir, invulnT, hurtFlash;
  float hurtT, regenAcc;
} Player;

typedef struct {
  int type;               /* index into ETYPES */
  float x,y,r,hp,maxHp,spd,dmg,xp;
  float atkCd,shootCd,castCd,summonCd,wanderT,wa,phase,flash;
  float kbx,kby;
  bool flies,shoots,casts,minion,necroMinion,elite,alive,vaultGuard;
  int shrineRef;          /* index into shrines, -1 none */
  float kbr;
} Enemy;

typedef struct {
  int kind;               /* index into BOSSES */
  float x,y,r,hp,maxHp,spd,dmg,xp;
  float atkCd,telegraph,telegraphR,teleA,dashT,dashA;
  float cdVolley,cdSlam,cdSummon,cdBlink,cdRain,cdNova,cdRock,cdDash;
  float phase,flash,kbx,kby;
  bool enraged;
} Boss;

typedef struct { float x,y,vx,vy,life,dmg; int kind; bool friendly,alive; } Arrow;
typedef struct { float x,y,t,val,vx,vy; int type; bool alive; } Drop;   /* 0 coin 1 potion 2 gem 3 arrows */
typedef struct { float x,y,vx,vy,life,max,size,grav; Color c; bool alive; } Particle;
typedef struct { float x,y,life,max; char txt[64]; Color c; bool big; bool alive; } FloatText;
typedef struct { const char *name; int kindr; float x,y,r; Color c,robe; } Npc;

enum { MODE_TITLE=0, MODE_PLAY, MODE_DEAD, MODE_VICTORY };
typedef struct {
  int mode, floor;
  double time; float shakeT, shakeMag, spawnT;
  bool paused, dbg, inv, won, firstVictory;
  bool bossDead[MAXFLOOR+1];
  bool flagDescended, flagGolem, flagMalachar, flagWarden;
  int transPhase;        /* 0 none, 1 out, 2 in */
  float transT;
  int pendingAction;     /* 0 none 1 descend 2 ascend-surface 3 ascend-floor 4 portal */
  int dialogNpc;         /* 0 none 1 elder 2 merchant 3 kid */
  int dialogPage;
  bool sealOpen;
  int nSealTiles;
  int sealTiles[MAXSEAL];
  bool cineOn;
  float cineT, cineDur;
  char cineHead[48], cineTitle[48], cineSub[64];
} GameState;

/* enemy types */
typedef struct { const char *name; float hp,spd,dmg,xp; float r,aggro; bool flies,shoots,casts; float kbr; } EDef;
enum { E_SLIME=0, E_DSLIME, E_BAT, E_SKELETON, E_WRAITH, E_GOLEM, E_NECRO, E_COUNT };
static const EDef ETYPES[E_COUNT] = {
  /* name        hp    spd   dmg   xp    r    aggro flies  shoots casts  kbr  */
  { "Slime",     26,   62,   8,   12,  11,  300, false,false,false, 0    },
  { "Dark Slime",40,   66,  10,   16,  11,  320, false,false,false, 0    },
  { "Cave Bat",  20,  128,   7,   12,   8,  380, true, false,false, 0    },
  { "Skeleton",  56,   72,  11,   26,  11,  340, false,true, false, 0    },
  { "Wraith",    92,   98,  15,   42,  12,  430, false,false,false, 0    },
  { "Stone Golem",185,  38,  22,   66,  14,  360, false,false,false, 0.25f},
  { "Necromancer",80,   56,  12,   58,  12,  520, false,false,true,  0    },
};
static const int SPAWNS[6][6] = {
  /* floors 1..5 spawn tables, -1 terminated */
  { E_DSLIME, E_BAT, E_SKELETON, -1 },
  { E_SKELETON, E_BAT, E_GOLEM, E_DSLIME, -1 },
  { E_SKELETON, E_WRAITH, E_NECRO, E_BAT, -1 },
  { E_WRAITH, E_NECRO, E_GOLEM, E_BAT, -1 },
  { E_WRAITH, E_NECRO, E_GOLEM, E_SKELETON, E_BAT, -1 },
  { -1 },
};

/* bosses: index 0 is the surface Warden, then floors 1..5 */
typedef struct { const char *name; float hp,spd,dmg,xp,r,enrage; } BDef;
enum { B_WARDEN=0, B_BONEKING, B_CINDERGOLEM, B_MALACHAR, B_PYRELORD, B_TYRANT, B_COUNT };
static const BDef BOSSES[B_COUNT] = {
  { "MOUL, THE CORRUPTED WARDEN", 300, 60, 20, 220, 22, 0.40f },
  { "THE BONE KING",              380, 64, 18, 260, 20, 0.35f },
  { "THE CINDER GOLEM",           560, 42, 24, 340, 24, 0.40f },
  { "MALACHAR THE NECRO LORD",    640, 58, 16, 420, 20, 0.40f },
  { "PYRELORD VEX",               780, 74, 20, 520, 22, 0.45f },
  { "THE EMBER TYRANT",          1150, 56, 26, 700, 28, 0.50f },
};
static const int FLOORBOSS[MAXFLOOR+1]={ -1, B_BONEKING, B_CINDERGOLEM, B_MALACHAR, B_PYRELORD, B_TYRANT };

/* quests */
enum { Q_SLIMES=0, Q_WARDEN, Q_BONEKING, Q_FIREBELOW, Q_VEX, Q_TYRANT, Q_COUNT };
enum { QS_NONE=0, QS_ACTIVE, QS_READY, QS_DONE };
typedef struct { const char *title, *goal; } QuestDef;
static const QuestDef QUESTS[Q_COUNT] = {
  { "Slimy Situation",  "Slay 8 slimes around Emberfall" },
  { "The Warden's Seal", "Slay Moul, the Corrupted Warden guarding the depths" },
  { "King of Bones",    "Defeat the Bone King (Floor 1)" },
  { "Fire Below",       "Defeat the Cinder Golem & Malachar (Floors 2-3)" },
  { "Champion of Ash",  "Defeat Pyrelord Vex (Floor 4)" },
  { "The Ember Tyrant", "Defeat the Ember Tyrant (Floor 5)" },
};

static GameState state = { .mode=MODE_TITLE, .floor=0, .firstVictory=true, .sealOpen=true };
static Player player = {
  .x=0, .y=0, .r=10, .speed=175, .hp=100, .maxHp=100,
  .level=1, .xp=0, .gold=25, .potions=2,
  .dmg=12, .armor=0, .swordBonus=0,
  .swordLvl=0, .armorLvl=0, .lightLvl=0,
  .bow=0, .arrows=0, .bowLvl=0, .weapon=0,
  .faceX=1, .faceY=0, .hurtT=999, .regenAcc=0
};
static Enemy enemies[MAXENEMIES]; static int nEnemies=0;
static Boss boss; static bool bossOn=false;
static Arrow arrows[MAXARROWS]; static int nArrows=0;
static Drop drops[MAXDROPS]; static int nDrops=0;
static Particle parts[MAXPARTS]; static int nParts=0;
static FloatText texts[MAXTEXTS]; static int nTexts=0;
static Npc npcs[3]; static int nNpcs=0;
static int killCount=0; static float playTime=0;
static int questId=Q_SLIMES, questState=QS_NONE, questProg=0;

static float camX=0, camY=0;
static int VW=1280, VH=720;
static bool muted=false;
static double musicT=0;

/* ---------------- cross-file prototypes ---------------- */
static void addText(float x,float y,const char *txt,Color c,float life,bool big);
static void burst(float x,float y,Color c,int n,float spd,float life,float size);
static void sfx(int id);
static void shake(float t,float m){ if(t>state.shakeT) state.shakeT=t; if(m>state.shakeMag) state.shakeMag=m; }
static int xpNeed(void){ return (int)(60.0f*powf((float)player.level,1.45f)); }
static void gainXp(int v);
static void hurtPlayer(float dmg,float fx,float fy);
static void damageEnemy(Enemy *e,float dmg,float kx,float ky);
static void damageBoss(float dmg,float kx,float ky);
static void doAttack(float ang);
static void shootBow(float ang);
static void makeEnemy(int type,float x,float y,bool elite,bool vaultGuard,Enemy **out);
static void spawnEnemySurface(void);
static void spawnEnemyFloor(void);
static void populateFloor(void);
static void spawnBoss(int kind,float x,float y);
static void moveEnt(float *x,float *y,float r,float dx,float dy);
static bool circleFree(float x,float y,float r);
static bool findFreeSpot(float x,float y,float r,float *ox,float *oy);
static void genDungeon(int fl);
static void update(float dt);
static void descend(void);
static void ascend(void);
static void interact(void);
static void openChest(Chest *c);
static void activateShrine(Shrine *s);
static void usePortal(void);
static void startTransition(int action);
static void readyIf(int q);
static void updateQuest(void);

static void spawnNpcs(void){
  float vx=VILLAGE_X*TS+TS/2.0f, vy=VILLAGE_Y*TS+TS/2.0f;
  nNpcs=0;
  npcs[nNpcs++] = (Npc){ "Elder Maren", 0, vx-2.2f*TS, vy-2.2f*TS, 11, (Color){122,92,196,255}, (Color){90,63,158,255} };
  npcs[nNpcs++] = (Npc){ "Finn the Trader", 1, vx+4.6f*TS+0.7f*TS, vy+2.8f*TS, 11, (Color){216,162,74,255}, (Color){168,113,44,255} };
  npcs[nNpcs++] = (Npc){ "Little Osk", 2, vx+1.6f*TS, vy+2.2f*TS, 9, (Color){108,196,108,255}, (Color){62,143,62,255} };
}
