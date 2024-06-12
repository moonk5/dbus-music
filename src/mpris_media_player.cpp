#include "mpris_media_player.h"

MprisMediaPlayer::MprisMediaPlayer(const std::string &session_name)
    : bus_name(session_name) {

  log = Log("MprisMediaPlayer");
  dbus_error_init(&err);
}

MprisMediaPlayer::~MprisMediaPlayer() {
  // Clean up
  // if (conn) {
  //   dbus_connection_unref(conn);
  // }
  //
  log.debug("Clean-up done!");
}

int MprisMediaPlayer::connect() {

  conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
  if (dbus_error_is_set(&err)) {
    print_dbus_error("Connection Error", &err);
    log.error("Connection Error");
    return -1;
  }

  log.debug("Session(" + bus_name + ") is connected");

  return -1;
}

void MprisMediaPlayer::print_dbus_error(const std::string &msg,
                                        DBusError *err) {
  std::cerr << msg << ": " << err->name << " - " << err->message << std::endl;
  dbus_error_free(err);
}
