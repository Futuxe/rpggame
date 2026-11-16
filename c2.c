/* ---------- dungeon generation: rooms, arena, Ember Seal, hidden vaults ---------- */
static bool roomsOverlap(int x,int y,int w,int h){
  for(int i=0;i<nRooms;i++){
    Room *r=&rooms[i];
    if(x < r->x+r->w+3 && x+w+3 > r->x && y < r->y+r->h+3 && y+h+3 > r->y) return true;
  }
  return false;
}
static bool vaultClear(int x,int y,int w,int h){
  for(int i=0;i<nRooms;i++){
    Room *r=&rooms[i];
    if(x < r->x+r->w+4 && x+w+4 > r->x && y < r->y+r->h+4 && y+h+4 > r->y) return false;
  }
  for(int i=0;i<nVaults;i++){
    VaultDef *v=&vaults[i];
    if(x < v->x+v->w+4 && x+w+4 > v->x && y < v->y+v->h+4 && y+h+4 > v->y) return false;
  }
  return true;
}
static void carve(int tx,int ty){ if(tx>0&&ty>0&&tx<mapW-1&&ty<mapH-1) setTile(tx,ty,T_FLOOR); }

static void genDungeon(int fl){
  mapW=160+fl*40; mapH=160+fl*40;
  resetWorldArrays();
  memset(tiles,T_WALL,mapW*mapH);
  nRooms=0; nVaults=0; nSealTiles=0; sealOpen=true;
  int want=14+fl*4, tries=0;
  while(nRooms<want && tries++<1200){
    int w=7+rand()%8, h=6+rand()%7;
    int x=2+rand()%(mapW-w-4), y=2+rand()%(mapH-h-4);
    if(roomsOverlap(x,y,w,h)) continue;
    rooms[nRooms++] = (Room){ x,y,w,h,x+w/2,y+h/2 };
  }
  for(int i=1;i<nRooms;i++){
    Room *a=&rooms[i-1], *b=&rooms[i];
    int x=a->cx, y=a->cy;
    while(x!=b->cx){ carve(x,y); carve(x,y+1); x += x<b->cx?1:-1; }
    while(y!=b->cy){ carve(x,y); carve(x+1,y); y += y<b->cy?1:-1; }
  }
  for(int i=0;i<nRooms;i++){
    Room *r=&rooms[i];
    for(int dy=0;dy<r->h;dy++)for(int dx=0;dx<r->w;dx++) carve(r->x+dx,r->y+dy);
  }
  /* boss arena on every floor */
  int bw = fl==5?26:20, bh = fl==5?17:13;
  int bx=(mapW-bw)/2, by=(mapH-bh)/2;
  for(int t=0;t<160;t++){
    int nx=2+rand()%(mapW-bw-4), ny=2+rand()%(mapH-bh-4);
    if(!roomsOverlap(nx-2,ny-2,bw+4,bh+4)){ bx=nx; by=ny; break; }
  }
  bossRoom = (Room){ bx,by,bw,bh,bx+bw/2,by+bh/2 };
  for(int dy=0;dy<bh;dy++)for(int dx=0;dx<bw;dx++) carve(bx+dx,by+dy);
  if(fl>=2){ /* magma ring around the arena */
    for(int dx=0;dx<bw;dx++){ setTile(bx+dx,by,T_LAVA); setTile(bx+dx,by+bh-1,T_LAVA); }
    for(int dy=0;dy<bh;dy++){ setTile(bx,by+dy,T_LAVA); setTile(bx+bw-1,by+dy,T_LAVA); }
  }
  Room nr=rooms[0]; float nd=1e9f;
  for(int i=0;i<nRooms;i++){
    float d=distf((float)rooms[i].cx,(float)rooms[i].cy,(float)bossRoom.cx,(float)bossRoom.cy);
    if(d<nd){ nd=d; nr=rooms[i]; }
  }
  int x=nr.cx, y=nr.cy;
  while(x!=bossRoom.cx){ carve(x,y); carve(x,y+1); x += x<bossRoom.cx?1:-1; }
  while(y!=bossRoom.cy){ carve(x,y); carve(x+1,y); y += y<bossRoom.cy?1:-1; }
  rooms[nRooms++] = bossRoom;

  /* Ember Seal: wall ring around the arena until every normal foe is dead */
  if(!(state.bossDead[fl])){
    for(int dy=-1;dy<=bh;dy++)for(int dx=-1;dx<=bw;dx++){
      bool edge = dx<0||dy<0||dx>=bw||dy>=bh;
      if(!edge) continue;
      int tx=bx+dx, ty=by+dy;
      if(tileAt(tx,ty)==T_FLOOR){
        setTile(tx,ty,T_WALL);
        if(nSealTiles<MAXSEAL) state.sealTiles[nSealTiles++]=ty*MAXMAP+tx;
      }
    }
  }
  state.nSealTiles=nSealTiles;
  state.sealOpen = nSealTiles==0;
  sealOpen = state.sealOpen;

  /* --- hidden vaults: sealed treasure rooms behind a cracked wall --- */
  int nv = fl>=4?2:1;
  for(int vi=0;vi<nv && nVaults<MAXVAULTS;vi++){
    int vw=9, vh=7, vx=-1, vy=-1;
    for(int t=0;t<700;t++){
      int nx=4+rand()%(mapW-vw-8), ny=4+rand()%(mapH-vh-8);
      if(vaultClear(nx,ny,vw,vh)){ vx=nx; vy=ny; break; }
    }
    if(vx<0) continue;
    for(int dy=0;dy<vh;dy++)for(int dx=0;dx<vw;dx++) carve(vx+dx,vy+dy);
    int cx=vx+(vw>>1), cy=vy+(vh>>1);
    Room *src=&rooms[0]; float best=1e9f;
    for(int i=0;i<nRooms;i++){
      if(rooms[i].cx==bossRoom.cx && rooms[i].cy==bossRoom.cy) continue;
      float d=distf((float)rooms[i].cx,(float)rooms[i].cy,(float)cx,(float)cy);
      if(d<best){ best=d; src=&rooms[i]; }
    }
    int x2=src->cx, y2=src->cy;
    while(x2!=cx){ carve(x2,y2); carve(x2,y2+1); x2 += x2<cx?1:-1; }
    while(y2!=cy){ carve(x2,y2); carve(x2+1,y2); y2 += y2<cy?1:-1; }
    /* seal every perimeter opening except the one closest to the source room,
       which becomes a cracked T_SECRET tile */
    int perX[64], perY[64], np=0;
    for(int dx=-1;dx<=vw;dx++){ if(np<64){perX[np]=vx+dx;perY[np++]=vy-1;} if(np<64){perX[np]=vx+dx;perY[np++]=vy+vh;} }
    for(int dy=0;dy<vh;dy++){ if(np<64){perX[np]=vx-1;perY[np++]=vy+dy;} if(np<64){perX[np]=vx+vw;perY[np++]=vy+dy;} }
    int doorX=-1,doorY=-1; float dbest=1e9f;
    for(int i=0;i<np;i++){
      if(tileAt(perX[i],perY[i])==T_WALL) continue;
      float d=distf((float)perX[i],(float)perY[i],(float)src->cx,(float)src->cy);
      if(d<dbest){ dbest=d; doorX=perX[i]; doorY=perY[i]; }
    }
    for(int i=0;i<np;i++){
      if(tileAt(perX[i],perY[i])==T_WALL) continue;
      setTile(perX[i],perY[i], (perX[i]==doorX&&perY[i]==doorY)?T_SECRET:T_WALL);
    }
    if(doorX<0) setTile(cx,vy-1,T_SECRET);
    int id=fl*10000+cx*100+cy;
    vaults[nVaults++] = (VaultDef){ vx,vy,vw,vh,cx,cy,false,id };
    if(nChests<MAXCHESTS)
      chests[nChests++] = (Chest){ cx*TS+TS/2.0f, cy*TS+TS/2.0f,
        keyIn(openedChestKeys,nOpened,id), id, true };
  }

  /* variation + lava list */
  nLavas=0;
  for(int i=0;i<mapW*mapH;i++){
    tileVar[i]=hash2((unsigned)i*137u,(unsigned)i*71u);
    if(tiles[i]==T_LAVA && nLavas<MAXLAVA){
      lavas[nLavas][0]=(i%mapW)*TS+TS/2;
      lavas[nLavas][1]=((i/mapW))*TS+TS/2;
      nLavas++;
    }
  }
  if(fl>=2){
    for(int i=0;i<fl;i++){
      int ri=1+rand()%(nRooms-1);
      Room *r=&rooms[ri];
      if(r->cx==bossRoom.cx && r->cy==bossRoom.cy) continue;
      int lx=r->x+2+rand()%(r->w-4>0?r->w-4:1), ly=r->y+2+rand()%(r->h-4>0?r->h-4:1);
      for(int dy=-1;dy<=1;dy++)for(int dx=-1;dx<=1;dx++)
        if(hash2((unsigned)(lx+dx)*3u,(unsigned)(ly+dy)*5u)>0.3f && tileAt(lx+dx,ly+dy)==T_FLOOR){
          setTile(lx+dx,ly+dy,T_LAVA);
          if(nLavas<MAXLAVA){ lavas[nLavas][0]=(lx+dx)*TS+TS/2; lavas[nLavas][1]=(ly+dy)*TS+TS/2; nLavas++; }
        }
    }
  }
  /* stairs */
  Room *start=&rooms[0];
  setTile(start->cx,start->cy,T_STAIRSUP);
  dstartX=start->cx*TS+TS/2.0f+TS; dstartY=start->cy*TS+TS/2.0f;
  int downCx=-1, downCy=-1;
  if(fl<MAXFLOOR && nRooms>2){
    int ri=1+rand()%(nRooms-1);
    while(rooms[ri].cx==bossRoom.cx && rooms[ri].cy==bossRoom.cy) ri=1+rand()%(nRooms-1);
    downCx=rooms[ri].cx; downCy=rooms[ri].cy;
    setTile(downCx,downCy,T_STAIRSDOWN);
  }
  /* shrines */
  nShrines=0;
  int nShr = fl>=3?2:1;
  for(int i=0;i<nShr && nRooms>3 && nShrines<MAXSHRINES;i++){
    int ri=1+rand()%(nRooms-1);
    Room *r=&rooms[ri];
    if(r->cx==bossRoom.cx && r->cy==bossRoom.cy) continue;
    if(r->cx==downCx && r->cy==downCy) continue;
    if(r->cx==start->cx && r->cy==start->cy) continue;
    int id=fl*10000 + r->cx*100 + r->cy;
    Shrine s={ r->cx*TS+TS/2.0f+rndf(-24,24), r->cy*TS+TS/2.0f+rndf(-16,16), fl, false, keyIn(doneShrineKeys,nDone,id), 0, id };
    shrines[nShrines++]=s;
  }
  /* chests */
  for(int i=0;i<4+fl*2 && nRooms>2 && nChests<MAXCHESTS;i++){
    int ri=1+rand()%(nRooms-1);
    Room *r=&rooms[ri];
    if(r->cx==bossRoom.cx && r->cy==bossRoom.cy) continue;
    int id=fl*10000 + r->cx*100 + r->cy;
    bool dup=false;
    for(int j=0;j<nChests;j++) if(chests[j].id==id) dup=true;
    if(dup) continue;
    chests[nChests++] = (Chest){ r->cx*TS+TS/2.0f+rndf(-22,22), r->cy*TS+TS/2.0f+rndf(-14,14), keyIn(openedChestKeys,nOpened,id), id, false };
  }
  /* torches on wall faces that look onto floor */
  for(int ty=1;ty<mapH-1;ty++)for(int tx=1;tx<mapW-1;tx++){
    if(tileAt(tx,ty)!=T_WALL) continue;
    int dirs[4][2]={{1,0},{-1,0},{0,1},{0,-1}};
    for(int d=0;d<4;d++){
      if(tileAt(tx+dirs[d][0],ty+dirs[d][1])==T_FLOOR && hash2((unsigned)tx*2u,(unsigned)ty*3u)<0.075f && nTorches<MAXTORCHES){
        Torch t={ tx*TS+TS/2.0f+dirs[d][0]*15, ty*TS+TS/2.0f+dirs[d][1]*15,
                  dirs[d][0], dirs[d][1], rnd0(TAU) };
        torches[nTorches++]=t;
        break;
      }
    }
  }
  buildTreeGrid();
}

