//_|___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|
//___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|__
//_|___|___|_  B R I C K   B R E A K E R   R E V E N G E   |___|___|___|
//___|___|___|___|___|__     SANTA DEMO 2018         |___|___|___|___|__
//_|___|___|___|___|___|___ f o r   M S  X  2 _|___|___|___|___|___|___|
//___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|__
//_|___|___|___|___|        BZ PROD GAME STUDIO         ___|___|___|___|
//___|___|___|___|___|___|_    REPRO FACTORY    _|___|___|___|___|___|__
//_|___|___|___|___|___|___|__               __|___|___|___|___|___|___|
//___|___|___|___|___|___|__Eric Boez (c) 2018___|___|___|___|___|___|__
//_|___|___|___|___|___|___|___|           _|___|___|___|___|___|___|___|
//___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|__
//_|___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|
//___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|__
//_|___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|
//___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|__
//_|___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|
//___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|__
//                 M a d e   w i t h     F U S I O N - C   (adapted to Fusion-C 1.2)                    
//                        The C Library For MSX
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fusion-c/header/msx_fusion.h"
#include "fusion-c/header/vdp_sprites.h"
#include "fusion-c/header/vdp_graph2.h"
#include "fusion-c/header/ayfx_player.h"
#include "fusion-c/header/pt3replayer.h"


static FCB file;                        // Used by MSXDOS loading routines
MMMtask  t;

// Global variables definitions
//
unsigned char playfield[24][16];
unsigned char song[2305];  
unsigned char LDbuffer[2400];

int x;
int y;
int dir_x;
int dir_y;
int div_x;
int div_y;
int px;
int py;
int brx;
int bry;
int nb_bricks;
char joy;
char trig;
char border;
char verif_flag;
char mvx;
int cpt;
int ball_cpt;
char anim;
char animtab[4];
int anim_speed;
int anim_dir;
int sens;
char santa;
char santawin;
char score;
char level;
char life;
char gameOn;

// Functions prototypes
void  FT_ScreenCopy(char x1, char y1, char dx, char dy, char x2, char y2, char src , char dst, char mode);
void  FT_SetName( FCB *p_fcb, const char *p_name );
void  FT_errorHandler(char n, char *name);
int   FT_Load Image(char *file_name, unsigned int start_Y, char *buffer);
int   FT_LoadData(char *file_name, char *buffer, int size, int skip);
char  FT_wait(int cicles);
int   FT_end(void);
void  FT_musicWait(void);
void  FT_verif(int x, int y, int point);
void  FT_Srpites(void);
void  FT_Joy();
void  FT_CheckFX (void);
void  FT_CalcPosition (void);
void  FT_GameInit(void);
void  FT_EmptyField(void);
void  FT_PutBricks(void);
void  FT_WinLevel(char nlevel);
void  drawScreen(void);
void  FT_ShowBricksWall(void);
void  FT_FirstScreen(void);
void  FT_lastScreen(void);

/* ---------------------------------
          FT_ScreenCopy

   Copy a screen zone to another place
   x1 & y1 =Top left coordonate  pixel of the zone to copy
   dx = Width Size in pixels, of the zone to copy
   dy = Height Size in pixels, of the zone to copy
   x2 & y2 = Destination coordonate where to copy the zone
   src = Source Page number of the Zone
   dst = Destination number of the zone
   mode = OP mode of the copy
-----------------------------------*/ 
void FT_ScreenCopy(char x1, char y1, char dx, char dy, char x2, char y2, char src , char dst, char mode)  
{
    int src_y=0;
    int dst_y=0;

    if (src==1)
        src_y=256;
    if (dst==1)
        dst_y=256;
    
    t.X = x1; 
    t.Y = src_y+y1; 
    t.DX = dx;  
    t.DY = dy;     
    t.X2 = x2; 
    t.Y2 = dst_y+y2;           
    t.s0 = 0; 
    t.DI = 0; 
    t.LOP = mode;
    fLMMM(&t);
}


