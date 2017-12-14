#include <pebble.h>

#define APP_NAME "stime"
static Window *s_window;

// Write message to buffer & send
static void send_message(void) {
  DictionaryIterator *iter;

  app_message_outbox_begin(&iter);
  dict_write_cstring(iter, MESSAGE_KEY_message, "I'm a Pebble!");

  dict_write_end(iter);
  app_message_outbox_send();
}

// Called when a message is received from PebbleKitJS
static void in_received_handler(DictionaryIterator *received, void *context) {
  Tuple *tuple;
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Received something");

  tuple = dict_find(received, MESSAGE_KEY_status);
  if(tuple) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Received Status: %d", (int)tuple->value->uint32);
  }

  tuple = dict_find(received, MESSAGE_KEY_message);
  if(tuple) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Received Message: %s", tuple->value->cstring);
  }

  send_message();
}

// Called when an incoming message from PebbleKitJS is dropped
static void in_dropped_handler(AppMessageResult reason, void *context) {
}

// Called when PebbleKitJS does not acknowledge receipt of a message
static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
}

static void init(void) {
  s_window = window_create();
  window_stack_push(s_window, true);

  // Register AppMessage handlers
  app_message_register_inbox_received(in_received_handler);
  app_message_register_inbox_dropped(in_dropped_handler);
  app_message_register_outbox_failed(out_failed_handler);

  // Initialize AppMessage inbox and outbox buffers with a suitable size
  const int inbox_size = 128;
  const int outbox_size = 128;
  app_message_open(inbox_size, outbox_size);
}

static void deinit(void) {
  app_message_deregister_callbacks();
  window_destroy(s_window);
}

int main( void ) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, APP_NAME " start");
  init();
  APP_LOG(APP_LOG_LEVEL_DEBUG, APP_NAME " inited");
  app_event_loop();
  APP_LOG(APP_LOG_LEVEL_DEBUG, APP_NAME " cleanup");
  deinit();
  APP_LOG(APP_LOG_LEVEL_DEBUG, APP_NAME " quit");
}
