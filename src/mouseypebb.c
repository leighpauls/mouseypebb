#include <pebble.h>

static Window *window;
static TextLayer *text_layer;

#define BUFFER_SIZE 32
static char string_buffer[BUFFER_SIZE];

enum {
  ACCEL_X_POS = 1,
  ACCEL_Y_POS = 2,
  LEFT_MOUSE_STATE = 10,
  MIDDLE_MOUSE_STATE = 11,
  RIGHT_MOUSE_STATE = 12,
};

static void timer_callback(void* cb_data) {
  AccelData data;
  accel_service_peek(&data);
  snprintf(string_buffer, BUFFER_SIZE, "%d,%d", data.x, data.y);
  text_layer_set_text(text_layer, string_buffer);

  DictionaryIterator *it;
  app_message_outbox_begin(&it);

  Tuplet x_pos = TupletInteger(ACCEL_X_POS, data.x);
  dict_write_tuplet(it, &x_pos);

  Tuplet y_pos = TupletInteger(ACCEL_Y_POS, data.y);
  dict_write_tuplet(it, &y_pos);

  app_message_outbox_send();

  app_timer_register(100, timer_callback, NULL);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  app_timer_register(100, timer_callback, NULL);
  // text_layer_set_text(text_layer, "Select");
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  DictionaryIterator *it;
  app_message_outbox_begin(&it);

  Tuplet left_mouse = TupletInteger(LEFT_MOUSE_STATE, 1);
  dict_write_tuplet(it, &left_mouse);

  app_message_outbox_send();
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Down");
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer = text_layer_create((GRect) { .origin = { 0, 72 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text(text_layer, "Press a button");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
}

static void handle_accel(AccelData* accel_data, uint32_t num_samples) {}

static void out_sent_handler(DictionaryIterator *sent, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Sent");
}
static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Failed");
}
static void in_received_handler(DictionaryIterator *received, void *context) {}
static void in_dropped_handler(AppMessageResult reason, void *context) {}

static void init(void) {
  app_message_register_inbox_received(in_received_handler);
  app_message_register_inbox_dropped(in_dropped_handler);
  app_message_register_outbox_sent(out_sent_handler);
  app_message_register_outbox_failed(out_failed_handler);

  const unsigned int inbound_size = 640;
  const unsigned int outbound_size = 640;
  app_message_open(inbound_size, outbound_size);

  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);

  accel_service_set_sampling_rate(ACCEL_SAMPLING_10HZ);
  accel_data_service_subscribe(0, handle_accel);
}

static void deinit(void) {
  window_destroy(window);

  accel_data_service_unsubscribe();
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
