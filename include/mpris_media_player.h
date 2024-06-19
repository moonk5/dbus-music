#ifndef MPRIS_MEDIA_PLAYER_H
#define MPRIS_MEDIA_PLAYER_H

#include "log.h"
#include <cstdint>
#include <dbus/dbus.h>
#include <iostream>
#include <string>

typedef enum ErrorCode {
  NONE = 1,
  DBUS_ERROR = -1,
  NULL_PTR = -2,
  UNKNOWN_TYPE = -3

} ErrorCodeType;

typedef enum DbusMethods {
  Next = 1,
  OpenUri,
  Pause,
  Play,
  PlayPause,
  Previous,
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

  // FIXME: search for list of services

  bool can_control();
  bool can_go_next();
  bool can_go_previous();
  bool can_pause();
  bool can_play();
  bool can_seek();

  void next();
  void pause();
  void play();
  void play_pause();
  void previous();
  void seek(int64_t offset);
  // void set_position(track, int64_t position);
  void stop();

  void get_metadata(const std::string &service_name);

  std::string convert_dbus_method_type_to_string(DbusMethodType type);
  std::string convert_dbus_property_type_to_string(DbusPropertyType type);

  /* Test */
  void test_menu();

private:
  std::string get_dbus_error(const std::string &msg, DBusError *err);
  void print_dbus_variant(DBusMessageIter *iter);

  int connect();
  void disconnect();

  DBusMessage *_dbus_msg_new_method_call(const std::string &dest,
                                         const std::string &path,
                                         const std::string &iface,
                                         const std::string &method);
  int construct_new_dbus_msg(DBusMessage *&msg, DbusMethodType type);
  int construct_new_dbus_msg(DBusMessage *&msg, DbusPropertyType type);

  int send_dbus_msg(DBusMessage *&msg);
  int send_dbus_msg_with_reply(DBusMessage *&msg, DBusMessage *&reply,
                               DBusError &err);

  void execute_base_method_func(DbusMethodType type);

  bool is_connected;
  DBusConnection *conn;

  std::string session_name;

  Log log;
};

#endif /* MPRIS_MEDIA_PLAYER_H */
