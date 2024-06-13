#include "spdlog/spdlog.h"
#include <cstdint>
#include <dbus/dbus.h>
#include <iostream>
#include <string>

#include "log.h"
#include "mpris_media_player.h"

int main(void) {

  MprisMediaPlayer mmp("org.mpris.MediaPlayer2.ncspot");

  //  const std::string service_name = "org.mpris.MediaPlayer2.ncspot";
  const std::string service_name =
      "org.mpris.MediaPlayer2.firefox.instance_1_20";
  std::cout << "Fetching metadata from service: " << service_name << std::endl;

  mmp.get_metadata(service_name);

  return 0;
}
