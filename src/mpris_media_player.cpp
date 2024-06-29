#include "mpris_media_player.h"
#include "dbus/dbus-protocol.h"
#include <bits/types/struct_sched_param.h>
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
  std::cout << "\n+------------------------------------------------------\n";
  std::cout << "| Destination: " << dest << "\n"
            << "| Path: " << path << "\n"
            << "| Interface: " << iface << "\n"
            << "| Method:" << method << "\n";
  std::cout << "+------------------------------------------------------\n";

  return dbus_message_new_method_call(dest.c_str(), path.c_str(), iface.c_str(),
                                      method.c_str());
}

int MprisMediaPlayer::construct_new_dbus_msg(DBusMethodType type,
                                             DBusMessage *&msg,
                                             void *set_value) {
  DBusMessageIter args;
  DBusMessageIter sub_iter;

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

  if (set_value) {
    dbus_message_iter_init_append(msg, &args);

    switch (type) {
    case Seek: {
      dbus_message_iter_append_basic(&args, DBUS_TYPE_INT64,
                                     static_cast<int64_t *>(set_value));
      break;
    }
    case SetPosition: {
      std::cerr << "Not Implemented" << std::endl;
      break;
    }
    case OpenUri: {
      std::cerr << "Not Implemented" << std::endl;
      break;
    }
    default:
      return ERROR_UNKNOWN_TYPE;
    } // end of switch (type)
  }

  std::cout << "Method call message created." << std::endl;

  return ERROR_NONE;
}

