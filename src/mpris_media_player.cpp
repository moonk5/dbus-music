#include "mpris_media_player.h"
#include "dbus/dbus-protocol.h"
#include <bits/types/struct_sched_param.h>
#include <new>

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
    return DBUS_ERROR;
  }

  if (!conn) {
    std::cerr << "Connection Null" << std::endl;
    return NULL_PTR;
  }

  is_connected = true;
  log.debug("Session(" + session_name + ") is connected");

  return NONE;
}

void MprisMediaPlayer::disconnect() {
  dbus_connection_unref(conn);
  is_connected = false;
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
    std::cout << "bool type" << std::endl;
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
                                             DbusMethodType type) {
  std::string iface = "org.mpris.MediaPlayer2.Player";
  std::string method;

  switch (type) {
  case Next:
    method = "Next";
    break;
  case OpenUri:
    method = "OpenUri";
    break;
  case Pause:
    method = "Pause";
    break;
  case Play:
    method = "Play";
    break;
  case PlayPause:
    method = "PlayPause";
    break;
  case Seek:
    method = "Seek";
    break;
  case SetPosition:
    method = "SetPosition";
    break;
  case Stop:
    method = "Stop";
    break;
  defaut:
    method = "Unknown";
    break;
  }

  if (method == "Unknown") {
    return UNKNOWN_TYPE;
  }

  msg = _dbus_msg_new_method_call(session_name, PATH, iface, method);
  if (!msg) {
    std::cerr << "Message Null" << std::endl;
    return NULL_PTR;
  }
  std::cout << "Method call message created." << std::endl;

  return NONE;
}

int MprisMediaPlayer::construct_new_dbus_msg(DBusMessage *&msg,
                                             DbusPropertyType type) {
  std::string iface = "org.freedesktop.DBus.Properties";
  std::string method = "Get";
  std::string param_iface_name = "org.mpris.MediaPlayer2.Player";
  std::string param_property_name;

  msg = _dbus_msg_new_method_call(session_name.c_str(), PATH.c_str(),
                                  iface.c_str(), method.c_str());
  if (!msg) {
    std::cerr << "Message Null" << std::endl;
    return NULL_PTR;
  }

  // Append arguments
  switch (type) {
  case CanControl:
    param_property_name = "CanControl";
    break;
  case CanGoNext:
    param_property_name = "CanGoNext";
    break;
  case CanGoPrevious:
    param_property_name = "CanGoPrevious";
    break;
  case CanPause:
    param_property_name = "CanPause";
    break;
  case CanPlay:
    param_property_name = "CanPlay";
    break;
  case CanSeek:
    param_property_name = "CanSeek";
    break;
  case LoopStatus:
    param_property_name = "LoopStatus";
    break;
  case MaximumRate:
    param_property_name = "MaximumRate";
    break;
  case Metadata:
    param_property_name = "Metadata";
    break;
  case MinimumRate:
    param_property_name = "MinimumRate";
    break;
  case PlaybackStatus:
    param_property_name = "PlaybackStatus";
    break;
  case Position:
    param_property_name = "Position";
    break;
  case Rate:
    param_property_name = "Rate";
    break;
  case Shuffle:
    param_property_name = "Shuffle";
    break;
  case Volume:
    param_property_name = "Volume";
    break;
  defaut:
    param_property_name = "Unknown";
    break;
  }

  if (param_property_name == "Unknown") {
    return UNKNOWN_TYPE;
  }

  std::cout << "| Parameters:\n|\t - Iface Name: " << param_iface_name
            << "\n|\t - Property Name: " << param_property_name << std::endl;

  dbus_message_append_args(msg, DBUS_TYPE_STRING, param_iface_name,
                           DBUS_TYPE_STRING, param_property_name,
                           DBUS_TYPE_INVALID);

  std::cout << "Method call message created." << std::endl;

  return NONE;
}

int MprisMediaPlayer::send_dbus_msg(DBusMessage *&msg) {
  if (msg == nullptr) {
    std::cout << "Message creation failed." << std::endl;
    return NULL_PTR;
  }

  // Set the no-reply flag
  dbus_message_set_no_reply(msg, true);

  // Send the message and flush the connection
  if (!dbus_connection_send(conn, msg, nullptr)) {
    std::cout << "Out of memory." << std::endl;
    return DBUS_ERROR;
  }

  return NONE;
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
    return DBUS_ERROR;
  }
  if (!reply) {
    std::cout << "Reply Null" << std::endl;
    dbus_message_unref(msg);
    return NULL_PTR;
  }
  std::cout << "Reply received." << std::endl;

  return NONE;
}

