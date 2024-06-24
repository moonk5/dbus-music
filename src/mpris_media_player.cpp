#include "mpris_media_player.h"
#include "dbus/dbus-protocol.h"
#include <bits/types/struct_sched_param.h>
#include <cstdint>
#include <new>
#include <system_error>

const std::string MprisMediaPlayer::PATH = "/org/mpris/MediaPlayer2";

MprisMediaPlayer::MprisMediaPlayer(const std::string &session)
    : session_name(session), is_connected(false) {
  log = Log("MprisMediaPlayer");
}

int MprisMediaPlayer::connect() {
  DBusError err;

  // Initialize the error
  dbus_error_init(&err);

  std::cout << "Connecting to the D-Bus session bus..." << std::endl;

  if (is_connected) {
    // forcibly reconnect
    if (conn) {
      dbus_connection_unref(conn);
      is_connected = false;
    }
  }

  conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
  if (dbus_error_is_set(&err)) {
    log.error(get_dbus_error("Connection Error", &err));
    return ERROR_DBUS;
  }

  if (!conn) {
    std::cerr << "Connection Null" << std::endl;
    return ERROR_NULL_PTR;
  }

  is_connected = true;
  log.debug("Session(" + session_name + ") is connected");

  return ERROR_NONE;
}

void MprisMediaPlayer::disconnect() {
  dbus_connection_unref(conn);
  is_connected = false;
}

std::string
MprisMediaPlayer::convert_dbus_method_type_to_string(DBusMethodType type) {
  std::string method_str;

  switch (type) {
  case Next:
    method_str = "Next";
    break;
  case OpenUri:
    method_str = "OpenUri";
    break;
  case Pause:
    method_str = "Pause";
    break;
  case Play:
    method_str = "Play";
    break;
  case PlayPause:
    method_str = "PlayPause";
    break;
  case Previous:
    method_str = "Previous";
    break;
  case Seek:
    method_str = "Seek";
    break;
  case SetPosition:
    method_str = "SetPosition";
    break;
  case Stop:
    method_str = "Stop";
    break;
  default:
    method_str = "Unknown";
    break;
  }

  return method_str;
}
std::string
MprisMediaPlayer::convert_dbus_property_type_to_string(DBusPropertyType type) {
  std::string property_str;

  switch (type) {
  case CanControl:
    property_str = "CanControl";
    break;
  case CanGoNext:
    property_str = "CanGoNext";
    break;
  case CanGoPrevious:
    property_str = "CanGoPrevious";
    break;
  case CanPause:
    property_str = "CanPause";
    break;
  case CanPlay:
    property_str = "CanPlay";
    break;
  case CanSeek:
    property_str = "CanSeek";
    break;
  case LoopStatus:
    property_str = "LoopStatus";
    break;
  case MaximumRate:
    property_str = "MaximumRate";
    break;
  case Metadata:
    property_str = "Metadata";
    break;
  case MinimumRate:
    property_str = "MinimumRate";
    break;
  case PlaybackStatus:
    property_str = "PlaybackStatus";
    break;
  case Position:
    property_str = "Position";
    break;
  case Rate:
    property_str = "Rate";
    break;
  case Shuffle:
    property_str = "Shuffle";
    break;
  case Volume:
    property_str = "Volume";
    break;
  defaut:
    property_str = "Unknown";
    break;
  }

  return property_str;
}

std::string
MprisMediaPlayer::convert_dbus_loop_status(DBusLoopStatusType loopStatus) {
  std::string loop_status_str = "None";

  switch (loopStatus) {
  case LoopStatusNone:
    loop_status_str = "None";
    break;
  case LoopStatusTrack:
    loop_status_str = "Track";
    break;
  case LoopStatusPlaylist:
    loop_status_str = "Playlist";
    break;
  }

  return loop_status_str;
}

std::string MprisMediaPlayer::get_dbus_error(const std::string &msg,
                                             DBusError *err) {
  std::string err_str =
      "[DBUS ERROR] " + msg + ": " + err->name + " - " + err->message;
  dbus_error_free(err);
  return err_str;
}

