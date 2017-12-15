#include <pebble.h>

#define APP_NAME "stime"
static Window *s_window;

static TextLayer *s_text_layer;

// Write message to buffer & send
static void send_message(const char *str) {
  DictionaryIterator *iter;

  app_message_outbox_begin(&iter);
  dict_write_cstring(iter, MESSAGE_KEY_message, str);
  dict_write_end(iter);
  app_message_outbox_send();
}

static int handle_state_change(int state) {
  static int orig_state = 0;
  switch (state) {
    case 1: {
      send_message("query message");
      break;
    }
    default:
    break;
  }
  orig_state = state;
  return orig_state;
}

// Called when a message is received from PebbleKitJS
static void in_received_handler(DictionaryIterator *received, void *context) {
  Tuple *tuple;
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Received something");

  if ((tuple = dict_find(received, MESSAGE_KEY_status))) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Received Status: %d", (int)tuple->value->uint32);
    handle_state_change((int)tuple->value->uint32);
  } else if ((tuple = dict_find(received, MESSAGE_KEY_message))) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Received Message: %s", tuple->value->cstring);
    text_layer_set_text(s_text_layer, tuple->value->cstring);
  }
}

// Called when an incoming message from PebbleKitJS is dropped
static void in_dropped_handler(AppMessageResult reason, void *context) {
}

// Called when PebbleKitJS does not acknowledge receipt of a message
static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
}

static void init(void) {
  s_window = window_create();
  Layer *window_layer = window_get_root_layer(s_window);
  GRect bounds = layer_get_bounds(window_layer);
  s_text_layer = text_layer_create(bounds);
  text_layer_set_text(s_text_layer, "STime");
  text_layer_set_font(s_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	text_layer_set_text_alignment(s_text_layer, GTextAlignmentCenter);

  // Add the text layer to the window
  layer_add_child(window_get_root_layer(s_window), text_layer_get_layer(s_text_layer));

  // Enable text flow and paging on the text layer, with a slight inset of 10, for round screens
  text_layer_enable_screen_text_flow_and_paging(s_text_layer, 10);
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
	text_layer_destroy(s_text_layer);
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
