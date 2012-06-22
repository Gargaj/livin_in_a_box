#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include "cg_dialog.h"
#include "cg_constants.h"
#include <resource.h>


int width, height, fullscreen = 1, quit;

typedef struct {
  int w,h;
} RES;

// if your windows supports more than 4000 reolutions, well, we're screwed :)

#define MAXNUMRES 4096

int nRes = 0;
RES ress[MAXNUMRES];

BOOL CALLBACK DlgFunc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
  switch ( uMsg ) {
    case WM_INITDIALOG: {
      char s[500];
      int i=0;

      DEVMODE d;
      EnumDisplaySettings(NULL,0,&d);
      while(1) {
        BOOL h = EnumDisplaySettings(NULL,i++,&d);
        if (!h || nRes>MAXNUMRES) break;

        /*** You can use this following line to select only certain aspect ratios ***/
        //        if ((d.dmPelsWidth * 3) / 4 != d.dmPelsHeight) continue;

        if (d.dmBitsPerPel != 32) continue;
        if (!nRes || ress[nRes-1].w != d.dmPelsWidth || ress[nRes-1].h != d.dmPelsHeight) {
          ress[nRes].w = d.dmPelsWidth;
          ress[nRes].h = d.dmPelsHeight;
          nRes++;
          _snprintf(s,500,"%d * %d",d.dmPelsWidth,d.dmPelsHeight);
          SendDlgItemMessage(hWnd, IDC_RESOLUTION, CB_ADDSTRING, 0, (LPARAM)s);
        }
      }

      for (i=0; i<nRes; i++)
        if (ress[i].w==800 && ress[i].h==600)
          SendDlgItemMessage(hWnd, IDC_RESOLUTION, CB_SETCURSEL, i, 0);

      SendDlgItemMessage(hWnd, IDC_FULLSCREEN, BM_SETCHECK, 1, 1);
      EnableWindow(GetDlgItem(hWnd,IDC_ONTOP),0);

                        } break;
    case WM_COMMAND:
      switch( LOWORD(wParam) ) {
    case IDOK: {
      width = ress[SendDlgItemMessage(hWnd, IDC_RESOLUTION, CB_GETCURSEL, 0, 0)].w;
      height= ress[SendDlgItemMessage(hWnd, IDC_RESOLUTION, CB_GETCURSEL, 0, 0)].h;
      fullscreen = SendDlgItemMessage(hWnd, IDC_FULLSCREEN, BM_GETCHECK , 0, 0);
      EndDialog (hWnd, TRUE);
               } break;
    case IDCANCEL: {
      EndDialog (hWnd, FALSE);
                   } break;
      } break;
  }
  return ( WM_INITDIALOG == uMsg ) ? TRUE : FALSE;
}

int cgDialog(int *w, int *h, int *f){

 if (!DialogBox(GetModuleHandle(NULL),MAKEINTRESOURCE(IDD_SETUP),GetForegroundWindow(),DlgFunc))
   return CG_QUIT;

   *w = width;
   *h = height;
   *f = fullscreen;
   return(CG_RUNDEMO);
}