void MprisMediaPlayer::print_dbus_variant(DBusMessageIter *iter) {
  int type = dbus_message_iter_get_arg_type(iter);
  switch (type) {
  case DBUS_TYPE_BOOLEAN: {
    bool value;
    dbus_message_iter_get_basic(iter, &value);
    std::cout << ((value) ? "true" : "false");
    break;
  }
  case DBUS_TYPE_STRING: {
    std::cout << "string type" << std::endl;
    char *value;
    dbus_message_iter_get_basic(iter, &value);
    std::cout << value;
    break;
  }
  case DBUS_TYPE_INT32: {
    std::cout << "int32" << std::endl;
    int32_t value;
    dbus_message_iter_get_basic(iter, &value);
    std::cout << value;
    break;
  }
  case DBUS_TYPE_UINT32: {
    std::cout << "uint32" << std::endl;
    uint32_t value;
    dbus_message_iter_get_basic(iter, &value);
    std::cout << value;
    break;
  }
  case DBUS_TYPE_INT64: {
    std::cout << "int64" << std::endl;
    int64_t value;
    dbus_message_iter_get_basic(iter, &value);
    std::cout << value;
    break;
  }
  case DBUS_TYPE_UINT64: {
    std::cout << "uint64" << std::endl;
    uint64_t value;
    dbus_message_iter_get_basic(iter, &value);
    std::cout << value;
    break;
  }
  case DBUS_TYPE_DOUBLE: {
    std::cout << "double" << std::endl;
    double value;
    dbus_message_iter_get_basic(iter, &value);
    std::cout << value;
    break;
  }
  case DBUS_TYPE_OBJECT_PATH: {
    std::cout << "object" << std::endl;
    char *value;
    dbus_message_iter_get_basic(iter, &value);
    std::cout << value;
    break;
  }
  case DBUS_TYPE_ARRAY: {
    std::cout << "array" << std::endl;
    DBusMessageIter sub_iter;
    dbus_message_iter_recurse(iter, &sub_iter);
    std::cout << "[";
    while (dbus_message_iter_get_arg_type(&sub_iter) != DBUS_TYPE_INVALID) {
      print_dbus_variant(&sub_iter);
      dbus_message_iter_next(&sub_iter);
      if (dbus_message_iter_get_arg_type(&sub_iter) != DBUS_TYPE_INVALID) {
        std::cout << ", ";
      }
    }
    std::cout << "]";
    break;
  }
  default:
    std::cout << "(unsupported type " << type << ")";
    break;
  }
}

DBusMessage *MprisMediaPlayer::_dbus_msg_new_method_call(
    const std::string &dest, const std::string &path, const std::string &iface,
    const std::string &method) {
  // Create a new method call message
  std::cout << "Creating a new method call message..." << std::endl;
  std::cout << "| Destination: " << dest << "\n"
            << "| Path: " << path << "\n"
            << "| Interface: " << iface << "\n"
            << "| Method:" << method << std::endl;

  return dbus_message_new_method_call(dest.c_str(), path.c_str(), iface.c_str(),
                                      method.c_str());
}

int MprisMediaPlayer::construct_new_dbus_msg(DBusMessage *&msg,
                                             DBusMethodType type) {
  std::string iface = "org.mpris.MediaPlayer2.Player";
  std::string method = convert_dbus_method_type_to_string(type);

  if (method == "Unknown") {
    return ERROR_UNKNOWN_TYPE;
  }

  msg = _dbus_msg_new_method_call(session_name, PATH, iface, method);
  if (!msg) {
    std::cerr << "Message Null" << std::endl;
    return ERROR_NULL_PTR;
  }
  std::cout << "Method call message created." << std::endl;

  return ERROR_NONE;
}

