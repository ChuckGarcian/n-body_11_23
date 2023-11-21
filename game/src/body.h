/* Screen Information */
#define SCRNW 800
#define SRCHT 450

/* Origin*/
#define CENTER_X SCRNW / 2
#define CENTER_Y SRCHT / 2

/* Body Object */
struct body 
{
  /* Simulation Relevant Properties */
  double posX;
  double posY;

  double vel_x;
  double vel_y;
  
  double mass;

  /* Graphics Related Properties */
  double radius;
  struct Color color; 
};
