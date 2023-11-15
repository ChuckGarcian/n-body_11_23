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
  int posX;
  int posY;

  int vel_x;
  int vel_y;
  
  int mass;

  /* Graphics Related Properties */
  int radius;
  struct Color color; 
};
