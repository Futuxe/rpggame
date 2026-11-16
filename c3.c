/* ---------------- combat, AI, spawning, seal & regen ---------------- */
static void gainXp(int v){
  player.xp+=v;
  addText(player.x+rndf(-6,6),player.y-24,TextFormat("+%d xp",v),(Color){154,209,255,255},0.8f,false);
  int need=xpNeed();
  while(player.xp>=need){
    player.xp-=need; player.level++;
    player.maxHp+=20; player.hp=fminf(player.maxHp,player.hp+player.maxHp*0.5f);
    player.dmg+=3; need=xpNeed();
    addText(player.x,player.y-40,TextFormat("LEVEL UP!  Lv.%d",player.level),(Color){255,215,94,255},1.6f,true);
    burst(player.x,player.y,(Color){255,215,94,255},22,170,0.8f,4);
    sfx(7);
  }
}
static void hurtPlayer(float dmg,float fx,float fy){
  if(player.invulnT>0 || state.mode!=MODE_PLAY) return;
  float red=1-fminf(0.5f,player.armor*0.08f);
  int d=(int)fmaxf(1,roundf(dmg*red));
  player.hp-=d; player.invulnT=0.7f; player.hurtFlash=0.3f;
  player.hurtT=0; player.regenAcc=0;
  addText(player.x,player.y-26,TextFormat("-%d",d),(Color){255,107,107,255},0.9f,false);
  float a=atan2f(player.y-fy,player.x-fx);
  moveEnt(&player.x,&player.y,player.r,cosf(a)*14,sinf(a)*14);
  shake(0.25f,5); sfx(4);
  if(player.hp<=0){
    player.hp=0; state.mode=MODE_DEAD; sfx(6);
    burst(player.x,player.y,(Color){255,107,107,255},24,160,0.9f,4);
  }
}
static void dropLoot(float x,float y,bool rich){
  int n=rich?10:4+rand()%4;
  for(int i=0;i<n && nDrops<MAXDROPS;i++)
    drops[nDrops++] = (Drop){ x+rndf(-14,14), y+rndf(-14,14), rnd0(TAU), (float)(2+rand()%5), rndf(-60,60), rndf(-60,60), 0, true };
  if(rand()%100<45 && nDrops<MAXDROPS)
    drops[nDrops++] = (Drop){ x,y-6,rnd0(TAU),1,0,0,1,true };
  if(rand()%100<40 && nDrops<MAXDROPS)
    drops[nDrops++] = (Drop){ x+rndf(-10,10),y+rndf(-10,10),rnd0(TAU),(float)(20+rand()%31),rndf(-40,40),rndf(-40,40),2,true };
  if(rich || rand()%100<35){
    if(nDrops<MAXDROPS) drops[nDrops++] = (Drop){ x+rndf(-10,10),y+rndf(-10,10),rnd0(TAU),(float)(5+rand()%7),rndf(-30,30),rndf(-30,30),3,true };
  }
  if(rich) for(int i=0;i<6 && nDrops<MAXDROPS;i++)
    drops[nDrops++] = (Drop){ x+rndf(-18,18),y+rndf(-18,18),rnd0(TAU),(float)(60+rand()%71),rndf(-50,50),rndf(-50,50),2,true };
}
static void killEnemy(Enemy *e){
  int idx=-1; for(int i=0;i<nEnemies;i++) if(&enemies[i]==e){ idx=i; break; }
  if(idx<0) return;
  Color bc = e->type==E_SLIME?(Color){95,206,98,255} : e->type==E_SKELETON?(Color){232,232,232,255}
           : e->type==E_BAT?(Color){138,110,160,255} : (Color){176,108,226,255};
  burst(e->x,e->y,bc,e->elite?26:14,e->elite?190:140,0.6f,e->elite?4.5f:3.5f);
  sfx(5);
  gainXp((int)e->xp);
  killCount++;
  dropLoot(e->x,e->y,e->elite);
  if(e->type==E_SLIME || e->type==E_DSLIME){
    if(questId==Q_SLIMES && questState==QS_ACTIVE){ questProg++; readyIf(Q_SLIMES); }
  }
  if(e->shrineRef>=0 && e->shrineRef<nShrines){
    Shrine *s=&shrines[e->shrineRef];
    s->left--;
    if(s->left<=0 && s->active){
      s->active=false; s->done=true;
      if(nDone<MAXKEYS) doneShrineKeys[nDone++]=s->id;
      sfx(8); burst(s->x,s->y,(Color){126,231,135,255},30,180,0.9f,4);
      addText(s->x,s->y-30,"Shrine cleared!",(Color){126,231,135,255},2.4f,true);
      for(int i=0;i<8 && nDrops<MAXDROPS;i++)
        drops[nDrops++] = (Drop){ s->x+rndf(-26,26),s->y+rndf(-26,26),rnd0(TAU),(float)(3+rand()%5),0,0,0,true };
    }
  }
  enemies[idx]=enemies[--nEnemies];
  (void)e;
}
static void damageEnemy(Enemy *e,float dmg,float kx,float ky){
  e->hp-=dmg; e->flash=0.15f;
  float kf=1-e->kbr;
  e->kbx+=kx*kf; e->kby+=ky*kf;
  addText(e->x+rndf(-8,8),e->y-e->r-6,TextFormat("%d",(int)roundf(dmg)),(Color){255,215,94,255},0.9f,false);
  burst(e->x,e->y,(Color){255,123,84,255},6,110,0.35f,2.5f);
  sfx(3);
  if(e->hp<=0) killEnemy(e);
}
static void damageBoss(float dmg,float kx,float ky){
  if(!bossOn) return;
  boss.hp-=dmg; boss.flash=0.14f;
  boss.kbx+=kx*0.25f; boss.kby+=ky*0.25f;
  addText(boss.x+rndf(-10,10),boss.y-boss.r-8,TextFormat("%d",(int)roundf(dmg)),(Color){255,215,94,255},0.9f,false);
  burst(boss.x,boss.y,(Color){255,123,84,255},8,120,0.4f,3);
  sfx(3);
  if(boss.hp>0 && !boss.enraged && boss.hp < boss.maxHp*BOSSES[boss.kind].enrage){
    boss.enraged=true;
    addText(boss.x,boss.y-boss.r-30,"ENRAGED!",(Color){255,90,60,255},2.2f,true);
    shake(0.5f,7); sfx(9);
  }
  if(boss.hp<=0){
    int kind=boss.kind;
    float bx=boss.x, by=boss.y;
    bossOn=false;
    gainXp((int)BOSSES[kind].xp);
    burst(bx,by,(Color){255,160,60,255},46,240,1.2f,5);
    shake(0.9f,11); sfx(9);
    for(int i=0;i<10 && nDrops<MAXDROPS;i++)
      drops[nDrops++] = (Drop){ bx+rndf(-30,30),by+rndf(-30,30),rnd0(TAU),(float)(20+rand()%51),rndf(-70,70),rndf(-70,70),2,true };
    for(int i=0;i<6 && nDrops<MAXDROPS;i++)
      drops[nDrops++] = (Drop){ bx+rndf(-26,26),by+rndf(-26,26),rnd0(TAU),(float)(4+rand()%6),rndf(-50,50),rndf(-50,50),0,true };
    if(kind==B_WARDEN){
      state.flagWarden=true;
      addText(bx,by-40,"THE WARDEN FALLS",(Color){255,160,90,255},3,true);
      addText(bx,by-64,"The stairway to the Depths is open!",(Color){255,224,138,255},3,true);
      readyIf(Q_WARDEN);
    } else {
      int fl=state.floor;
      state.bossDead[fl]=true;
      if(kind==B_CINDERGOLEM) state.flagGolem=true;
      if(kind==B_MALACHAR) state.flagMalachar=true;
      addText(bx,by-40,BOSSES[kind].name,(Color){255,160,90,255},2.6f,true);
      addText(bx,by-64,"is slain.",(Color){201,209,217,255},2.6f,true);
      if(fl==1) readyIf(Q_BONEKING);
      if(fl==3) readyIf(Q_FIREBELOW);
      if(fl==4) readyIf(Q_VEX);
      if(fl==5){
        readyIf(Q_TYRANT);
        state.won=true; state.mode=MODE_VICTORY;
      }
    }
  }
}
static void doAttack(float ang){
  if(player.weapon==1){ if(player.atkCd<=0) shootBow(ang); return; }
  if(player.atkCd>0) return;
  player.atkCd=0.38f; player.swingT=0.18f; player.swingDir=ang;
  player.faceX=cosf(ang); player.faceY=sinf(ang);
  sfx(0);
  float range=48, dmg=player.dmg+player.swordBonus;
  for(int i=0;i<nEnemies;i++){
    Enemy *e=&enemies[i];
    float d=distf(player.x,player.y,e->x,e->y);
    if(d>range+e->r) continue;
    float a=atan2f(e->y-player.y,e->x-player.x);
    float diff=fabsf(fmodf(a-ang+TAU+PI,TAU)-PI);
    if(diff<1.25f) damageEnemy(e,dmg,cosf(a)*180,sinf(a)*180);
  }
  if(bossOn){
    float d=distf(player.x,player.y,boss.x,boss.y);
    if(d<range+boss.r){
      float a=atan2f(boss.y-player.y,boss.x-player.x);
      float diff=fabsf(fmodf(a-ang+TAU+PI,TAU)-PI);
      if(diff<1.25f) damageBoss(dmg,cosf(a)*180,sinf(a)*180);
    }
  }
}
static void shootBow(float ang){
  if(player.arrows<=0){ addText(player.x,player.y-26,"No arrows!",(Color){255,154,154,255},0.9f,false); sfx(10); return; }
  player.arrows--;
  player.atkCd=0.42f; player.swingT=0.12f; player.swingDir=ang;
  player.faceX=cosf(ang); player.faceY=sinf(ang);
  float dmg=9+player.bowLvl*5;
  if(nArrows<MAXARROWS){
    arrows[nArrows++] = (Arrow){ player.x+cosf(ang)*14, player.y+sinf(ang)*14,
      cosf(ang)*470, sinf(ang)*470, 1.2f, dmg, 0, true, true };
  }
  sfx(11);
}
static void makeEnemy(int type,float x,float y,bool forceElite,bool vaultGuard,Enemy **out){
  if(nEnemies>=MAXENEMIES){ if(out)*out=NULL; return; }
  EDef d=ETYPES[type];
  int fl=state.floor>0?state.floor:0;
  float boost=(1+fl*0.26f)*(1+(player.level-1)*0.06f);
  bool elite = forceElite || (!vaultGuard && fl>0 && rnd0(1.0f)<0.10f+fl*0.025f);
  Enemy *e=&enemies[nEnemies++];
  *e = (Enemy){ type,x,y,d.r, d.hp*boost,d.hp*boost, d.spd*(elite?0.92f:1), d.dmg*boost*(elite?1.5f:1),
    d.xp*(elite?2.5f:1), rndf(0.5f,1.5f),rndf(1,2.5f),rndf(2,4),rndf(4,7),
    0,rnd0(TAU),rnd0(TAU),0, 0,0,
    d.flies,d.shoots,d.casts,false,false,elite,true,vaultGuard, -1, d.kbr };
  if(elite) e->r*=1.35f;
  if(out)*out=e;
}
static int enemyCapFloor(void){ int fl=state.floor; return 18+fl*5; }
static void spawnEnemySurface(void){
  if(state.mode!=MODE_PLAY) return;
  int cap=22+player.level*2; if(cap>40) cap=40;
  if(nEnemies>=cap) return;
  float ang=rnd0(TAU), d=rndf(560,980);
  float x=player.x+cosf(ang)*d, y=player.y+sinf(ang)*d;
  if(!circleFree(x,y,12)) return;
  int tx=(int)floorf(x/TS), ty=(int)floorf(y/TS);
  if(distf((float)tx,(float)ty,VILLAGE_X,VILLAGE_Y) < VILLAGE_R+3) return;
  bool inCorrupt = tileAt(tx,ty)==T_CORRUPT;
  int type;
  if(inCorrupt) type = rnd0(1)<0.65f?E_WRAITH:E_SKELETON;
  else if(distf((float)tx,(float)ty,VILLAGE_X,VILLAGE_Y)>30) type = rnd0(1)<0.55f?E_SKELETON:E_SLIME;
  else type=E_SLIME;
  makeEnemy(type,x,y,false,false,NULL);
}
static void spawnEnemyFloor(void){
  int fl=state.floor, cap=enemyCapFloor();
  if(nEnemies>=cap || nRooms==0) return;
  Room *r=&rooms[rand()%nRooms];
  if(r->cx==bossRoom.cx && r->cy==bossRoom.cy && !state.bossDead[fl]) return;
  float x=(r->x+1+rand()%(r->w-2))*TS, y=(r->y+1+rand()%(r->h-2))*TS;
  float sx,sy; findFreeSpot(x,y,12,&sx,&sy);
  if(distf(sx,sy,player.x,player.y)<460) return;
  const int *pool=SPAWNS[fl]; int n=0; while(pool[n]>=0) n++;
  if(n==0) return;
  makeEnemy(pool[rand()%n],sx,sy,false,false,NULL);
}
static void populateFloor(void){
  nEnemies=0; nArrows=0; nDrops=0;
  int fl=state.floor;
  if(fl==0) return;
  int count=12+fl*5;
  for(int i=0;i<count && nRooms>0;i++){
    Room *r=&rooms[rand()%nRooms];
    if(r->cx==bossRoom.cx && r->cy==bossRoom.cy) continue;
    float x=(r->x+1+rnd0((float)(r->w-2)))*TS, y=(r->y+1+rnd0((float)(r->h-2)))*TS;
    float sx,sy; findFreeSpot(x,y,12,&sx,&sy);
    if(distf(sx,sy,player.x,player.y)<380) continue;
    const int *pool=SPAWNS[fl]; int n=0; while(pool[n]>=0) n++;
    if(n==0) continue;
    makeEnemy(pool[rand()%n],sx,sy,false,false,NULL);
  }
  for(int v=0;v<nVaults;v++){
    VaultDef *vt=&vaults[v];
    for(int i=0;i<2+(fl>>1);i++){
      float sx,sy;
      findFreeSpot((vt->x+1.5f+rnd0(vt->w-3.0f))*TS,(vt->y+1.5f+rnd0(vt->h-3.0f))*TS,12,&sx,&sy);
      makeEnemy(SPAWNS[fl][0],sx,sy,true,true,NULL);
    }
  }
  sealOpen=state.sealOpen;
}
static void spawnBoss(int kind,float x,float y){
  if(bossOn) return;
  BDef b=BOSSES[kind];
  boss=(Boss){ kind,x,y,b.r, b.hp,b.hp, b.spd,b.dmg,b.xp,
    1.0f,0,0,0,0,0, 3,2,5,4,5,6,4,3, rnd0(TAU),0,0,0,false };
  bossOn=true;
  addText(x,y-b.r-24,b.name,(Color){255,140,80,255},2.8f,true);
  shake(0.5f,8); sfx(9);
}
static void updateBoss(float dt){
  if(!bossOn) return;
  float dx=player.x-boss.x, dy=player.y-boss.y;
  float d=sqrtf(dx*dx+dy*dy)+0.001f;
  float spd=boss.spd*(boss.enraged?1.35f:1);
  if(d>70){ moveEnt(&boss.x,&boss.y,boss.r, dx/d*spd*dt, dy/d*spd*dt); }
  boss.kbx*=0.88f; boss.kby*=0.88f;
  if(fabsf(boss.kbx)+fabsf(boss.kby)>0.2f) moveEnt(&boss.x,&boss.y,boss.r,boss.kbx*dt,boss.kby*dt);
  boss.atkCd-=dt;
  if(boss.atkCd<=0){
    boss.atkCd = boss.enraged?1.1f:1.6f;
    if(d<90){
      hurtPlayer(boss.dmg*1.4f, boss.x,boss.y);
      burst(player.x,player.y,(Color){255,130,50,255},18,160,0.5f,4);
      shake(0.3f,6);
    } else {
      int n=boss.enraged?12:8;
      float base=atan2f(player.y-boss.y,player.x-boss.x);
      for(int i=0;i<n;i++){
        float a=base+(i-(n-1)/2.0f)*(TAU/n);
        if(nArrows<MAXARROWS)
          arrows[nArrows++] = (Arrow){ boss.x+cosf(a)*boss.r, boss.y+sinf(a)*boss.r,
            cosf(a)*250, sinf(a)*250, 3.0f, boss.dmg*0.6f, 1, false, true };
      }
      sfx(2);
    }
  }
  if(boss.kind==B_MALACHAR || boss.kind==B_NECRO){
    /* handled below for necromancer boss only */
  }
  if(boss.kind==B_MALACHAR){
    boss.cdSummon-=dt;
    if(boss.cdSummon<=0){
      boss.cdSummon=boss.enraged?6:9;
      for(int i=0;i<2;i++){
        float a=rnd0(TAU), rr=boss.r+30;
        float sx,sy; if(findFreeSpot(boss.x+cosf(a)*rr,boss.y+sinf(a)*rr,10,&sx,&sy)){
          Enemy *e; makeEnemy(E_SKELETON,sx,sy,false,false,&e);
          if(e){ e->minion=true; e->xp*=0.2f; }
        }
      }
      addText(boss.x,boss.y-boss.r-16,"RISE!",(Color){196,107,255,255},1.4f,true);
    }
  }
  if(boss.flash>0) boss.flash-=dt;
}
static void updateEnemies(float dt){
  for(int i=0;i<nEnemies;i++){
    Enemy *e=&enemies[i];
    if(e->flash>0) e->flash-=dt;
    float dx=player.x-e->x, dy=player.y-e->y;
    float d=sqrtf(dx*dx+dy*dy)+0.001f;
    bool aggro = d < ETYPES[e->type].aggro;
    e->wanderT-=dt;
    if(aggro){
      if(e->shoots && d<380 && d>70){
        e->shootCd-=dt;
        if(e->shootCd<=0){
          e->shootCd=rndf(1.6f,2.6f);
          float a=atan2f(dy,dx);
          if(nArrows<MAXARROWS)
            arrows[nArrows++] = (Arrow){ e->x+cosf(a)*e->r,e->y+sinf(a)*e->r,
              cosf(a)*240,sinf(a)*240,2.6f,e->dmg*0.8f,1,false,true };
          sfx(2);
        }
      }
      if(d>e->r+14) avoidLavaEnt(e, dx/d*e->spd*dt, dy/d*e->spd*dt);
      else {
        e->atkCd-=dt;
        if(e->atkCd<=0){
          e->atkCd=1.0f;
          hurtPlayer(e->dmg,e->x,e->y);
          if(e->necroMinion) hurtPlayer(0,e->x,e->y);
        }
      }
      if(e->casts && d<520){
        e->castCd-=dt;
        if(e->castCd<=0 && nEnemies<enemyCapFloor()+6){
          e->castCd=rndf(5,8);
          float a=rnd0(TAU);
          float sx,sy; if(findFreeSpot(e->x+cosf(a)*40,e->y+sinf(a)*40,9,&sx,&sy)){
            Enemy *m; makeEnemy(E_SKELETON,sx,sy,false,false,&m);
            if(m){ m->minion=true; m->xp*=0.2f; }
          }
        }
      }
    } else {
      if(e->wanderT<=0){ e->wanderT=rndf(1.5f,3.5f); e->wa=rnd0(TAU); }
      if(!e->shoots || d>240) avoidLavaEnt(e, cosf(e->wa)*e->spd*0.35f*dt, sinf(e->wa)*e->spd*0.35f*dt);
    }
    /* knockback with collision (never into walls) */
    if(fabsf(e->kbx)+fabsf(e->kby)>0.5f){
      moveEnt(&e->x,&e->y,e->r,e->kbx*dt,e->kby*dt);
      e->kbx*=0.86f; e->kby*=0.86f;
    }
  }
}
static void updateArrows(float dt){
  for(int i=0;i<nArrows;i++){
    Arrow *a=&arrows[i];
    if(!a->alive) continue;
    a->life-=dt; a->x+=a->vx*dt; a->y+=a->vy*dt;
    if(a->life<=0 || !circleFree(a->x,a->y,3)){
      a->alive=false; continue;
    }
    if(a->friendly){
      for(int j=0;j<nEnemies;j++){
        Enemy *e=&enemies[j];
        if(distf(a->x,a->y,e->x,e->y)<e->r+4){
          damageEnemy(e,a->dmg,a->vx*0.35f,a->vy*0.35f);
          a->alive=false; break;
        }
      }
      if(a->alive && bossOn && distf(a->x,a->y,boss.x,boss.y)<boss.r+4){
        damageBoss(a->dmg,a->vx*0.3f,a->vy*0.3f);
        a->alive=false;
      }
    } else if(distf(a->x,a->y,player.x,player.y)<player.r+4){
      hurtPlayer(a->dmg,a->x-a->vx,a->y-a->vy);
      a->alive=false;
    }
  }
  int w=0;
  for(int i=0;i<nArrows;i++) if(arrows[i].alive) arrows[w++]=arrows[i];
  nArrows=w;
}
static void updateDrops(float dt){
  for(int i=0;i<nDrops;i++){
    Drop *d=&drops[i];
    if(!d->alive) continue;
    d->t+=dt*4; d->x+=d->vx*dt; d->y+=d->vy*dt;
    d->vx*=0.9f; d->vy*=0.9f;
    float dd=distf(d->x,d->y,player.x,player.y);
    if(dd<70){ float a=atan2f(player.y-d->y,player.x-d->x); float pull=clampf((70-dd)*8,60,320);
      d->x+=cosf(a)*pull*dt; d->y+=sinf(a)*pull*dt; }
    if(dd<player.r+10){
      d->alive=false;
      if(d->type==0){ player.gold+=(int)d->val; addText(d->x,d->y-10,TextFormat("+%dg",(int)d->val),(Color){255,215,94,255},0.7f,false); sfx(1); }
      else if(d->type==1){ player.potions++; addText(d->x,d->y-10,"+1 potion",(Color){255,123,143,255},0.9f,false); sfx(8); }
      else if(d->type==3){ player.arrows+=(int)d->val; addText(d->x,d->y-10,TextFormat("+%d arrows",(int)d->val),(Color){232,226,208,255},0.9f,false); sfx(11); }
      else { gainXp((int)d->val); sfx(8); }
    }
  }
  int w=0;
  for(int i=0;i<nDrops;i++) if(drops[i].alive) drops[w++]=drops[i];
  nDrops=w;
}
static void updateRegen(float dt){
  player.hurtT+=dt;
  if(player.hurtT>=30 && player.hp<player.maxHp){
    player.regenAcc+=dt;
    while(player.regenAcc>=1){ player.regenAcc-=1; player.hp=fminf(player.maxHp,player.hp+1); }
  } else if(player.hp>=player.maxHp) player.regenAcc=0;
}
static void updateSeal(float dt){
  (void)dt;
  if(state.floor<=0 || state.sealOpen || state.nSealTiles<=0) return;
  int left=0;
  for(int i=0;i<nEnemies;i++){
    Enemy *e=&enemies[i];
    if(e->minion || e->shrineRef>=0 || e->vaultGuard) continue;
    left++;
  }
  if(left==0){
    for(int i=0;i<state.nSealTiles;i++){
      int key=state.sealTiles[i];
      setTile(key%MAXMAP,key/MAXMAP,T_FLOOR);
    }
    state.sealOpen=true; sealOpen=true;
    sfx(8); shake(0.7f,9);
    burst(bossRoom.cx*TS+TS/2.0f,bossRoom.cy*TS+TS/2.0f,(Color){255,154,61,255},40,230,1.1f,4);
    addText(player.x,player.y-64,"THE EMBER SEAL SHATTERS",(Color){255,138,94,255},3,true);
    addText(player.x,player.y-88,"The guardian awaits...",(Color){201,209,217,255},2.6f,true);
  }
}
static void updateSpawns(float dt){
  /* surface Warden gate */
  if(state.floor==0 && !state.flagWarden && !bossOn){
    if(entranceX!=0 && distf(player.x,player.y,entranceX,entranceY)<260)
      spawnBoss(B_WARDEN, entranceX, entranceY-90);
  }
  /* floor guardian once the seal is broken */
  if(state.floor>0 && !state.bossDead[state.floor] && !bossOn && state.sealOpen){
    float brx=bossRoom.cx*TS+TS/2.0f, bry=bossRoom.cy*TS+TS/2.0f;
    if(fabsf(player.x-brx)<bossRoom.w*TS*0.5f+48 && fabsf(player.y-bry)<bossRoom.h*TS*0.5f+48)
      spawnBoss(FLOORBOSS[state.floor],brx,bry);
  }
  state.spawnT-=dt;
  if(state.spawnT<=0){
    state.spawnT=1.4f;
    if(state.floor==0) spawnEnemySurface();
    else if(state.sealOpen && !bossOn) spawnEnemyFloor();
  }
}