/* ---------------------------------
                FT_SetName

    Set the name of a file to load
                (MSX DOS)
-----------------------------------*/ 
void FT_SetName( FCB *p_fcb, const char *p_name ) 
{
  char i, j;
  memset( p_fcb, 0, sizeof(FCB) );
  for( i = 0; i < 11; i++ ) {
    p_fcb->name[i] = ' ';
  }
  for( i = 0; (i < 8) && (p_name[i] != 0) && (p_name[i] != '.'); i++ ) {
    p_fcb->name[i] =  p_name[i];
  }
  if( p_name[i] == '.' ) {
    i++;
    for( j = 0; (j < 3) && (p_name[i + j] != 0) && (p_name[i + j] != '.'); j++ ) {
      p_fcb->ext[j] =  p_name[i + j] ;
    }
  }
}

/* ---------------------------------
            FT_errorHandler

          In case of Error
-----------------------------------*/ 
void FT_errorHandler(char n, char *name)
{
  InitPSG();
  Screen(0);
  SetColors(15,6,6);
  switch (n)
  {
      case 1:
          Print("\n\rFAILED: fcb_open(): ");
          Print(name);
      break;

      case 2:
          Print("\n\rFAILED: fcb_close():");
          Print(name);
      break;  

      case 3:
          Print("\n\rStop Kidding, run me on MSX2 !");
      break; 
  }
Exit(0);
}

/* ---------------------------------
          FT_LoadSc8Image

    Load a SC8 Picture and put datas
  on screen, begining at start_Y line
-----------------------------------*/ 
int FT_LoadSc8Image(char *file_name, unsigned int start_Y, char *buffer)
    {
        int rd=2304;
        int nbl=0;
        InitPSG();
        FT_SetName( &file, file_name );
        if(fcb_open( &file ) != FCB_SUCCESS) 
        {
          FT_errorHandler(1, file_name);
          return (0);
        }
        fcb_read( &file, buffer, 7 );  // Skip 7 first bytes of the file
        while(rd!=0)
        {
            SetColors(15,0,buffer[192]);
            rd = fcb_read( &file, buffer, 2304 );
                if (rd!=0)
                {
                  SetColors(15,0,0);
                  nbl=rd/256;
                  HMMC(buffer, 0,start_Y,256,nbl ); // Move the buffer to VRAM. 17 lines x 256 pixels  from memory
                  start_Y=start_Y+nbl; 
              }
        }
      if( fcb_close( &file ) != FCB_SUCCESS ) 
      {
         FT_errorHandler(2, file_name);
          return (0);
      }

return(1);
}


/* ---------------------------------
          FT_LoadData
  Load Data to a specific pointer
  size is the size of data to read
  skip represent the number of Bytes
  you want to skip from the begining of the file
  Example: skip=7 to skip 7 bytes of a MSX bin
-----------------------------------*/ 
int FT_LoadData(char *file_name, char *buffer, int size, int skip)
{
    InitPSG();
    FT_SetName( &file, file_name );
    if(fcb_open( &file ) != FCB_SUCCESS) 
    {
          FT_errorHandler(1, file_name);
          return (0);
    }
    if (skip>0)
    {
        fcb_read( &file, buffer, skip );
    }
    SetColors(15,0,80);
    
    fcb_read( &file, buffer, size );
    
    if( fcb_close( &file ) != FCB_SUCCESS ) 
    {
      FT_errorHandler(2, file_name);
      return (0);
    }
    SetColors(16,0,0);
    return(0);
}

// Wait some times
//
char FT_wait(int cicles)
{
  int i;
  for(i=0;i<cicles;i++) 
  {
    EnableInterupt()
    Halt();
  }
  return(0);
}

