#include "message_sent_window.h"

	
// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static GFont s_res_gothic_14;
static TextLayer *text_address;
static TextLayer *text_message;
static TextLayer *s_textlayer_1;

static void initialise_ui(void) {
  s_window = window_create();
  window_set_background_color(s_window, GColorBlack);
  window_set_fullscreen(s_window, true);
  
  s_res_gothic_14 = fonts_get_system_font(FONT_KEY_GOTHIC_14);
  // text_address
  text_address = text_layer_create(GRect(0, 82, 144, 20));
  text_layer_set_background_color(text_address, GColorClear);
  text_layer_set_text_color(text_address, GColorWhite);
  text_layer_set_text(text_address, "-");
  text_layer_set_text_alignment(text_address, GTextAlignmentCenter);
  text_layer_set_font(text_address, s_res_gothic_14);
  layer_add_child(window_get_root_layer(s_window), (Layer *)text_address);
  
  // text_message
  text_message = text_layer_create(GRect(0, 99, 144, 34));
  text_layer_set_background_color(text_message, GColorClear);
  text_layer_set_text_color(text_message, GColorWhite);
  text_layer_set_text(text_message, "-");
  text_layer_set_text_alignment(text_message, GTextAlignmentCenter);
  text_layer_set_font(text_message, s_res_gothic_14);
  layer_add_child(window_get_root_layer(s_window), (Layer *)text_message);
  
  // s_textlayer_1
  s_textlayer_1 = text_layer_create(GRect(0, 57, 144, 20));
  text_layer_set_background_color(s_textlayer_1, GColorClear);
  text_layer_set_text_color(s_textlayer_1, GColorWhite);
  text_layer_set_text(s_textlayer_1, "Message Sent");
  text_layer_set_text_alignment(s_textlayer_1, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_textlayer_1);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  text_layer_destroy(text_address);
  text_layer_destroy(text_message);
  text_layer_destroy(s_textlayer_1);
}
// END AUTO-GENERATED UI CODE

static void handle_window_unload(Window* window) {
  destroy_ui();
}

void message_sent_timer( void* data )
{
	hide_message_sent_window();	
}

void show_message_sent_window( const char* person, const char* message, const bool success ) {
  initialise_ui();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
	
	APP_LOG(APP_LOG_LEVEL_INFO, "Showing message sent window %s %s", person, message );
			
	if( success )
	{
		text_layer_set_text(s_textlayer_1, "Message Sent");
	}
	else
	{
		text_layer_set_text(s_textlayer_1, "Message Failed");	
	}
	
	text_layer_set_text( text_address, person );	
	text_layer_set_text( text_message, message );	
	
	layer_mark_dirty( window_get_root_layer( s_window ) );
	
  window_stack_push(s_window, true);
	
	app_timer_register( 3000, message_sent_timer, NULL );
}

void hide_message_sent_window(void) {
  window_stack_remove(s_window, true);
}
