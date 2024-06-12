#ifndef MPRIS_MEDIA_PLAYER_H
#define MPRIS_MEDIA_PLAYER_H

#include "log.h"
#include <dbus/dbus.h>
#include <iostream>
#include <string>

typedef enum DbusMethods {
  Next,
  OpenUri,
  Pause,
  Play,
  PlayPause,
  Seek,
  SetPosition,
  Stop
} DbusMethodType;

typedef enum DbusProperties {
  CanControl,
  CanGoNext,
  CanGoPrevious,
  CanPause,
  CanPlay,
  CanSeek,
  LoopStatus,
  MaximumRate,
  Metadata,
  MinimumRate,
  PlaybackStatus,
  Position,
  Rate,
  Shuffle,
  Volume
} DbusPropertyType;

class MprisMediaPlayer {
public:
  static const std::string PATH = "/org/mpris/MediaPlayer2";

public:
  MprisMediaPlayer(const std::string &session_name);
  ~MprisMediaPlayer();

  int connect();

  void print_dbus_error(const std::string &msg, DBusError *err);

private:
  DBusConnection *conn;
  DBusError err;
  DBusMessage *reply;

  std::string bus_name;

  Log log;
};

#endif /* MPRIS_MEDIA_PLAYER_H */