/* ---------- collision ---------- */
static bool circleFree(float x,float y,float r){
  int minTx=(int)floorf((x-r)/TS), maxTx=(int)floorf((x+r)/TS);
  int minTy=(int)floorf((y-r)/TS), maxTy=(int)floorf((y+r)/TS);
  for(int ty=minTy;ty<=maxTy;ty++)for(int tx=minTx;tx<=maxTx;tx++){
    if(!solidTile(tileAt(tx,ty))) continue;
    float cx=clampf(x,(float)(tx*TS),(float)((tx+1)*TS));
    float cy=clampf(y,(float)(ty*TS),(float)((ty+1)*TS));
    float dx=cx-x, dy=cy-y;
    if(dx*dx+dy*dy < r*r) return false;
  }
  if(state.floor==0){
    int gr=(int)ceilf(r/TS)+1;
    int tx0=(int)floorf(x/TS), ty0=(int)floorf(y/TS);
    for(int dy=-gr;dy<=gr;dy++)for(int dx=-gr;dx<=gr;dx++){
      int tx=tx0+dx, ty=ty0+dy;
      if(tx<0||ty<0||tx>=mapW||ty>=mapH) continue;
      for(int ti=treeHead[ty*mapW+tx]; ti>=0; ti=treeNext[ti]){
        Tree *t=&trees[ti];
        float ddx=t->x-x, ddy=t->y-y, rr=r+t->cr;
        if(ddx*ddx+ddy*ddy<rr*rr) return false;
      }
    }
  }
  for(int i=0;i<nBuildings;i++){
    Building *b=&buildings[i];
    float cx=clampf(x,(float)b->x,(float)(b->x+b->w));
    float cy=clampf(y,(float)b->y,(float)(b->y+b->h));
    float dx=cx-x, dy=cy-y;
    if(dx*dx+dy*dy < r*r) return false;
  }
  for(int i=0;i<nRocks;i++){
    float dx=rocks[i].x-x, dy=rocks[i].y-y, rr=r+rocks[i].r*0.8f;
    if(dx*dx+dy*dy<rr*rr) return false;
  }
  if(state.floor==0) for(int i=0;i<nNpcs;i++){
    float dx=npcs[i].x-x, dy=npcs[i].y-y, rr=r+npcs[i].r*0.8f;
    if(dx*dx+dy*dy<rr*rr) return false;
  }
  for(int i=0;i<nChests;i++){
    if(chests[i].opened) continue;
    float dx=chests[i].x-x, dy=chests[i].y-y, rr=r+10;
    if(dx*dx+dy*dy<rr*rr) return false;
  }
  for(int i=0;i<nShrines;i++){
    float dx=shrines[i].x-x, dy=shrines[i].y-y, rr=r+10;
    if(dx*dx+dy*dy<rr*rr) return false;
  }
  return true;
}
/* slide-and-substep movement: knockback can never shove an entity into stone */
static void moveEnt(float *x,float *y,float r,float dx,float dy){
  float steps=fmaxf(1.0f,ceilf(fmaxf(fabsf(dx),fabsf(dy))/4.0f));
  for(int i=0;i<(int)steps;i++){
    float sx=dx/steps, sy=dy/steps;
    if(circleFree(*x+sx,*y+sy,r)){ *x+=sx; *y+=sy; continue; }
    if(circleFree(*x+sx,*y,r)) *x+=sx;
    else if(circleFree(*x,*y+sy,r)) *y+=sy;
  }
}
static void avoidLavaEnt(Enemy *e,float dx,float dy){
  if(e->flies || tileAtPx(e->x+dx,e->y+dy)!=T_LAVA){ moveEnt(&e->x,&e->y,e->r,dx,dy); return; }
  if(dx!=0 && tileAtPx(e->x+dx,e->y)!=T_LAVA) moveEnt(&e->x,&e->y,e->r,dx,0);
  else if(dy!=0 && tileAtPx(e->x,e->y+dy)!=T_LAVA) moveEnt(&e->x,&e->y,e->r,0,dy);
}
static bool findFreeSpot(float x,float y,float r,float *ox,float *oy){
  if(circleFree(x,y,r)){ *ox=x; *oy=y; return true; }
  for(int d=1;d<40;d++){
    for(int i=0;i<8;i++){
      float a=i/8.0f*TAU;
      float nx=x+cosf(a)*d*10, ny=y+sinf(a)*d*10;
      if(circleFree(nx,ny,r)){ *ox=nx; *oy=ny; return true; }
    }
  }
  *ox=x; *oy=y; return false;
}
