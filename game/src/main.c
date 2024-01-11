#include <stdio.h>
#include <stddef.h>
#include "raylib.h"
#include "body.h"
#include "list.h"
#include <math.h>

/* Static Functions */
static void init_bodies (struct body *bodies, size_t cnt, float pct_heavy);
static void draw_bodies (struct body *bodies, size_t cnt);
static void update_bodies (struct body *bodies, size_t cnt);
static void handle_collision (struct body *bodies, size_t cnt, size_t steps);
static double get_distance (struct body *bdyA, struct body *bdyB);
static void resolve_collision(struct body *bdyA, struct body *bdyB, double distance);
static void handle_camera_pos (Camera2D *_camera);


/* Global Constants */
const double dt = .10;
struct list collided_bodies;

int main(void)
{
    const int screenWidth = SCRNW;
    const int screenHeight = SRCHT;
    const int bdy_cnt = 800;
    
    InitWindow(screenWidth, screenHeight, "n-body");
    SetTargetFPS(60);
    
    Camera2D camera = {0};
    camera.target =  (Vector2) {0,0};
    camera.zoom = 1;

    struct body bodies[bdy_cnt];
    init_bodies (bodies, bdy_cnt, .01);
    list_init (&collided_bodies);
    
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
      // Update
      update_bodies (bodies, bdy_cnt); 
      handle_camera_pos (&camera);
      
      /* Draw Bodies */
      BeginDrawing();
        BeginMode2D (camera);
          ClearBackground(BLACK);
          
          draw_bodies (bodies, bdy_cnt);
        EndMode2D();
      EndDrawing();
    }

    CloseWindow();
    return 0;
}

/* Initializes Bodies Array 'BODIES' of count 'CNT' */
static void init_bodies (struct body *bodies, size_t cnt, float pct_heavy)
{
  /* Number of heavy bodies and light bodies */
  int heavy_cnt = cnt * pct_heavy;
  int light_cnt = cnt - heavy_cnt;
  int random_spawn_range = 5 * cnt;
  
  printf ("cnt=%ld\n", cnt);
  printf ("heavy_cnt=%d\n", heavy_cnt);
  printf ("Light_cnt=%d\n", light_cnt);

  while (heavy_cnt--)
  {
    cnt--;
    printf("Spawning heavy !\n");
    printf ("cnt=%ld\n", cnt);
    struct body *bdy = &bodies[cnt];
    bdy->color = RAYWHITE;
    bdy->radius = 10;
    bdy->mass = 50;//GetRandomValue (20, 9999);
    bdy->posX = CENTER_X + GetRandomValue(-1 * random_spawn_range, random_spawn_range);
    bdy->posY = CENTER_Y + GetRandomValue(-1 * random_spawn_range, random_spawn_range); 
    bdy->vel_x = 0;
    bdy->vel_y = 0;
    
  }

  while (light_cnt --)
  {
    cnt--;
    printf("Spawning light !\n");
    printf ("cnt=%ld\n", cnt);
    
    struct body *bdy = &bodies[cnt];
    bdy->color = RAYWHITE;
    bdy->radius = 10;
    bdy->mass = 10;//GetRandomValue (1, 10);
    bdy->posX = CENTER_X + GetRandomValue(-1 * random_spawn_range, random_spawn_range);
    bdy->posY = CENTER_Y + GetRandomValue(-1 * random_spawn_range, random_spawn_range);
    bdy->vel_x = 0;
    bdy->vel_y = 0;
    
  }
  
  printf("DONEGNERATINGBODIES\n");

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
      double r = pow (radial_dist, 2);

      /* Newton's Law of Gravity: F = mm-/r² ⟹ a = m/r² */
      ax -= bdy2->mass * (bdy1->posX - bdy2->posX) / r; //(pow(r, bdy1->vel_x) + r);
      ay -= bdy2->mass * (bdy1->posY - bdy2->posY) / r;//(pow(r, bdy1->vel_y) + r);
      
      if (radial_dist > (bdy1->radius + bdy2->radius) * .95)
      {
        // ax -= bdy2->mass * (bdy1->posX - bdy2->posX) / r; //(pow(r, bdy1->vel_x) + r);
        // ay -= bdy2->mass * (bdy1->posY - bdy2->posY) / r;//(pow(r, bdy1->vel_y) + r);
      } else {
       
       
      }
     }
     
     bdy1->vel_x += ax * dt;
     bdy1->vel_y += ay * dt;
  }

  handle_collision(bodies, cnt, 4); 
  for (int i = 0; i < cnt; i++)
  {
    struct body *bdy1 = &bodies[i];
    bdy1->posX += bdy1->vel_x * dt;
    bdy1->posY += bdy1->vel_y * dt;
  }
 

}

/* Populates list with all bodies that are coll*/
// static void get_colliding_bodies (struct body *bodies, int cnt)
// {
//   for (int i = 0; i < cnt; i++)
//     {
//       struct body *bdy1 = &bodies[i];
//       for (int j = 0; j < cnt; j++)  // Avoid self-collision
//       {
//          if (i == j) continue;
//          struct body *bdy2 = &bodies[j];

//          double dx = bdy1->posX - bdy2->posX;
//          double dy = bdy1->posY - bdy2->posY;
//          double dis = get_distance(bdy1, bdy2); // Using square of distance to avoid sqrt
//          double radi_sum = (bdy1->radius) + (bdy1->radius) + 0; // Square of combined radii

