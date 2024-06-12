#include "spdlog/spdlog.h"
#include <cstdint>
#include <dbus/dbus.h>
#include <iostream>
#include <string>

#include "log.h"
#include "mpris_media_player.h"

int main(void) {

  MprisMediaPlayer mmp("org.mpris.MediaPlayer2.ncspot");

  mmp.connect();

  return 0;
}
