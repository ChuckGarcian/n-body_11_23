#include <stdio.h>
#include <stddef.h>
#include "raylib.h"
#include "body.h"
#include <math.h>

static void init_bodies (struct body *bodies, size_t cnt);
static void draw_bodies (struct body *bodies, size_t cnt);
static void update_bodies (struct body *bodies, size_t cnt);
const double dt = .1

int main(void)
{
    const int screenWidth = SCRNW;
    const int screenHeight = SRCHT;
    const int bdy_cnt = 10; 

    struct body bodies[bdy_cnt];
    init_bodies (bodies, bdy_cnt);
    
    InitWindow(screenWidth, screenHeight, "n-body");
    SetTargetFPS(60);
     
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
      // Update
      update_bodies (bodies, bdy_cnt);     
      /* Draw Bodies */
      BeginDrawing();
      ClearBackground(RAYWHITE);
      draw_bodies (bodies, bdy_cnt);
      EndDrawing();
    }

    CloseWindow();
    return 0;
}

/* Initializes Bodies Array 'BODIES' of count 'CNT' */
static void init_bodies (struct body *bodies, size_t cnt)
{
  while (cnt--)
  {
    struct body *bdy = &bodies[cnt];
    bdy->color = RED;
    bdy->radius = 10;
    bdy->mass = 100;
    bdy->posX = CENTER_X + cnt * 30;
    bdy->posY = CENTER_Y + cnt * -20; 
    bdy->vel_x = 0;
    bdy->vel_y = 0;
  }
}

static void draw_bodies (struct body *bodies, size_t cnt)
{
  while (cnt--)
  {
    struct body *bdy = &bodies[cnt];
    DrawCircle (bdy->posX, bdy->posY, bdy->radius, bdy->color);            
  }
}

/* Updates all bodies by a time step */
static void update_bodies (struct body *bodies, size_t cnt)
{
  
  // printf ("cnt=%d \n", cnt);
  for (int i = 0; i < cnt; i++)
  {
    struct body *bdy1 = &bodies[i];
    double ax = 0;
    double ay = 0;

    for (int j = 0; j < cnt; j++)
     {
      if (i == j) continue;
      struct body *bdy2 = &bodies[j];
      
      /* Determine Distance between bodies */
      double radial_dist = sqrt (pow (bdy1->posX - bdy2->posX, 2) + pow (bdy1->posY - bdy2->posY, 2));
      double r = radial_dist * radial_dist; // Needs to be squared not cubed 
    
      /* Newton's Law of Gravity: F = mm-/r² ⟹ a = m/r² */      
      ax -= bdy2->mass * (bdy1->posX - bdy2->posX) / r;
      ay -= bdy2->mass * (bdy1->posY - bdy2->posY) / r;
     }
    
     bdy1->vel_x += ax * dt;
     bdy1->vel_y += ay * dt;
  }

  /* Finalize position */
  for (int i = 0; i < cnt; i++)
    {
      struct body *bdy = &bodies[i];
      bdy->posX += bdy->vel_x * dt;
      bdy->posY += bdy->vel_y * dt;
    }


}