//           if (dis < radi_sum) // Check for collision
//           {
//             list_push_front (&collided_bodies, &bdy1->elem);
//           }
//         }
//     }
// }



/* Resolves Body Collisions with perfect inelastic collision */
/*
Used this wikipedia article to help with the impulse calculation 

https://en.wikipedia.org/wiki/Elastic_collision#:~:text=In%20an%20angle%2Dfree%20representation%2C%20the%20changed%20velocities%20are%20computed%20using%20the%20centers%20x1%20and%20x2%20at%20the%20time%20of%20contact%20as
*/
static void handle_collision (struct body *bodies, size_t cnt, size_t steps)
{
  // get_colliding_bodies (bodies, cnt);
/* make them random*/
  for (int i=0;i<cnt;i++){
    int new = GetRandomValue(0, cnt - 1);
    struct body c1 = bodies[i];
    bodies[i] = bodies[new];
    bodies[new] = c1;
}

  for (int i = 0; i < cnt; i++)
    {
      struct body *bdy1 = &bodies[i];
      // for (int s = 0; s < steps; s++)
      // {
      //   bdy1->posX += bdy1->vel_x * (dt / steps);
      //   bdy1->posY += bdy1->vel_y * (dt / steps);

        for (int j = i + 1; j < cnt; j++)  // Avoid self-collision
        {
          if (i == j) continue;
            struct body *bdy2 = &bodies[j];
            double gf = 5;
            double dx = bdy1->posX  - bdy2->posX;
            double dy = bdy1->posY - bdy2->posY;
            double dis = get_distance(bdy1, bdy2); // Using square of distance to avoid sqrt
            double radi_sum = (bdy1->radius) + (0*gf) + (bdy1->radius) + 0; // Square of combined radii

            if (dis < radi_sum) // Check for collision
          {
                double dvx = bdy1->vel_x - bdy2->vel_x;
                double dvy = bdy1->vel_y - bdy2->vel_y;
                
                // Check if particles are approaching each other
                if (dvx * dx + dvy * dy > 0) continue;

                // Coefficient of restitution
                double eps =  .8; // Can be set to a different value for less elastic collisions

                // Collision response calculations
                double mass_sum = bdy1->mass + bdy2->mass;
                double impulse_x = 1 * (2 * dx * (dvx * dx + dvy * dy)) / (dis * mass_sum);
                double impulse_y = 1 * (2 *  dy * (dvx * dx + dvy * dy)) / (dis * mass_sum);
                /* 
                According to Wikipedia
                v′₁ = 2 * (dx) * (dvx * dx + dvy * dy) / (dis * mass_sum)
                
                Impulse:= F × ΔT = m₁Δv 
                  -But Δv = v′₁ - v₁
                So F = 
                F × ΔT = 

                there needs to be a minimum impulse 
                */
                bdy1->vel_x -= (impulse_x * dt) / bdy2->mass;
                bdy1->vel_y -= (impulse_y * dt) / bdy2->mass;
                bdy2->vel_x += (impulse_x * dt) / bdy1->mass;
                bdy2->vel_y += (impulse_y * dt) / bdy1->mass;
                resolve_collision (bdy1, bdy2, dis);
                
                  /*
                  
                  double dvx = bdy1->vel_x - bdy2->vel_x;
            double dvy = bdy1->vel_y - bdy2->vel_y;
             // Check if particles are approaching each other - referenced rebound collision.c
             if (dvx*dx + dvy*dy > 0) continue; 
            resolve_collision (bdy1, bdy2, dis);
              // Momentum conservation for perfectly inelastic collision
            double vx = (bdy1->mass * bdy1->vel_x + bdy2->mass * bdy2->vel_x) / (bdy1->mass + bdy2->mass);
            double vy = (bdy1->mass * bdy1->vel_y + bdy2->mass * bdy2->vel_y) / (bdy1->mass + bdy2->mass);
            bdy1->vel_x = vx;
            bdy1->vel_y = vy;
            bdy2->vel_x = vx;
            bdy2->vel_y = vy;
                  */

          }
        // }
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
  //return (dist < distance_tolerance) ? distance_tolerance : dist; 
  return dist;  
}

/* 
   Resolves body collision by body 'BDYA' and body 'BDYB' by positioning both 
   bodies such that they are not intersecting.

   References: https://ericleong.me/research/circle-circle/
*/
static void resolve_collision(struct body *bdyA, struct body *bdyB, double distance)
{
  /* Midpoint tells us how far bodies needs 
     to be pushed to resolve intersection 
     (i.e collision) */
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

/* Updates CAMERA position from key press events */
static void handle_camera_pos (Camera2D *_camera)
{
  Camera2D camera = *_camera;
  float incr_amt = 3 * (pow (55, 1 - camera.zoom));
  
  /* Camera Position Controls */
  if (IsKeyDown(KEY_RIGHT)) camera.target.x += incr_amt;
  if (IsKeyDown (KEY_LEFT)) camera.target.x -= incr_amt;
  if (IsKeyDown (KEY_UP)) camera.target.y -= incr_amt;
  if (IsKeyDown (KEY_DOWN)) camera.target.y += incr_amt;
  
  /* Zoom Controls */
  if (IsKeyDown (KEY_W)) camera.zoom += .01;
  if (IsKeyDown (KEY_S)) camera.zoom -= .01;
  *_camera = camera;
}