int MprisMediaPlayer::construct_new_dbus_msg(DBusGetSetType getset_type,
                                             DBusPropertyType property_type,
                                             DBusMessage *&msg,
                                             void *set_value) {
  DBusMessageIter args;
  DBusMessageIter sub_iter;

  std::string iface = "org.freedesktop.DBus.Properties";
  std::string method = ((getset_type == Getter) ? "Get" : "Set");
  std::string param_iface_name = "org.mpris.MediaPlayer2.Player";
  std::string param_property_name;

  msg = _dbus_msg_new_method_call(session_name.c_str(), PATH.c_str(),
                                  iface.c_str(), method.c_str());
  if (!msg) {
    std::cerr << "Message Null" << std::endl;
    return ERROR_NULL_PTR;
  }

  // Append arguments
  param_property_name = convert_dbus_property_type_to_string(property_type);
  if (param_property_name == "Unknown") {
    return ERROR_UNKNOWN_TYPE;
  }

  std::cout << "| Parameters:\n|\t - Iface Name: " << param_iface_name
            << "\n|\t - Property Name: " << param_property_name << std::endl;

  dbus_message_iter_init_append(msg, &args);
  dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &param_iface_name);
  dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &param_property_name);

  // Append the value variant
  if (getset_type == Setter) {
    DBusLoopStatusType *loop_type;
    const char *loop_status_cstr;
    switch (property_type) {
    case LoopStatus:
      loop_type = static_cast<DBusLoopStatusType *>(set_value);
      std::cout << "loop_type: " << convert_dbus_loop_status(*loop_type);
      loop_status_cstr = convert_dbus_loop_status(*loop_type).c_str();

      dbus_message_iter_open_container(&args, DBUS_TYPE_VARIANT, "s",
                                       &sub_iter);
      dbus_message_iter_append_basic(&sub_iter, DBUS_TYPE_STRING,
                                     &loop_status_cstr);
      dbus_message_iter_close_container(&args, &sub_iter);

      break;
    case Shuffle:
      dbus_message_iter_open_container(&args, DBUS_TYPE_VARIANT, "b",
                                       &sub_iter);
      dbus_message_iter_append_basic(&sub_iter, DBUS_TYPE_BOOLEAN,
                                     static_cast<bool *>(set_value));
      dbus_message_iter_close_container(&args, &sub_iter);

      break;
    case Volume:
      dbus_message_iter_open_container(&args, DBUS_TYPE_VARIANT, "d",
                                       &sub_iter);
      dbus_message_iter_append_basic(&sub_iter, DBUS_TYPE_DOUBLE,
                                     static_cast<double *>(set_value));
      dbus_message_iter_close_container(&args, &sub_iter);

      break;
    default:
      return ERROR_DBUS;
      break;
    }
  }
  std::cout << "Method call message created." << std::endl;

  return ERROR_NONE;
}

int MprisMediaPlayer::send_dbus_msg(DBusMessage *&msg) {
  if (msg == nullptr) {
    std::cout << "Message creation failed." << std::endl;
    return ERROR_NULL_PTR;
  }

  // Set the no-reply flag
  dbus_message_set_no_reply(msg, true);

  // Send the message and flush the connection
  if (!dbus_connection_send(conn, msg, nullptr)) {
    std::cout << "Out of memory." << std::endl;
    return ERROR_DBUS;
  }

  return ERROR_NONE;
}

int MprisMediaPlayer::send_dbus_msg_with_reply(DBusMessage *&msg,
                                               DBusMessage *&reply,
                                               DBusError &err) {
  // Send the message and get a reply
  std::cout << "Sending the message and waiting for a reply..." << std::endl;
  reply = dbus_connection_send_with_reply_and_block(conn, msg, -1, &err);
  if (dbus_error_is_set(&err)) {
    log.error(get_dbus_error("Error in Send Message", &err));
    dbus_message_unref(msg);
    return ERROR_DBUS;
  }
  if (!reply) {
    std::cout << "Reply Null" << std::endl;
    dbus_message_unref(msg);
    return ERROR_NULL_PTR;
  }
  std::cout << "Reply received." << std::endl;

  return ERROR_NONE;
}

void MprisMediaPlayer::execute_base_method_func(DBusMethodType type) {
  DBusMessage *msg;

  // Connect to the session bus
  if (!is_connected && !connect()) {
    return;
  }

  // Create a new method call Message
  if (construct_new_dbus_msg(msg, type) != ERROR_NONE) {
    return;
  }

  if (send_dbus_msg(msg) != ERROR_NONE) {
    return;
  }

  // Clean up
  dbus_message_unref(msg);
  disconnect();
  std::cout << "Cleanup done." << std::endl;

  return;
}

int MprisMediaPlayer::execute_base_property_func(DBusGetSetType getset,
                                                 DBusPropertyType type,
                                                 DBusMessage *&reply,
                                                 void *set_value) {

  DBusError err;
  DBusMessage *msg;
  DBusMessageIter args;
  int output = ERROR_NONE;

  // Initialize the error
  dbus_error_init(&err);

  // Connect to the session bus
  if (!is_connected && ((output = connect()) != ERROR_NONE)) {
    return output;
  }

  // Create a new method call Message
  if ((output = construct_new_dbus_msg(getset, type, msg, set_value)) !=
      ERROR_NONE) {
    return output;
  }

  if ((output = send_dbus_msg_with_reply(msg, reply, err)) != ERROR_NONE) {
    return output;
  }

  // Clean up
  dbus_message_unref(msg);
  disconnect();
  std::cout << "Cleanup done." << std::endl;

  return output;
}