// Ending Game
//
int FT_end(void)
{
    KillKeyBuffer();
    PutText((256-(8*17))/2,150,"G A M E   O V E R",0);
    //Beep();
    WaitKey();
    Cls();
    FT_GameInit();
    gameOn=1;
    return(0);
}
// Play music while waiting pressing Space Barre
//
void FT_musicWait(void)
{
  while (Inkey()!=32)
  {
    Halt(); 
    DisableInterupt();
    PT3Rout();
    PT3Play();
    EnableInterupt();
  }
  InitPSG();
}

// Verifiation if the ball hurting a brick
//
void FT_verif(int x, int y, int point)
{
  int fx,fy;
  char Soundfx;
  fx=x/16;
  fy=y/8;

      
      if (playfield[fy][fx]!=0)
        {
          verif_flag=1;
         
          if (point==1 || point==3)
          {
            dir_y *= -1;
          }
          if (point==2 || point==4)
          {
            dir_x *= -1;
          }
          if (playfield[fy][fx]<=8)
          {
            playfield[fy][fx]=0;
          }
          else
          {
            playfield[fy][fx]--;
          }
          if (playfield[fy][fx]==0)
          {
              brx=fx;
              bry=fy;
              Soundfx=12;
          }
          else
          {
             Soundfx=19;
          }
     
        PlayFX(Soundfx);
        }
}

// Show Sprites on screen, and do graphic job
//
void FT_Srpites(void)
{ 
         PutSprite(0,1,x,y,10);           // Ball Sprite
         PutSprite(1,0,px,py,10);         // The Paddle is composed of 3 sprites
         PutSprite(2,0,px+8,py,10);
         PutSprite(3,0,px+16,py,10);
    
          cpt--;
          if (cpt==0) //  Santa Walking
          {
               FT_ScreenCopy(animtab[anim], 88+32*anim_dir, 24, 32, mvx, 0, 1 , 0, 0);
                cpt=anim_speed;
                if (santawin==0)
                {
                    mvx=mvx+sens;
                    if (mvx<4)
                    {
                      anim_dir=1;
                      sens=4;
                    }
                    if (mvx>250)
                    {
                      anim_dir=0;
                      sens=-4;
                    }
                    anim++;
                    if (anim>3)
                    {
                       anim=0;
                    }
                }
          }

         if (brx!=255 && bry!=255)            // If there is a brick to remove ? it does the job
         {
            FT_ScreenCopy(240, 32, 16, 8, brx*16, bry*8, 1 , 0, 0);  
            brx=255;
            bry=255;
         }


         if (santawin==1)         // Santa was touched
         {
            santawin=0;
            score++;
            FT_ScreenCopy(128, 118, 16, 16, 80+(score*16), 196, 1 , 0, 0); // Pain d'epice
            PlayFX(7);
            if (score==10)
            {
              FT_wait(60);
              FT_WinLevel(level);
            }
         }
}   

// Joystick control function
//
void FT_Joy()
{
  joy=JoystickRead(0);
  trig=TriggerRead(0);

  // FT_Srpites();
/*
    if (trig!=0)
    {
      InitPSG();
      Screen(0);
      Exit(0);
    }
*/
    switch (joy)
    {
        case 1:
          VDP60Hz();
        break;
        
        case 3:
        if (px<231)
        {
            px++;
        }
        break;
        
        case 5:
          VDP50Hz();
        break;
        
        case 7:
        if (px>0)
        {
          px--;
        }
        break;
    }
}