int MprisMediaPlayer::construct_new_dbus_msg(DBusPropertyType type,
                                             DBusMessage *&msg,
                                             void *set_value) {
  DBusMessageIter args;
  DBusMessageIter sub_iter;

  std::string iface = "org.freedesktop.DBus.Properties";
  std::string method = ((!set_value) ? "Get" : "Set");
  std::string param_iface_name = "org.mpris.MediaPlayer2.Player";
  std::string param_property_name;

  msg = _dbus_msg_new_method_call(session_name.c_str(), PATH.c_str(),
                                  iface.c_str(), method.c_str());
  if (!msg) {
    std::cerr << "Message Null" << std::endl;
    return ERROR_NULL_PTR;
  }

  // append arguments
  param_property_name = convert_dbus_property_type_to_string(type);
  if (param_property_name == "unknown") {
    return ERROR_UNKNOWN_TYPE;
  }

  std::cout << "| parameters:\n|\t - iface name: " << param_iface_name
            << "\n|\t - property name: " << param_property_name << std::endl;
  std::cout << "+------------------------------------------------------\n";

  dbus_message_iter_init_append(msg, &args);
  dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &param_iface_name);
  dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &param_property_name);

  // append the value variant
  if (set_value) {
    DBusLoopStatusType *loop_type;
    const char *loop_status_cstr;
    switch (type) {
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

void MprisMediaPlayer::execute_base_method_func(DBusMethodType type,
                                                void *set_value) {
  DBusMessage *msg;

  // Connect to the session bus
  if (!is_connected && !connect()) {
    return;
  }

  // Create a new method call Message
  if (construct_new_dbus_msg(type, msg, set_value) != ERROR_NONE) {
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

int MprisMediaPlayer::execute_base_property_func(DBusPropertyType type,
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
  if ((output = construct_new_dbus_msg(type, msg, set_value)) != ERROR_NONE) {
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
  DBusMessage *reply = nullptr;
  bool output = false;

  if (execute_base_property_func(type, reply) != ERROR_NONE)
    return output;

  if (reply != nullptr) {
    if (read_reply(reply, &output) == ERROR_NONE) {
      std::cout << convert_dbus_property_type_to_string(type) << ": "
                << ((output) ? "true" : "false") << std::endl;
    }
    dbus_message_unref(reply);
  }

  return output;
}

double MprisMediaPlayer::property_func_return_double(DBusPropertyType type) {
  DBusMessage *reply;
  double output = 0.0;

  if (execute_base_property_func(type, reply) != ERROR_NONE)
    return output;

  if (reply != nullptr) {
    if (read_reply(reply, &output) == ERROR_NONE) {
      std::cout << convert_dbus_property_type_to_string(type) << ": " << output
                << std::endl;
    }
    dbus_message_unref(reply);
  }

  return output;
}

void MprisMediaPlayer::fill_in_metadata(DBusMetadata &metadata, std::string key,
                                        DBusMessageIter *value_iter) {

  DBusMessageIter sub_iter;

  switch (metadata.get_keytype(key)) {
  case DBusMetadata::ArtUrl:
    char *artUrl;
    dbus_message_iter_get_basic(value_iter, &artUrl);
    metadata.art_url = artUrl;
    break;
  case DBusMetadata::Url:
    char *url;
    dbus_message_iter_get_basic(value_iter, &url);
    metadata.url = url;
    break;
  case DBusMetadata::TrackId:
    char *trackId;
    dbus_message_iter_get_basic(value_iter, &trackId);
    metadata.track_id = trackId;
    break;
  case DBusMetadata::AlbumArtist:
    dbus_message_iter_recurse(value_iter, &sub_iter);

    while (dbus_message_iter_get_arg_type(&sub_iter) != DBUS_TYPE_INVALID) {
      char *albumArtist;
      dbus_message_iter_get_basic(&sub_iter, &albumArtist);
      metadata.album_artist.push_back(albumArtist);

      dbus_message_iter_next(&sub_iter);
    }
    break;
  case DBusMetadata::Artist:
    dbus_message_iter_recurse(value_iter, &sub_iter);

    while (dbus_message_iter_get_arg_type(&sub_iter) != DBUS_TYPE_INVALID) {
      char *artist;
      dbus_message_iter_get_basic(&sub_iter, &artist);
      metadata.artist.push_back(artist);

      dbus_message_iter_next(&sub_iter);
    }
    break;
  case DBusMetadata::Album:
    char *album;
    dbus_message_iter_get_basic(value_iter, &album);
    metadata.album = album;
    break;
  case DBusMetadata::Title:
    char *title;
    dbus_message_iter_get_basic(value_iter, &title);
    metadata.title = title;
    break;
  case DBusMetadata::DiscNumber:
    int32_t discNum;
    dbus_message_iter_get_basic(value_iter, &discNum);
    metadata.disc_number = discNum;
    break;
  case DBusMetadata::TrackNumber:
    int32_t trackNum;
    dbus_message_iter_get_basic(value_iter, &trackNum);
    metadata.track_number = trackNum;
    break;
  case DBusMetadata::Length:
    int64_t length;
    dbus_message_iter_get_basic(value_iter, &length);
    metadata.length = length;
    break;
  case DBusMetadata::UserRating:
    double userRating;
    dbus_message_iter_get_basic(value_iter, &userRating);
    metadata.user_rating = userRating;
    break;
  }
}

int MprisMediaPlayer::read_reply(DBusMessage *reply, void *output) {
  DBusMessageIter args;

  if (!dbus_message_iter_init(reply, &args)) {
    std::cerr << "Message has no arguments!" << std::endl;
    return ERROR_DBUS;
  }

  if (DBUS_TYPE_VARIANT != dbus_message_iter_get_arg_type(&args)) {

    // NOTE: special case for list of session names
    if (DBUS_TYPE_ARRAY == dbus_message_iter_get_arg_type(&args)) {
      DBusMessageIter sub_iter;
      char *str;

      dbus_message_iter_recurse(&args, &sub_iter);
      while (dbus_message_iter_get_arg_type(&sub_iter) == DBUS_TYPE_STRING) {
        dbus_message_iter_get_basic(&sub_iter, &str);

        std::cout << str << ", ";
        // result.push_back(str);
        dbus_message_iter_next(&sub_iter);
      }
      std::cout << std::endl;
      return ERROR_NONE;
    }

    std::cerr << "Argument is not variant!" << std::endl;
    return ERROR_DBUS;
  }

  DBusMessageIter variant_iter;
  dbus_message_iter_recurse(&args, &variant_iter);

  auto arg_type = dbus_message_iter_get_arg_type(&variant_iter);

  switch (arg_type) {
  case DBUS_TYPE_BOOLEAN: {
    bool value;
    dbus_message_iter_get_basic(&variant_iter, &value);
    *static_cast<bool *>(output) = value;
    break;
  }
  case DBUS_TYPE_INT32: {
    int32_t value;
    dbus_message_iter_get_basic(&variant_iter, &value);
    *static_cast<int32_t *>(output) = value;
    break;
  }
  case DBUS_TYPE_INT64: {
    int64_t value;
    dbus_message_iter_get_basic(&variant_iter, &value);
    *static_cast<int64_t *>(output) = value;
    break;
  }
  case DBUS_TYPE_DOUBLE: {
    double value;
    dbus_message_iter_get_basic(&variant_iter, &value);
    *static_cast<double *>(output) = value;
    break;
  }
  case DBUS_TYPE_STRING: {
    const char *value;
    dbus_message_iter_get_basic(&variant_iter, &value);
    *static_cast<std::string *>(output) = value;
    break;
  }
  case DBUS_TYPE_ARRAY: {
    DBusMetadata meta;

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

            fill_in_metadata(meta, key, &value_iter);
          }
        }
      }
      dbus_message_iter_next(&dict_iter);
    }

    *static_cast<DBusMetadata *>(output) = meta;
    break;
  }
  } // end of switch (arg_type)

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
  if (execute_base_property_func(Shuffle, reply, &shuffle_on) != ERROR_NONE)
    return;

  if (reply != nullptr)
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
  if (execute_base_property_func(Volume, reply, &volume) != ERROR_NONE)
    return;

  if (reply != nullptr)
    dbus_message_unref(reply);

  return;
}

int64_t MprisMediaPlayer::get_position() {
  DBusMessage *reply;
  int64_t output = 0;

  if (execute_base_property_func(Position, reply) != ERROR_NONE)
    return output;

  read_reply(reply, &output);

  if (reply != nullptr)
    dbus_message_unref(reply);

  std::cout << "position: " << output << std::endl;

  return output;
}

std::string MprisMediaPlayer::get_loop_status() {
  DBusMessage *reply;
  std::string output;

  if (execute_base_property_func(LoopStatus, reply) != ERROR_NONE)
    return output;

  read_reply(reply, &output);

  if (reply != nullptr)
    dbus_message_unref(reply);

  std::cout << "loop status: " << output.c_str() << std::endl;

  return output;
}

void MprisMediaPlayer::set_loop_status(DBusLoopStatusType loop_status) {
  DBusMessage *reply;
  if (execute_base_property_func(LoopStatus, reply, &loop_status) != ERROR_NONE)
    return;

  if (reply != nullptr)
    dbus_message_unref(reply);

  return;
}

void MprisMediaPlayer::get_metadata(DBusMetadata &metadata) {
  DBusMessage *reply;

  if (execute_base_property_func(Metadata, reply) != ERROR_NONE)
    return;

  read_reply(reply, &metadata);

  if (reply != nullptr)
    dbus_message_unref(reply);

  return;
}

void MprisMediaPlayer::next() { execute_base_method_func(Next); }
void MprisMediaPlayer::pause() { execute_base_method_func(Pause); }
void MprisMediaPlayer::play() { execute_base_method_func(Play); }
void MprisMediaPlayer::play_pause() { execute_base_method_func(PlayPause); }
void MprisMediaPlayer::previous() { execute_base_method_func(Previous); }
void MprisMediaPlayer::seek(int64_t offset) {
  execute_base_method_func(Seek, &offset);
}

//  void set_position(track, int64_t position);
void MprisMediaPlayer::stop() { execute_base_method_func(Stop); }

void MprisMediaPlayer::test_menu() {

  DBusMetadata metadata;
  std::vector<std::string> sessions;
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
      // set_shuffle(true);
      // seek(-10000000);
      get_session_list(sessions);
      break;
    case 9:
      get_metadata(metadata);
      metadata.print();
      break;
    case 0:
      run_menu = false;
      break;
    default:
      break;
    }
  }
}

int MprisMediaPlayer::get_session_list(std::vector<std::string> &sessions) {
  DBusMessage *msg;
  DBusMessage *reply;
  DBusError err;
  int output = ERROR_NONE;

  std::string session = "org.freedesktop.DBus";
  std::string path = "/org/freedesktop/DBus";
  std::string iface = "org.freedesktop.DBus";
  std::string method = "ListNames";

  // Initialize the error
  dbus_error_init(&err);

  // Connect to the session bus
  if (!is_connected && ((output = connect()) != ERROR_NONE)) {
    return output;
  }

  msg = _dbus_msg_new_method_call(session, path, iface, method);
  if (!msg) {
    std::cerr << "Message Null" << std::endl;
    return ERROR_NULL_PTR;
  }

  if ((output = send_dbus_msg_with_reply(msg, reply, err)) != ERROR_NONE) {
    return output;
  }

  if (reply != nullptr) {
    if (read_reply(reply, &sessions) == ERROR_NONE) {
    }
    // dbus_message_unref(reply);
  }

  // Clean up
  dbus_message_unref(msg);
  disconnect();
  std::cout << "Cleanup done." << std::endl;

  return ERROR_NONE;
}
