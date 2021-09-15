#include <dbus/dbus.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

dbus_uint32_t inhibit()
{
  const char *application_name = "application_name";
  const char *reason_for_inhibit = "reason_for_inhibit";
  DBusError err;
  DBusConnection *conn;
  DBusMessage *msg;
  DBusPendingCall *pending;
  DBusMessageIter args;
  dbus_uint32_t cookie;

  // error
  dbus_error_init(&err);

  // conn
  conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
  if (dbus_error_is_set(&err))
  {
    fprintf(stderr, "connection error: %s\n", err.message);
    dbus_error_free(&err);
    exit(1);
  }
  if (NULL == conn)
  {
    exit(1);
  }

  // message
  msg = dbus_message_new_method_call(
      "org.freedesktop.ScreenSaver",
      "/org/freedesktop/ScreenSaver",
      "org.freedesktop.ScreenSaver",
      "Inhibit");
  if (NULL == msg)
  {
    fprintf(stderr, "message null\n");
    exit(1);
  }

  // add arguments
  dbus_message_iter_init_append(msg, &args);
  if (!dbus_message_iter_append_basic(
          &args, DBUS_TYPE_STRING, &application_name))
  {
    fprintf(stderr, "add argument error\n");
    exit(1);
  }
  if (!dbus_message_iter_append_basic(
          &args, DBUS_TYPE_STRING, &reason_for_inhibit))
  {
    fprintf(stderr, "add argument error\n");
    exit(1);
  }

  // send
  if (!dbus_connection_send_with_reply(conn, msg, &pending, -1))
  {
    fprintf(stderr, "send message error\n");
    exit(1);
  }
  if (NULL == pending)
  {
    fprintf(stderr, "pending null\n");
    exit(1);
  }
  dbus_connection_flush(conn);

  // free message
  dbus_message_unref(msg);

  // wait reply
  dbus_pending_call_block(pending);

  // replay message
  msg = dbus_pending_call_steal_reply(pending);
  if (NULL == msg)
  {
    fprintf(stderr, "reply null\n");
    exit(1);
  }

  // free pending
  dbus_pending_call_unref(pending);

  // read reply arguments
  if (!dbus_message_iter_init(msg, &args))
  {
    fprintf(stderr, "message has no arguments\n");
    exit(1);
  }
  else if (DBUS_TYPE_UINT32 != dbus_message_iter_get_arg_type(&args))
  {
    fprintf(stderr, "argument is not int\n");
    exit(1);
  }
  else
  {
    dbus_message_iter_get_basic(&args, &cookie);
  }

  return cookie;
}

void uninhibit(dbus_uint32_t cookie)
{
  DBusError err;
  DBusConnection *conn;
  DBusMessage *msg;
  DBusPendingCall *pending;
  DBusMessageIter args;

  dbus_error_init(&err);

  // conn
  conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
  if (dbus_error_is_set(&err))
  {
    fprintf(stderr, "connection error: %s\n", err.message);
    dbus_error_free(&err);
    exit(1);
  }
  if (NULL == conn)
  {
    exit(1);
  }

  // message
  msg = dbus_message_new_method_call(
      "org.freedesktop.ScreenSaver",
      "/org/freedesktop/ScreenSaver",
      "org.freedesktop.ScreenSaver",
      "UnInhibit");
  if (NULL == msg)
  {
    fprintf(stderr, "message null\n");
    exit(1);
  }

  // add arguments
  dbus_message_iter_init_append(msg, &args);
  if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_UINT32, &cookie))
  {
    fprintf(stderr, "add argument error\n");
    exit(1);
  }

  // send
  if (!dbus_connection_send_with_reply(conn, msg, &pending, -1))
  {
    fprintf(stderr, "add argument error\n");
    exit(1);
  }
  if (NULL == pending)
  {
    fprintf(stderr, "pending null\n");
    exit(1);
  }
  dbus_connection_flush(conn);

  // free message
  dbus_message_unref(msg);

  // wait reply
  dbus_pending_call_block(pending);
  dbus_pending_call_unref(pending);
}

int main()
{
  dbus_uint32_t cookie = inhibit();
  printf("screensaver inhibited\n");

  printf("press any key to uninhibit screensaver");
  getchar();
  uninhibit(cookie);

  printf("press any key to quit");
  getchar();
  return 0;
}