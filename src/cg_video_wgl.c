#include "cg_video_wgl.h"
#include <stdlib.h>
#include <windows.h>
#include <GL/gl.h>
#include <GL/glext.h>

HWND hWnd = NULL;
HDC hDC = NULL;

PFNGLACTIVETEXTUREPROC glActiveTexture = NULL;
PFNGLATTACHSHADERPROC glAttachShader = NULL;
PFNGLBINDBUFFERPROC glBindBuffer = NULL;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray = NULL;
PFNGLBUFFERDATAPROC glBufferData = NULL;
PFNGLCOMPILESHADERPROC glCompileShader = NULL;
PFNGLCREATEPROGRAMPROC glCreateProgram = NULL;
PFNGLCREATESHADERPROC glCreateShader = NULL;
PFNGLDELETEBUFFERSPROC glDeleteBuffers = NULL;
PFNGLDELETEPROGRAMPROC glDeleteProgram = NULL;
PFNGLDELETESHADERPROC glDeleteShader = NULL;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays = NULL;
PFNGLDETACHSHADERPROC glDetachShader = NULL;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = NULL;
PFNGLGENBUFFERSPROC glGenBuffers = NULL;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays = NULL;
PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation = NULL;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog = NULL;
PFNGLGETPROGRAMIVPROC glGetProgramiv = NULL;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog = NULL;
PFNGLGETSHADERIVPROC glGetShaderiv = NULL;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = NULL;
PFNGLLINKPROGRAMPROC glLinkProgram = NULL;
PFNGLMAPBUFFERPROC glMapBuffer = NULL;
PFNGLPATCHPARAMETERIPROC glPatchParameteri = NULL;
PFNGLSHADERSOURCEPROC glShaderSource = NULL;
PFNGLUNIFORM1FPROC glUniform1f = NULL;
PFNGLUNIFORM1IPROC glUniform1i = NULL;
PFNGLUNIFORM2FPROC glUniform2f = NULL;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv = NULL;
PFNGLUNMAPBUFFERPROC glUnmapBuffer = NULL;
PFNGLUSEPROGRAMPROC glUseProgram = NULL;
PFNGLVALIDATEPROGRAMPROC glValidateProgram = NULL;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = NULL;

char cg_isrunning = 1;

double drand48()
{
  return rand() / (double)RAND_MAX;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
  switch (uMsg)                 
  {
    case WM_KEYDOWN:
      {
        switch( wParam ) {
          case VK_ESCAPE:
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        }
      }

    case WM_SYSCOMMAND: 
      {
        switch (wParam) {
          case SC_SCREENSAVE:         
          case SC_MONITORPOWER:       
            return 0;             
        } break;                  
      }

    case WM_CLOSE: 
      {
        cg_isrunning = 0;
        return 0;               
      }

  }

  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int CreateScene(int width, int height, int fullscreen) 
{
  WNDCLASS  WC;
  DWORD   wStyle;
  DWORD   wExStyle;
  RECT r;
  DEVMODE   sSettings;
  unsigned  PixelFormat;
  PIXELFORMATDESCRIPTOR pfd;
  HGLRC hRC;

  hWnd = NULL;
  
  // create a window class
  WC.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  WC.lpfnWndProc = WndProc;
  WC.cbClsExtra = 0;
  WC.cbWndExtra = 0;
  WC.hInstance = GetModuleHandle(NULL);
  WC.hIcon = NULL;
  WC.hCursor = LoadCursor(NULL, IDC_ARROW);
  WC.hbrBackground = NULL;
  WC.lpszMenuName = NULL;
  WC.lpszClassName = "cgcls";

  if (!RegisterClass(&WC)) return -1;
  
  wExStyle = WS_EX_APPWINDOW;
  wStyle = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
  if (!fullscreen) wStyle |= WS_OVERLAPPED | WS_CAPTION;// | WS_SYSMENU;

  r.top = r.left = 0;
  r.right = width;
  r.bottom = height;

  if (fullscreen) {
    // change display settings
    memset(&sSettings, 0, sizeof(sSettings));
    sSettings.dmSize = sizeof(sSettings);
    sSettings.dmPelsWidth = width;
    sSettings.dmPelsHeight = height;
    sSettings.dmBitsPerPel = 32;
    sSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
    if (ChangeDisplaySettings(&sSettings, 0) != DISP_CHANGE_SUCCESSFUL)
      return -1;
    ShowCursor(FALSE);
  } else {
    AdjustWindowRectEx(&r,wStyle,FALSE,wExStyle);
  }

  hWnd = CreateWindowEx(wExStyle,"cgcls","cg wgl",wStyle,
              0,0,width,height,
              NULL,NULL,GetModuleHandle(NULL),NULL);

  if (hWnd == NULL)
    return -1;

  // display window
  ShowWindow(hWnd, SW_SHOW);
  SetForegroundWindow(hWnd);
  SetFocus(hWnd);

  // create device context
  if (!(hDC = GetDC(hWnd))) 
    return -1;

  // set pixel format
  memset(&pfd, 0, sizeof(pfd));
  pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
  pfd.nVersion   = 1;
  pfd.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
  pfd.iPixelType = PFD_TYPE_RGBA;
  pfd.cColorBits = 32;
  pfd.cDepthBits = 32;
  pfd.dwLayerMask = PFD_MAIN_PLANE;

  if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd))) return -1;
  if (!SetPixelFormat(hDC, PixelFormat, &pfd))       return -1;

  if (!(hRC = wglCreateContext(hDC))) return -1;
  if (!wglMakeCurrent(hDC, hRC))      return -1;

  glActiveTexture = (PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture");
  glAttachShader = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
  glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
  glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)wglGetProcAddress("glBindVertexArray");
  glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
  glCompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
  glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
  glCreateShader = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
  glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");
  glDeleteProgram = (PFNGLDELETEPROGRAMPROC)wglGetProcAddress("glDeleteProgram");
  glDeleteShader = (PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader");
  glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)wglGetProcAddress("glDeleteVertexArrays");
  glDetachShader = (PFNGLDETACHSHADERPROC)wglGetProcAddress("glDetachShader");
  glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");
  glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
  glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)wglGetProcAddress("glGenVertexArrays");
  glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)wglGetProcAddress("glGetAttribLocation");
  glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)wglGetProcAddress("glGetProgramInfoLog");
  glGetProgramiv = (PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv");
  glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");
  glGetShaderiv = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");
  glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
  glLinkProgram = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
  glMapBuffer = (PFNGLMAPBUFFERPROC)wglGetProcAddress("glMapBuffer");
  glPatchParameteri = (PFNGLPATCHPARAMETERIPROC)wglGetProcAddress("glPatchParameteri");
  glShaderSource = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
  glUniform1f = (PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f");
  glUniform1i = (PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i");
  glUniform2f = (PFNGLUNIFORM2FPROC)wglGetProcAddress("glUniform2f");
  glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)wglGetProcAddress("glUniformMatrix4fv");
  glUnmapBuffer = (PFNGLUNMAPBUFFERPROC)wglGetProcAddress("glUnmapBuffer");
  glUseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
  glValidateProgram = (PFNGLVALIDATEPROGRAMPROC)wglGetProcAddress("glValidateProgram");
  glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer"); 

  return 0;
}

void RestoreScene(){
  DestroyWindow(hWnd);
  ChangeDisplaySettings(NULL,NULL);
}
