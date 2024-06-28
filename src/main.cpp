#include "spdlog/spdlog.h"
#include <cstdint>
#include <dbus/dbus.h>
#include <iostream>
#include <string>

#include "log.h"
#include "mpris_media_player.h"

int main(void) {

  const std::string service_name =
      "org.mpris.MediaPlayer2.ncspot.instance315730";
  // const std::string service_name =
  //     "org.mpris.MediaPlayer2.firefox.instance_1_20";

  MprisMediaPlayer mmp(service_name);

  mmp.test_menu();

  // mmp.get_metadata(service_name);

  return 0;
}
