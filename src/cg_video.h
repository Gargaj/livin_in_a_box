#ifdef WIN32
#include "cg_video_wgl.h"
#else
#include "cg_video_x11.h"
#endif

extern char             cg_isrunning;

int CreateScene(int width, int height, int mode);
void RestoreScene();
