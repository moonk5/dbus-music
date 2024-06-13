#include "mpris_media_player.h"

const std::string MprisMediaPlayer::PATH = "/org/mpris/MediaPlayer2";

MprisMediaPlayer::MprisMediaPlayer(const std::string &session_name)
    : bus_name(session_name) {

  log = Log("MprisMediaPlayer");
  dbus_error_init(&err);
}

int MprisMediaPlayer::connect() {
  conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
  if (dbus_error_is_set(&err)) {
    log.error(get_dbus_error("Connection Error", &err));
    return DBUS_ERROR; // FIXME: error code handling?
  }

  if (!conn) {
    std::cerr << "Connection Null" << std::endl;
    return NULL_PTR;
  }

  log.debug("Session(" + bus_name + ") is connected");

  return SUCCESS;
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
  case DBUS_TYPE_STRING: {
    char *value;
    dbus_message_iter_get_basic(iter, &value);
    std::cout << value;
    break;
  }
  case DBUS_TYPE_INT32: {
    int32_t value;
    dbus_message_iter_get_basic(iter, &value);
    std::cout << value;
    break;
  }
  case DBUS_TYPE_UINT32: {
    uint32_t value;
    dbus_message_iter_get_basic(iter, &value);
    std::cout << value;
    break;
  }
  case DBUS_TYPE_INT64: {
    int64_t value;
    dbus_message_iter_get_basic(iter, &value);
    std::cout << value;
    break;
  }
  case DBUS_TYPE_UINT64: {
    uint64_t value;
    dbus_message_iter_get_basic(iter, &value);
    std::cout << value;
    break;
  }
  case DBUS_TYPE_DOUBLE: {
    double value;
    dbus_message_iter_get_basic(iter, &value);
    std::cout << value;
    break;
  }
  case DBUS_TYPE_OBJECT_PATH: {
    char *value;
    dbus_message_iter_get_basic(iter, &value);
    std::cout << value;
    break;
  }
  case DBUS_TYPE_ARRAY: {
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
