#include <stdlib.h>
#include <string.h>
#include "cg_dialog.h"
#include "cg_constants.h"

int width, height, fullscreen = 1, quit;

/*
 * cgDialog is the connection between the application and the
 * Window content managment system.
 */
int cgDialog(int *w, int *h, int *f){

   *w = 800;
   *h = 600;
   *f = 0;

   return(CG_RUNDEMO);
}
