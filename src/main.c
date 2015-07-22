#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer, *s_date_layer;
static GFont s_time_font, s_date_font;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;


static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
  
  int hour = tick_time->tm_hour;
  // int minute = tick_time->tm_min;
  
  // Create a long-lived buffer
  static char buffer[5];
  static char date_buffer[] = "00/00/00";

  // Write the current hours and minutes into the buffer
    if(clock_is_24h_style() == true) {
        //Use 24h hour format
        strftime(buffer, sizeof(buffer), "%H:%M", tick_time);
    } else {
        //Use 12 hour format
        static char min_buffer[] = "00";
      if(hour > 12){
        hour -= 12;
      }
        strftime(min_buffer, sizeof(min_buffer), "%M", tick_time);
        snprintf(buffer, sizeof(buffer), "%d:%s", hour, min_buffer);

    }
  
  // format the way that the time text is going to be displayed
  //strftime(buffer, sizeof("00:00"),"%I:%M", tick_time);
strftime(date_buffer, sizeof("00/00/00"), "%D/%Z/%M", tick_time);
  


  // Display this time and date on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
  text_layer_set_text(s_date_layer, date_buffer);

}

static void main_window_load(Window *window) {
  // Create GBitmap, then set to created BitmapLayer
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_Venom1);
  s_background_layer = bitmap_layer_create(GRect(0, -15, 144, 180));
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
  
  // Create time TextLayer
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_Reckoner_30));
  s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_Reckoner_30));

  //Time Layer
  s_time_layer = text_layer_create(GRect(5, 130, 120, 40));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text(s_time_layer, "00:00");
  
  //Date Layer
  s_date_layer = text_layer_create(GRect(20, 130, 120, 40));
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer, GColorBlack);
  text_layer_set_text(s_date_layer, "00/00/00");

  // Improve the layout to be more like a watchface
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentLeft);

  text_layer_set_font(s_date_layer, s_date_font);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentRight);
  
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
}

static void main_window_unload(Window *window) {
    
    // Unload GFont
    fonts_unload_custom_font(s_time_font);
    fonts_unload_custom_font(s_date_font);
    // Destroy time layer
    text_layer_destroy(s_time_layer);
    text_layer_destroy(s_date_layer);
    
}

//register all events
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
 update_time();
 
}

  


static void init() {
 
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Make sure the time is displayed from the start
  update_time();
}

static void deinit() {
    // Destroy Window
    window_destroy(s_main_window);
}


//this is the main road map for the app  first it will initialize by running init(), then it will run the app until
//the user exits the app and then will run the deinit() code, which will free up all of the memory that we 
//set aside at the for the app.
int main(void) {
  init();
  app_event_loop();
  deinit();
}