#include "cg_music.h"
#include "cg_video.h"
#include "bass.h"

#pragma comment(lib,"bass.lib")

HSTREAM hStream = NULL;

void InitMusic(char *musicfilename) {
  if (!BASS_Init(-1,44100,NULL,hWnd,NULL))
    return 0;

  hStream = BASS_StreamCreateFile(FALSE,musicfilename,0,0,0);

  BASS_Start();
}
void SyncPlay() {
  if (!BASS_ChannelIsActive(hStream))
    BASS_ChannelPlay(hStream,FALSE);
}

void FreeMusic() {
  BASS_Stop();
  BASS_StreamFree(hStream);
  BASS_Free();
}

