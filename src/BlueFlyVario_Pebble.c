#include <pebble.h>

//UI Elements
static Window *window;
static TextLayer *altitude_layer;
static TextLayer *vario_layer;
static TextLayer *speed_layer;
static TextLayer *time_layer;

const int inbound_size = 1024;
const int outbound_size = 1024;

AppSync sync;
uint8_t sync_buffer[1024];

// Synchronized Fields
#define FIELD_DAMPED_ALTITUDE (0x1000 |  0)
#define FIELD_VARIO2          (0x1000 |  2)
#define FIELD_LOCATION_SPEED  (0x1000 | 10)
#define FIELD_FLIGHT_TIME     (0x1000 | 16)

static void sync_tuple_changed_callback(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context)
{
  if (key == FIELD_FLIGHT_TIME)
  {
    text_layer_set_text(time_layer, new_tuple->value->cstring);
  }
  if (key == FIELD_DAMPED_ALTITUDE)
  {
    text_layer_set_text(altitude_layer, new_tuple->value->cstring);
  }
  if (key == FIELD_VARIO2)
  {
    text_layer_set_text(vario_layer, new_tuple->value->cstring);
  }
  if (key == FIELD_LOCATION_SPEED)
  {
    text_layer_set_text(speed_layer, new_tuple->value->cstring);
  }
  if (key == FIELD_FLIGHT_TIME)
  {
    text_layer_set_text(time_layer, new_tuple->value->cstring);
  }
}

static void sync_error_callback(DictionaryResult dict_error, AppMessageResult app_message_error, void *context)
{
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Error Syncinc");
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  //Create Layout

  //Altitude
  altitude_layer = text_layer_create((GRect) { 
	  .origin = { 0, 0 }, 
	  .size = { bounds.size.w, 20 } }
  );
  text_layer_set_text_alignment(altitude_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(altitude_layer));

  //Vario
  vario_layer = text_layer_create((GRect) { 
	  .origin = { 0, 20 }, 
	  .size = { bounds.size.w, 20 } }
  );
  text_layer_set_text_alignment(vario_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(vario_layer));

  //Speed
  speed_layer = text_layer_create((GRect) { 
	  .origin = { 0, 40 }, 
	  .size = { bounds.size.w, 20 } }
  );
  text_layer_set_text_alignment(speed_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(speed_layer));

  //Time
  time_layer = text_layer_create((GRect) { 
	  .origin = { 0, 80 }, 
	  .size = { bounds.size.w, 20 } }
  );
  text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(time_layer));

}

static void window_unload(Window *window) {
  text_layer_destroy(altitude_layer);
  text_layer_destroy(vario_layer);
  text_layer_destroy(speed_layer);
  text_layer_destroy(time_layer);
}

static void init(void) {
  //Start Sync
  Tuplet initial_values[] = {
    TupletCString(FIELD_DAMPED_ALTITUDE, "---"),
    TupletCString(FIELD_VARIO2, "---"),
    TupletCString(FIELD_LOCATION_SPEED, "---"),
    TupletCString(FIELD_FLIGHT_TIME, "---")
  };

  //initialize send/recieve buffer
  app_message_open(inbound_size, outbound_size);


  //Use app_sync for sending/receiving data
  app_sync_init(&sync, sync_buffer, sizeof(sync_buffer),
              initial_values, ARRAY_LENGTH(initial_values),
              sync_tuple_changed_callback, sync_error_callback, NULL);


  //Create Window
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);

  app_sync_deinit(&sync);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