bool MprisMediaPlayer::property_func_return_bool(DBusPropertyType type) {
  DBusMessage *reply;
  bool output = false;

  if (execute_base_property_func(Getter, type, reply, nullptr) != ERROR_NONE)
    return output;

  read_reply(reply, &output);

  if (reply == nullptr)
    dbus_message_unref(reply);

  std::cout << convert_dbus_property_type_to_string(type) << ": "
            << ((output) ? "true" : "false") << std::endl;
  return output;
}

double MprisMediaPlayer::property_func_return_double(DBusPropertyType type) {
  DBusMessage *reply;
  double output = 0.0;

  if (execute_base_property_func(Getter, type, reply, nullptr) != ERROR_NONE)
    return output;

  read_reply(reply, &output);

  if (reply == nullptr)
    dbus_message_unref(reply);

  std::cout << convert_dbus_property_type_to_string(type) << ": " << output
            << std::endl;

  return output;
}

int MprisMediaPlayer::read_reply(DBusMessage *&reply, void *output) {
  DBusMessageIter args;

  if (!dbus_message_iter_init(reply, &args)) {
    std::cout << "Message has no arguments!" << std::endl;
    return ERROR_DBUS;
  }

  if (DBUS_TYPE_VARIANT != dbus_message_iter_get_arg_type(&args)) {
    std::cout << "Argument is not variant!" << std::endl;
    return ERROR_DBUS;
  }

  DBusMessageIter variant_iter;
  dbus_message_iter_recurse(&args, &variant_iter);

  auto arg_type = dbus_message_iter_get_arg_type(&variant_iter);

  switch (arg_type) {
  case DBUS_TYPE_BOOLEAN:
  case DBUS_TYPE_INT64:
  case DBUS_TYPE_DOUBLE:
    dbus_message_iter_get_basic(&variant_iter, output);
    break;
  case DBUS_TYPE_ARRAY:
    DBusMessageIter dict_iter;
    dbus_message_iter_recurse(&variant_iter, &dict_iter);

    while (dbus_message_iter_get_arg_type(&dict_iter) != DBUS_TYPE_INVALID) {
      if (DBUS_TYPE_DICT_ENTRY == dbus_message_iter_get_arg_type(&dict_iter)) {
        DBusMessageIter dict_entry_iter;
        dbus_message_iter_recurse(&dict_iter, &dict_entry_iter);

        if (dbus_message_iter_get_arg_type(&dict_entry_iter) ==
            DBUS_TYPE_STRING) {
          char *key;
          dbus_message_iter_get_basic(&dict_entry_iter, &key);
          dbus_message_iter_next(&dict_entry_iter);

          if (dbus_message_iter_get_arg_type(&dict_entry_iter) ==
              DBUS_TYPE_VARIANT) {
            DBusMessageIter value_iter;
            dbus_message_iter_recurse(&dict_entry_iter, &value_iter);
            std::cout << key << ": ";
            print_dbus_variant(&value_iter);
            std::cout << std::endl;
          }
        }
      }
      dbus_message_iter_next(&dict_iter);
    }
  }

  return ERROR_NONE;
}

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

bool MprisMediaPlayer::can_control() {
  return property_func_return_bool(CanControl);
}

bool MprisMediaPlayer::can_go_next() {
  return property_func_return_bool(CanGoNext);
}

bool MprisMediaPlayer::can_go_previous() {
  return property_func_return_bool(CanGoPrevious);
}
#pragma
bool MprisMediaPlayer::can_pause() {
  return property_func_return_bool(CanPause);
}

bool MprisMediaPlayer::can_play() { return property_func_return_bool(CanPlay); }

bool MprisMediaPlayer::can_seek() { return property_func_return_bool(CanSeek); }

bool MprisMediaPlayer::get_shuffle() {
  return property_func_return_bool(Shuffle);
}

void MprisMediaPlayer::set_shuffle(bool shuffle_on) {
  DBusMessage *reply;
  if (execute_base_property_func(Setter, Shuffle, reply, &shuffle_on) !=
      ERROR_NONE)
    return;

  if (reply == nullptr)
    dbus_message_unref(reply);

  return;
}
double MprisMediaPlayer::get_maximum_rate() {
  return property_func_return_double(MaximumRate);
}