// Calcul position of the ball
//
void FT_CalcPosition (void)
{
  int touch,i;
  ball_cpt--;
  if(ball_cpt==0)
  {
     ball_cpt=2;
    // If the Ball is hurting left or right border of the screen
    if (x>=246 || x<=3)
    {
        dir_x *= -1;
    }
    
    // If the Ball is hurting up or bottom border of the screen
    if (y<=1)
    {
        dir_y *= -1;
    }
    if (y>190)        /// Loosing a life
    {
       PlayFX(10);
       life--;
       for(i=0;i<90;i++)
      {
        FT_wait(1);
        FT_CheckFX();
       }
       SpriteOff();
       if (life<=0)
       {
          gameOn=0;
          FT_end();
       }
        x=170;
        y=110;
        dir_x=1;
        dir_y=-1;
        div_x=1;
        div_y=1;
        px=130;
        py=185;
        brx=255;
        bry=255;
        gameOn=1;
       FT_ShowBricksWall();
    }
    
    // If the ball is entering the screen zone where Bricks are supposed to be (5 brick lines, with 8 pixels height : 5x8=40 )
    if (y<12*8 && y>32)            
    {
        verif_flag=0;                   // This flag is at 1 if a brick is touched. It disable other testing in case of the ball touch a brick
        santa=0;
        if (verif_flag==0)  
        {
            FT_verif(x+4, y-1,1);           // Testing Ball point 1
        }

        if (verif_flag==0)
        {
            FT_verif(x+9, y+4,2);         // testing Ball point 2
        }
       
        if (verif_flag==0)
        {
            FT_verif(x+4, y+9,3);         // Testing Ball point 3
        }
       
        if (verif_flag==0)
        {
            FT_verif(x-1, y+4, 4);        // testing Ball point 4
        }
    }
      verif_flag=0;

    // If Ball touching santa ?
    if (y<=32 && x>=mvx && x<=mvx+16 && santa==0)
    {
      santa=1;
      santawin=1;
    }
    if ((y<=32 && x<mvx && santa==1) || (y<=32 && x>mvx+16 && santa==1) )
    {
      santa=0;
    }
  

    // If the BAll enter the zone where the paddle is
    if ((y>=180 && y<=185) && (x>px-4 && x<px+24) && dir_y>0)
    {
        touch=x-px;                     // Are the Ball and paddle in the same X Zone ?
        santa=0;
        // If ball is touching paddle on its extremity
        if ((touch>-10 && touch <=4) || (touch>20 && touch<=28))
        {
            dir_x*=-1;
            div_x=2;
        }
        // If Ball is touching the paddle on its center
        if ((touch>3 && touch <=21))
        {     
           div_x=1;
        }
         
        dir_y*= -1;                   // Reversing Y ball direction
        y=y-2;
        SetColors(255,0,border);
        PlayFX(5);
      }
    
      SetColors(255,0,border);
   
      x+=dir_x;
      y+=dir_y;
     
 }
}
// New game init
//
void FT_GameInit(void)
{
  x=170;
  y=110;
  dir_x=1;
  dir_y=-1;
  div_x=1;
  div_y=1;
  px=130;
  py=185;
  brx=255;
  bry=255;
  verif_flag=0;
  score=0;
  life=5;
  FT_PutBricks();
  FT_ShowBricksWall();
  SpriteOn();
  gameOn=1;
}

// Fill the main array with empty 
//
void FT_EmptyField(void)
{
  int j,i;

   for(j=0;j<24;j++)
   {
      for(i=0;i<16;i++)
      {
        playfield[j][i]=0;
      }
  }

}


