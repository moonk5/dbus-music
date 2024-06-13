#ifndef MPRIS_MEDIA_PLAYER_H
#define MPRIS_MEDIA_PLAYER_H

#include "log.h"
#include <dbus/dbus.h>
#include <iostream>
#include <string>

typedef enum ErrorCode {
  SUCCESS = 1,
  DBUS_ERROR = -1,
  NULL_PTR = -2,

} ErrorCodeType;

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
  static const std::string PATH;

public:
  MprisMediaPlayer(const std::string &session_name);

  void get_metadata(const std::string &service_name);

private:
  std::string get_dbus_error(const std::string &msg, DBusError *err);
  void print_dbus_variant(DBusMessageIter *iter);

  int connect();

  DBusConnection *conn;
  DBusError err;
  DBusMessage *reply;

  std::string bus_name;

  Log log;
};

#endif /* MPRIS_MEDIA_PLAYER_H */