double MprisMediaPlayer::get_minimum_rate() {
  return property_func_return_double(MinimumRate);
}

double MprisMediaPlayer::get_rate() {
  return property_func_return_double(Rate);
}

double MprisMediaPlayer::get_volume() {
  return property_func_return_double(Volume);
}

void MprisMediaPlayer::set_volume(double volume) {
  DBusMessage *reply;
  if (execute_base_property_func(Setter, Volume, reply, &volume) != ERROR_NONE)
    return;

  if (reply == nullptr)
    dbus_message_unref(reply);

  return;
}

int64_t MprisMediaPlayer::get_position() {
  DBusMessage *reply;
  int64_t output = 0;

  if (execute_base_property_func(Getter, Position, reply, nullptr) !=
      ERROR_NONE)
    return output;

  read_reply(reply, &output);

  if (reply == nullptr)
    dbus_message_unref(reply);

  std::cout << "position: " << output << std::endl;

  return output;
}

std::string MprisMediaPlayer::get_loop_status() { return ""; }

void MprisMediaPlayer::set_loop_status(DBusLoopStatusType loop_status) {
  DBusMessage *reply;
  if (execute_base_property_func(Setter, LoopStatus, reply, &loop_status) !=
      ERROR_NONE)
    return;

  if (reply == nullptr)
    dbus_message_unref(reply);

  return;
}
/*

(<{'mpris:artUrl':
<'https://i.scdn.co/image/ab67616d0000b27399cb9d3da88aa1747d72fae8'>,
'mpris:length': <int64 249273000>, 'mpris:trackid': <objectpath
'/org/ncspot/spotify/track/0R8JLNP107Hr7V7lL9oh13'>, 'xesam:album':
<'ミラーチューン'>, 'xesam:albumArtist': <['ZUTOMAYO']>, 'xesam:artist':
<['ZUTOMAYO']>, 'xesam:discNumber': <1>, 'xesam:title': <'ミラーチューン'>,
'xesam:trackNumber': <1>, 'xesam:url':
<'https://open.spotify.com/track/0R8JLNP107Hr7V7lL9oh13'>, 'xesam:userRating':
<0.0>}>,)
*/
void MprisMediaPlayer::get_metadata() {

  DBusMessage *reply;
  DBusMetadata metadata;

  if (execute_base_property_func(Getter, Metadata, reply, &metadata) !=
      ERROR_NONE)
    return;

  read_reply(reply, &metadata);

  if (reply == nullptr)
    dbus_message_unref(reply);

  // metadata.print();

  return;
}

void MprisMediaPlayer::next() { execute_base_method_func(Next); }
void MprisMediaPlayer::pause() { execute_base_method_func(Pause); }
void MprisMediaPlayer::play() { execute_base_method_func(Play); }
void MprisMediaPlayer::play_pause() { execute_base_method_func(PlayPause); }
void MprisMediaPlayer::previous() { execute_base_method_func(Previous); }
// void MprisMediaPlayer::seek(int64_t offset);
//  void set_position(track, int64_t position);
void MprisMediaPlayer::stop() { execute_base_method_func(Stop); }

void MprisMediaPlayer::test_menu() {

  bool run_menu = true;
  int choice;

  std::string menu_properties_str = "\n\n1) CanControl\n"
                                    "2) CanGoNext\n"
                                    "3) CanGoPrevious\n"
                                    "4) CanPause\n"
                                    "5) CanPlay\n"
                                    "6) CanSeek\n"
                                    "7) PlayPause\n"
                                    "8) Shuffle\n"
                                    "9) set Shuffle\n"
                                    "0) Exit\n";

  while (run_menu) {
    std::cout << menu_properties_str;
    std::cout << "Enter choice: ";

    std::cin >> choice;

    switch (choice) {
    case 1:
      can_control();
      break;
    case 2:
      can_go_next();
      break;
    case 3:
      can_go_previous();
      break;
    case 4:
      can_pause();
      break;
    case 5:
      can_play();
      break;
    case 6:
      can_seek();
      break;
    case 7:
      play_pause();
      break;
    case 8:
      get_shuffle();
      break;
    case 9:
      get_volume();
      set_volume(0.8);
      get_volume();
      break;
    case 0:
      run_menu = false;
      break;
    default:
      break;
    }
  }
}