// Put some Bricks inthe Array
//
void FT_PutBricks(void)
{
  int j,i;
  unsigned char level1[8][16]={
                          {2,2,2,2,2,2,1,1,1,1,2,2,2,2,2,2},
                          {2,2,2,1,1,1,5,5,5,5,1,1,1,2,2,2},
                          {2,2,2,1,1,3,5,5,5,5,3,1,1,2,2,2},
                          {2,2,2,1,1,3,3,3,3,3,3,1,1,2,2,2},
                          {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                          {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                          {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                          {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
                        };
                        
  unsigned char level2[8][16]={
                          {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
                          {5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5},
                          {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
                          {2,2,2,1,1,3,3,3,3,3,3,1,1,2,2,2},
                          {3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3},
                          {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                          {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                          {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
                        };
   unsigned char level3[8][16]={
                          {0,0,0,0,0,0,0,4,4,4,4,0,0,0,0,0},
                          {4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4},
                          {1,1,3,3,2,2,2,1,1,2,2,2,3,3,1,1},
                          {1,1,1,3,2,2,2,1,1,2,2,2,3,1,1,1},
                          {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
                          {0,9,0,0,0,0,0,0,0,0,0,0,0,0,9,0},
                          {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                          {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
                        };
    unsigned char level4[8][16]={
                          {5,3,5,3,5,3,5,3,5,3,5,3,5,3,5,3},
                          {3,5,3,5,3,5,3,5,3,5,3,5,3,5,3,5},
                          {5,3,5,3,5,3,5,3,5,3,5,3,5,3,5,3},
                          {3,5,3,5,3,5,3,5,3,5,3,5,3,5,3,5},
                          {5,9,5,5,5,5,5,5,5,5,5,5,5,5,9,5},
                          {5,9,5,5,5,5,5,5,5,5,5,5,5,5,9,5},
                          {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                          {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
                        };
    unsigned char level5[8][16]={
                          {4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4},
                          {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                          {9,9,9,9,9,9,9,2,2,9,9,9,9,9,9,9},
                          {4,4,4,4,4,4,4,3,3,4,4,4,4,4,4,4},
                          {4,4,4,4,4,4,4,3,3,4,4,4,4,4,4,4},
                          {4,4,4,4,4,4,4,2,2,4,4,4,4,4,4,4},
                          {4,4,4,4,4,4,2,2,2,2,4,4,4,4,4,4},
                          {4,4,4,4,4,4,2,2,2,2,4,4,4,4,4,4},
                        };

  // Fill The main array with some bricks
  for(j=0;j<8;j++)
   {
      for(i=0;i<16;i++)
      {
        switch (level)
        {
          case 1:
          playfield[j+4][i]=level1[j][i];
          break;

          case 2:
          playfield[j+4][i]=level2[j][i];
          break;

          case 3:
          playfield[j+4][i]=level3[j][i];
          break;

          case 4:
          playfield[j+4][i]=level4[j][i];
          break;

          case 5:
          playfield[j+4][i]=level5[j][i];
          break;

          default:
          playfield[j+4][i]=level1[j][i];
          break;
        }
      }
  }
}

// Level Win show gift
//
void FT_WinLevel(char nlevel)
{
  char *name="strip1.dat";
  char *number="0123456789";
  char *part="part 0/5 of your";
  int i;
  gameOn=0;
  InitPSG();
  SpriteOff();
  KillKeyBuffer();
  Cls();

  part[5]=number[nlevel];
  name[5]=number[nlevel];
  if (level<5)
  {
    PutText((256-(8*24))/2,80,"Prepare yourself for the",0);
    PutText((256-(8*16))/2,90,part,0);
    PutText((256-(8*14))/2,100,"Christmas gift",0);
  }
  else
  {
    PutText((256-(8*12))/2,90,"And now your",0);
    PutText((256-(8*14))/2,100,"CHRISMAST GIFT",0);
  }
  PlayFX(6);
  for(i=0;i<200;i++)
  {
    FT_wait(1);
    FT_CheckFX();
  }
  SetColors(255,255,255);
  Cls();

  FT_SetName( &file, name);
  fcb_open( &file );

  fcb_read( &file, LDbuffer, 7 );

  for(i=0;i<212;i++)
  {
    fcb_read( &file, LDbuffer, 151 );
    HMMC(&LDbuffer, 53,i ,151,1 );
    Halt(); 
    DisableInterupt();
    PT3Rout();
    PT3Play();
    EnableInterupt();
    
  }
  fcb_close( &file );

  FT_musicWait();

  SetColors(255,0,border);
  Cls();
  if (level==5)
  {
    FT_lastScreen();
  }

  level++;
  FT_GameInit();
}


// printing  life and level numbers
//
void drawScreen(void)
{
   FT_ScreenCopy(144,88, 16, 8, 0, 202, 1 , 0, 0); // Life
   FT_ScreenCopy(144,136-life*8, 16, 8, 18, 202, 1 , 0, 0);
   FT_ScreenCopy(160,88, 20, 8, 32, 202, 1 , 0, 0); // Level
   FT_ScreenCopy(144,136-level*8, 16, 8, 55, 202, 1 , 0, 0);

}

// Showing Bricks Wall on screen
//
void FT_ShowBricksWall(void)
{
  int j,i;
  int brx,bry;
  i=0;
  j=0;

  for(j=0;j<24;j++)
  {
    for(i=0;i<16;i++)
    {
      if (playfield[j][i]>=1)
      {
          switch(playfield[j][i])
          {
            case 1:      // jaune
            brx=96;
            bry=8;
            break;

            case 2:     // Rouge
            brx=0;
            bry=8;
            break;

            case 3:     // vert
            brx=128;
            bry=8;
            break;

            case 4:     // Bleu
            brx=192;
            bry=8;
            break;

            case 5:     // Orange
            brx=32;
            bry=8;
            break;

            case 9:     // Glass
            brx=160;
            bry=32;
            break;

            default:
            brx=0;
            bry=0;
            break;
          }
            FT_ScreenCopy(brx,bry, 16, 8, i*16, j*8, 1 , 0, 0);
            nb_bricks++;
      }
    }
  }
  drawScreen();
  PutText((256-(8*11))/2,150,"GET READY !",0);
  PlayFX(6);
  for(i=0;i<100;i++)
  {
    FT_wait(1);
    FT_CheckFX();
  }
  SpriteOn();
  PutText((256-(8*11))/2,150,"           ",0);
}

void FT_lastScreen(void)
{

  SetColors(200,0,0);
  PutText((256-(8*26))/2,15,"BRICK BREAKER - SANTA DEMO",0);
  SetColors(255,0,0);
  PutText((256-(8*11))/2,30,"Well Done !",0);
  PutText((256-(8*21))/2,40,"Thank you for playing",0);

  PutText((256-(8*15))/2,100,"MERRY CHRISTMAS",0);
  //PutText((256-(8*11))/2,100,"JOYEUX NOEL");
  PutText((256-(8*23))/2,110,"AND HAPPY NEW YEAR 2019",0);
  //PutText((256-(8*19))/2,110,"ET BONNE ANNEE 2019");
  SetColors(200,0,0);
  PutText((256-(8*32))/2,180,"Take a look to FUSION-C Library!",0);
   
  FT_musicWait();

}

void FT_FirstScreen(void)
{

  SetColors(200,0,0); 
  PutText((256-(8*26))/2,15,"BRICK BREAKER - SANTA DEMO",0);
  SetColors(255,0,0);
  PutText((256-(8*18))/2,30,"Made with Fusion-C",0);
  //PutText((256-(8*18))/2,30,"Fait avec Fusion-C");
  PutText((256-(8*24))/2,40,"The MSX Library for SDCC",0);
  //PutText((256-(8*25))/2,40,"La Librarie MSX pour SDCC");
  PutText((256-(8*16))/2,50,"Eric Boez - 2018",0);
  PutText((256-(8*21))/2,60,"For www.msxvillage.fr",0);
  //PutText((256-(8*22))/2,60,"pour www.msxvillage.fr");

  PutText((256-(8*25))/2,100,"YOUR CHRISTMAS GIFT IS AT",0);
  //PutText((256-(8*27))/2,100,"VOTRE CADEAU DE NOEL EST AU");
  PutText((256-(8*18))/2,110,"THE END OF LEVEL 5",0);
  //PutText((256-(8*25))/2,110,"BOUT DU NIVEAU 5, TOUCHEZ");
  PutText((256-(8*29))/2,120,"10 SANTA SHOOTS TO PASS LEVEL",0);
  //PutText((256-(8*25))/2,120,"10 FOIS LE PERE NOEL POUR");
  PutText((256-(8*11))/2,140,"GOOD LUCK",0);
  //PutText((256-(8*18))/2,140,"PASSER UN NIVEAU !");

  PutText((256-(8*22))/2,160,"Use Keyboard <- and ->",0);
  //PutText((256-(8*29))/2,160,"Utilisez les touches <- et ->");

}

/* ---------------------------------
            FT_CheckFX
     Check if Playing Sound FX
          must be updated
-----------------------------------*/
void FT_CheckFX (void)
{ 
 if (TestFX()==1)
 {
      if (JIFFY!=0)
      {
       JIFFY=0;
       UpdateFX();
      }
  }
}

void FT_SpriteDef(void)
{
  // Sprites Definition Patterns
//
 unsigned char board_pattern[]={
  0b11111111,
  0b11111111,
  0b11111111,
  0b11111111,
  0b11111111,
  0b00000000,
  0b00000000,
  0b00000000
};

 unsigned char ball_pattern[]={
    0b00111100,
    0b01111110,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b01111110,
    0b00111100
};
  char BallColors[8]= {15,8,8,8,8,8,8,15};
  char BoardColors[8]={15,15,15,15,15,15,15,15};

  // Building Sprites
  SpriteReset();
  SpriteSmall();
  SetSpritePattern(0, board_pattern,8);
  SetSpritePattern(1, ball_pattern,8);

  SC8SpriteColors(0,BallColors);
  SC8SpriteColors(1,BoardColors);
  SC8SpriteColors(2,BoardColors);
  SC8SpriteColors(3,BoardColors);


}


// M A I N
// 
void main(void) 
{
  // Main Variables definitions
  
  afbdata=MMalloc(956);
  animtab[0]=0;
  animtab[1]=24;
  animtab[2]=48;
  animtab[3]=24;

  //  Variables assignations when starting
  x=170;
  y=110;
  dir_x=1;
  dir_y=-1;
  div_x=1;
  div_y=1;
  px=130;
  py=185;
  brx=255;
  bry=255;
  verif_flag=0;
  border=100;
  nb_bricks=0;
  mvx=245;
  anim_dir=0;
  sens=-4;
  ball_cpt=4;
  santa=0;
  santawin=0;
  JIFFY = 0;
  level=1;
  life=5;
  score=0;
  gameOn=1;

  anim_speed=30;
  cpt=anim_speed;

   if(ReadMSXtype()==0)      // IF computer is MSX1. 
   {
       FT_errorHandler(3,"msx 1 ");
   }

  // Preparing Screen
  SetColors(15,0,1);
  InitPSG();
  SpriteOn();
  Screen(8);
  SetColors(255,0,border);
  Cls();
  KeySound(0);
  VDP60Hz();
  KillKeyBuffer();
  FT_FirstScreen();

  //
  FT_LoadSc8Image("title.SC8",256,LDbuffer);
  FT_LoadData("breaker.afb", afbdata, 956, 0);

  FT_ScreenCopy(0,0, 255, 211, 0, 0, 1 , 0, 0);
  FT_LoadSc8Image("BREAKER.SC8",256,LDbuffer);
  FT_LoadData("bells.pt3", song, 2305, 0);
  PT3Init (song+99, 0);
  InitFX();

  SetColors(250,0,border);
  PutText(52,202,"Press SPACE to Play",0);
  WaitKey();
  Cls();
  
 // WaitKey();
  if(ReadMSXtype()==3)  // If MSX is Turbo-R Switch CPU to Z80 Mode
  {
      ChangeCPU(0);
  }
  FT_SpriteDef();
  FT_EmptyField();
  FT_PutBricks();
 
  FT_ShowBricksWall();

  // Main GameLoop
    while (1)
    {  
          if (gameOn==1)
          {
       
            FT_Joy();

            FT_CalcPosition();
         
            FT_Srpites(); 

            FT_CheckFX();
          
          }
          else
          {
          ///
          }
    }
}