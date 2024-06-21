#ifndef MPRIS_MEDIA_PLAYER_H
#define MPRIS_MEDIA_PLAYER_H

#include "log.h"
#include <cstdint>
#include <dbus/dbus.h>
#include <iostream>
#include <string>

typedef enum ErrorCode {
  ERROR_NONE = 1,
  ERROR_DBUS = -1,
  ERROR_NULL_PTR = -2,
  ERROR_UNKNOWN_TYPE = -3

} ErrorCodeType;

typedef enum DBusMethods {
  Next = 1,
  OpenUri,
  Pause,
  Play,
  PlayPause,
  Previous,
  Seek,
  SetPosition,
  Stop
} DBusMethodType;

typedef enum DBusProperties {
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
} DBusPropertyType;

typedef enum DBusLoopStatus {
  LoopStatusNone = 1,
  LoopStatusTrack,
  LoopStatusPlaylist
} DBusLoopStatusType;

typedef enum DBusGetSet { Getter = 1, Setter } DBusGetSetType;

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

  std::string get_loop_status();
  void set_loop_status(DBusLoopStatusType loop_status);

  double get_maximum_rate();
  void get_metadata(const std::string &service_name);
  int64_t get_position();
  double get_rate();
  bool get_shuffle();
  void set_shuffle(bool shuffle_on);
  double get_volume(); // read & write

  void next();
  void pause();
  void play();
  void play_pause();
  void previous();
  void seek(int64_t offset);
  // void set_position(track, int64_t position);
  void stop();

  std::string convert_dbus_method_type_to_string(DBusMethodType method);
  std::string convert_dbus_property_type_to_string(DBusPropertyType property);
  std::string convert_dbus_loop_status(DBusLoopStatusType loopStatus);

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
  int construct_new_dbus_msg(DBusMessage *&msg, DBusMethodType method_type);
  int construct_new_dbus_msg(DBusGetSetType getset_type,
                             DBusPropertyType property_type, DBusMessage *&msg,
                             void *set_value);

  int send_dbus_msg(DBusMessage *&msg);
  int send_dbus_msg_with_reply(DBusMessage *&msg, DBusMessage *&reply,
                               DBusError &err);

  void execute_base_method_func(DBusMethodType type);
  int execute_base_property_func(DBusGetSetType getset,
                                 DBusPropertyType property_type,
                                 DBusMessage *&reply, void *set_value);

  int get_base_property_func(DBusPropertyType type, DBusMessage *&reply);
  int set_base_property_func(DBusPropertyType type, void *set_value);

  bool property_func_return_bool(DBusPropertyType type);

  int read_reply(DBusMessage *&reply, void *output);

  bool is_connected;
  DBusConnection *conn;

  std::string session_name;

  Log log;
};

#endif /* MPRIS_MEDIA_PLAYER_H */
