#include <stdio.h>
#include <stddef.h>
#include "raylib.h"
#include "body.h"
#include <math.h>

/* Static Functions */
static void init_bodies (struct body *bodies, size_t cnt);
static void draw_bodies (struct body *bodies, size_t cnt);
static void update_bodies (struct body *bodies, size_t cnt);
static void handle_collision (struct body *bodies, size_t cnt);
static double get_distance (struct body *bdyA, struct body *bdyB);
static void resolve_collision(struct body *bdyA, struct body *bdyB, double distance);

/* Global Constants */
const double dt = .2;

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
      handle_collision (bodies, bdy_cnt);
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
      double radial_dist = get_distance (bdy1, bdy2);      
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

/* Resolves Body Collisions with perfect inelastic collision */
static void handle_collision (struct body *bodies, size_t cnt)
{
  for (int i = 0; i < cnt; i++)
    {
        struct body *bdy1 = &bodies[i];

        for (int j = 0; j < cnt; j++)  // Avoid self-collision
        {
          if (i == j) continue;
            struct body *bdy2 = &bodies[j];

            double dx = bdy1->posX - bdy2->posX;
            double dy = bdy1->posY - bdy2->posY;
            double dis = get_distance(bdy1, bdy2); // Using square of distance to avoid sqrt
            double radi_sum = (bdy1->radius) + (bdy1->radius) + 1; // Square of combined radii

            if (dis <= radi_sum) // Check for collision
          {
            resolve_collision (bdy1, bdy2, dis);
              // Momentum conservation for perfectly inelastic collision
              double vx = (bdy1->mass * bdy1->vel_x + bdy2->mass * bdy2->vel_x) / (bdy1->mass + bdy2->mass);
              double vy = (bdy1->mass * bdy1->vel_y + bdy2->mass * bdy2->vel_y) / (bdy1->mass + bdy2->mass);
              bdy1->vel_x = vx;
              bdy1->vel_y = vy;
              bdy2->vel_x = vx;
              bdy2->vel_y = vy;
          }
        }
    }

}


/* Calculates the distance between the two bodies: BDYA and BDYB */
static double get_distance (struct body *bdyA, struct body *bdyB)
{
  double dx = bdyA->posX - bdyB->posX; 
  double dy = bdyA->posY - bdyB->posY;
  double dist = sqrt (dx * dx + dy * dy);
  double distance_tolerance = .20; /* Smallest distance two objects can be */
  return (dist < distance_tolerance) ? distance_tolerance : dist;   
}

/* 
   Resolves body collision by body 'BDYA' and body 'BDYB' by positioning both 
   bodies such that they are not intersecting.
   
   References: https://ericleong.me/research/circle-circle/
  */
*/
static void resolve_collision(struct body *bdyA, struct body *bdyB, double distance)
{
  /* Midpoint tells us how far bodies needs to be pushed
     to resolve intersection (i.e collision) */
  double midpoint_x = (bdyA->posX + bdyB->posX) / 2;
  double midpoint_y = (bdyA->posY + bdyB->posY) / 2;
  
  /* Resolve body overlap */
  double original_bdyA_x = bdyA->posX;
  double original_bdyA_y = bdyA->posY;
  double original_bdyB_x = bdyB->posX;
  double original_bdyB_y = bdyB->posY;
  
  bdyA->posX = midpoint_x + bdyA->radius * (original_bdyA_x - original_bdyB_x) / distance;
  bdyA->posY = midpoint_y + bdyA->radius * (original_bdyA_y - original_bdyB_y) / distance;
  bdyB->posX = midpoint_x + bdyB->radius * (original_bdyB_x - original_bdyA_x) / distance;
  bdyB->posY = midpoint_y + bdyB->radius * (original_bdyB_y - original_bdyA_y) / distance;
}