bool MprisMediaPlayer::can_control() {
  DBusError err;
  DBusMessage *msg;
  DBusMessageIter args;
  DBusMessage *reply;

  // Initialize the error
  dbus_error_init(&err);

  // Connect to the session bus
  if (!is_connected && !connect()) {
    return false;
  }

  // Create a new method call Message
  if (construct_new_dbus_msg(msg, Metadata) != ErrorCode::NONE) {
    return false;
  }

  if (send_dbus_msg_with_reply(msg, reply, err) != NONE) {
    return false;
  }

  // Read the reply
  if (!dbus_message_iter_init(reply, &args)) {
    std::cout << "Message has no arguments!" << std::endl;
  } else if (DBUS_TYPE_VARIANT != dbus_message_iter_get_arg_type(&args)) {
    std::cout << "Argument is not variant!" << std::endl;
  } else {

    DBusMessageIter variant_iter;
    dbus_message_iter_recurse(&args, &variant_iter);

    if (DBUS_TYPE_ARRAY == dbus_message_iter_get_arg_type(&variant_iter)) {

      DBusMessageIter dict_iter;
      dbus_message_iter_recurse(&variant_iter, &dict_iter);

      while (dbus_message_iter_get_arg_type(&dict_iter) != DBUS_TYPE_INVALID) {
        if (DBUS_TYPE_DICT_ENTRY ==
            dbus_message_iter_get_arg_type(&dict_iter)) {
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
    } else if (DBUS_TYPE_BOOLEAN ==
               dbus_message_iter_get_arg_type(&variant_iter)) {

      print_dbus_variant(&variant_iter);
    }
  }

  // Clean up
  dbus_message_unref(reply);
  dbus_message_unref(msg);
  disconnect();
  std::cout << "Cleanup done." << std::endl;

  return true;
}

void MprisMediaPlayer::get_metadata(const std::string &service_name) {
  DBusError err;
  DBusMessage *msg;
  DBusMessageIter args;
  DBusMessage *reply;

  // Initialize the error
  dbus_error_init(&err);

  // Connect to the session bus
  std::cout << "Connecting to the D-Bus session bus..." << std::endl;
  if (!connect()) {
    return;
  }
  std::cout << "Successfully connected to the D-Bus session bus." << std::endl;

  // Create a new method call message
  std::cout << "Creating a new method call message..." << std::endl;
  msg = dbus_message_new_method_call(
      service_name.c_str(),              // Target service
      "/org/mpris/MediaPlayer2",         // Object to call on
      "org.freedesktop.DBus.Properties", // Interface to call on
      "Get"                              // Method name
  );
  if (!msg) {
    std::cerr << "Message Null" << std::endl;
    return;
  }
  std::cout << "Method call message created." << std::endl;

  // Append arguments
  const char *interface_name = "org.mpris.MediaPlayer2.Player";
  const char *property_name = "Metadata";
  dbus_message_append_args(msg, DBUS_TYPE_STRING, &interface_name,
                           DBUS_TYPE_STRING, &property_name, DBUS_TYPE_INVALID);
  std::cout << "Arguments appended to the message." << std::endl;

  // Send the message and get a reply
  std::cout << "Sending the message and waiting for a reply..." << std::endl;
  reply = dbus_connection_send_with_reply_and_block(conn, msg, -1, &err);
  if (dbus_error_is_set(&err)) {
    log.error(get_dbus_error("Error in Send Message", &err));
    dbus_message_unref(msg);
    return;
  }
  if (!reply) {
    std::cerr << "Reply Null" << std::endl;
    dbus_message_unref(msg);
    return;
  }
  std::cout << "Reply received." << std::endl;

  // Read the reply
  if (!dbus_message_iter_init(reply, &args)) {
    std::cerr << "Message has no arguments!" << std::endl;
  } else if (DBUS_TYPE_VARIANT != dbus_message_iter_get_arg_type(&args)) {
    std::cerr << "Argument is not variant!" << std::endl;
  } else {
    DBusMessageIter variant_iter;
    dbus_message_iter_recurse(&args, &variant_iter);

    if (DBUS_TYPE_ARRAY == dbus_message_iter_get_arg_type(&variant_iter)) {
      DBusMessageIter dict_iter;
      dbus_message_iter_recurse(&variant_iter, &dict_iter);

      while (dbus_message_iter_get_arg_type(&dict_iter) != DBUS_TYPE_INVALID) {
        if (DBUS_TYPE_DICT_ENTRY ==
            dbus_message_iter_get_arg_type(&dict_iter)) {
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
  }

  // Clean up
  dbus_message_unref(reply);
  dbus_message_unref(msg);
  dbus_connection_unref(conn);
  std::cout << "Cleanup done." << std::endl;
}

bool MprisMediaPlayer::can_go_next() { return true; }
bool MprisMediaPlayer::can_go_previous() { return true; }
bool MprisMediaPlayer::can_pause() { return true; }
bool MprisMediaPlayer::can_play() { return true; }
bool MprisMediaPlayer::can_seek() { return true; }

void MprisMediaPlayer::play_pause() {
  DBusError err;
  DBusMessage *msg;

  // Initialize the error
  dbus_error_init(&err);

  // Connect to the session bus
  if (!is_connected && !connect()) {
    return;
  }

  // Create a new method call Message
  if (construct_new_dbus_msg(msg, PlayPause) != ErrorCode::NONE) {
    return;
  }

  if (send_dbus_msg(msg) != NONE) {
    return;
  }

  // Clean up
  dbus_message_unref(msg);
  disconnect();
  std::cout << "Cleanup done." << std::endl;
}

void MprisMediaPlayer::test_menu() {

  bool run_menu = true;
  int choice;

  std::string menu_properties_str = "1) CanControl\n"
                                    "2) CanGoNext\n"
                                    "3) CanGoPrevious\n"
                                    "4) CanPause\n"
                                    "5) CanPlay\n"
                                    "6) CanSeek\n"
                                    "7) PlayPause\n"
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
    case 0:
      run_menu = false;
      break;
    default:
      break;
    }
  }
}
