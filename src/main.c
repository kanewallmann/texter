#include <pebble.h>
#include "loading_window.h"
#include "select_window.h"
#include "message_sent_window.h"
#include "constants.h"
	
static Window* menu_window;
static MenuLayer* menu;
static SelectWindow* contact_list = NULL;

// Setup the current path
char* current_path = path;

void home_select_handler( SelectItem* item, unsigned short index )
{	
		if( index == 0 || index == 2 )		
		{
			strcpy( address, item->value );
			
			DictionaryIterator *iter;
			app_message_outbox_begin(&iter);
			dict_write_uint8( iter, 0, ACTION_SELECT_CONTACT );
			app_message_outbox_send();
	
			show_loading_window();
		}
		else if( index == 3 )		
		{
			DictionaryIterator *iter;
			app_message_outbox_begin(&iter);
			dict_write_uint8( iter, 0, ACTION_RECENT );
			app_message_outbox_send();
			
			show_loading_window();
		}
		else if( index == 5 )		
		{
			DictionaryIterator *iter;
			app_message_outbox_begin(&iter);
			dict_write_uint8( iter, 0, ACTION_FAVOURITE );
			app_message_outbox_send();
			
			show_loading_window();
		}
		
		APP_LOG(APP_LOG_LEVEL_INFO, "Selected contact %s", item->value );
}

void contact_select_handler( SelectItem* item, unsigned short index )
{	
		if( strcmp( item->title, "" ) == 0 )	
			return;
	
		strcpy( address, item->value );
			
		DictionaryIterator *iter;
		app_message_outbox_begin(&iter);
		dict_write_uint8( iter, 0, ACTION_SELECT_CONTACT );
		app_message_outbox_send();

		show_loading_window();
}

void navigate_select_handler( SelectItem* item, unsigned short index )
{
		if( strcmp( item->title, "-" ) == 0 )
			return;
		
		APP_LOG(APP_LOG_LEVEL_INFO, "Current path %s", path );
	
		*current_path = index + '0';
		current_path++;
		*current_path = 0;
	
		APP_LOG(APP_LOG_LEVEL_INFO, "New path %s", path );
	
		show_loading_window();
	
	  DictionaryIterator *iter;
		app_message_outbox_begin(&iter);
		dict_write_uint8( iter, 0, ACTION_NAVIGATE );
		dict_write_cstring( iter, 1, path );
		dict_write_cstring( iter, 2, address );
		app_message_outbox_send();
	
		APP_LOG(APP_LOG_LEVEL_INFO, "Navigate %s", path );
}

static void inbox_received_callback(DictionaryIterator* iterator, void* context)
{
  APP_LOG(APP_LOG_LEVEL_INFO, "Message received!");
	
	// Get the action id
  Tuple* t = dict_find( iterator, 0 );
	unsigned char action = t->value->uint8;
	
	APP_LOG( APP_LOG_LEVEL_INFO, "Received message with action id %i", action );
	
	if( action == ACTION_HOME_DATA )
	{
		SelectWindow* select_window = select_window_create();
		
		select_window->handler = home_select_handler;
		
		t = dict_find( iterator, 1 ); strcpy( select_window->item_up.title, t->value->cstring );
		t = dict_find( iterator, 2 ); strcpy( select_window->item_up.value, t->value->cstring );
		t = dict_find( iterator, 3 ); strcpy( select_window->item_down.title, t->value->cstring );
		t = dict_find( iterator, 4 ); strcpy( select_window->item_down.value, t->value->cstring );

		strcpy( select_window->item_long_up.title, "+ Recent" );
		strcpy( select_window->item_long_down.title, "+ Favourite" );
		
		select_window_refresh( select_window );
		
		hide_loading_window();
						
		APP_LOG( APP_LOG_LEVEL_INFO, "Received selection" );
	}
	else if( action == ACTION_CONTACTS )
	{
		contact_list = select_window_create();
		
		contact_list->handler = contact_select_handler;
		
		t = dict_find( iterator, 1 ); strcpy( contact_list->item_up.title, t->value->cstring );
		t = dict_find( iterator, 2 ); strcpy( contact_list->item_up.value, t->value->cstring );
		t = dict_find( iterator, 3 ); strcpy( contact_list->item_select.title, t->value->cstring );
		t = dict_find( iterator, 4 ); strcpy( contact_list->item_select.value, t->value->cstring );
		t = dict_find( iterator, 5 ); strcpy( contact_list->item_down.title, t->value->cstring );
		t = dict_find( iterator, 6 ); strcpy( contact_list->item_down.value, t->value->cstring );
		
		contact_list->item_long_up.title[0] = 0;
		contact_list->item_long_select.title[0] = 0;
		contact_list->item_long_down.title[0] = 0;
		
		select_window_refresh( contact_list );
		
		hide_loading_window();
						
		APP_LOG( APP_LOG_LEVEL_INFO, "Received contacts" );
	}
	else if( action == ACTION_CONTACTS_EXTRA )
	{
		if( contact_list != NULL )
		{
			t = dict_find( iterator, 1 ); strcpy( contact_list->item_long_up.title, t->value->cstring );
			t = dict_find( iterator, 2 ); strcpy( contact_list->item_long_up.value, t->value->cstring );
			t = dict_find( iterator, 3 ); strcpy( contact_list->item_long_select.title, t->value->cstring );
			t = dict_find( iterator, 4 ); strcpy( contact_list->item_long_select.value, t->value->cstring );
			t = dict_find( iterator, 5 ); strcpy( contact_list->item_long_down.title, t->value->cstring );
			t = dict_find( iterator, 6 ); strcpy( contact_list->item_long_down.value, t->value->cstring );

			select_window_refresh( contact_list );

			APP_LOG( APP_LOG_LEVEL_INFO, "Received contact extra" );
		}
	}
	else if( action == ACTION_MESSAGE_BRANCH )
	{
		SelectWindow* select_window = select_window_create();
		select_window->handler = navigate_select_handler;
		
		APP_LOG( APP_LOG_LEVEL_INFO, "Created window" );
		
		t = dict_find( iterator, 1 ); strcpy( select_window->item_up.title, t->value->cstring );
		t = dict_find( iterator, 2 ); strcpy( select_window->item_select.title, t->value->cstring );
		t = dict_find( iterator, 3 ); strcpy( select_window->item_down.title, t->value->cstring );
		
		t = dict_find( iterator, 4 ); strcpy( select_window->item_long_up.title, t->value->cstring );
		t = dict_find( iterator, 5 ); strcpy( select_window->item_long_select.title, t->value->cstring );
		t = dict_find( iterator, 6 ); strcpy( select_window->item_long_down.title, t->value->cstring );
		
		select_window_refresh( select_window );
		
		APP_LOG( APP_LOG_LEVEL_INFO, "Refreshed list" );
				
		hide_loading_window();
		
		APP_LOG( APP_LOG_LEVEL_INFO, "Hide loading window" );
	}
	else if( action == ACTION_MESSAGE_SENT )
	{
		// Close all other windows
		window_stack_pop_all( false );
		
		// Copy the person's name and message into our buffer 
		t = dict_find( iterator, 1 );	strcpy( person, t->value->cstring );
		t = dict_find( iterator, 2 );	strcpy( message, t->value->cstring );
		
		// Display the message sent window for some time
		show_message_sent_window( person, message );
	}
}

static void inbox_dropped_callback(AppMessageResult reason, void *context)
{
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

void handle_init(void)
{
	APP_LOG(APP_LOG_LEVEL_INFO, "Handling init" );
			
	// Register for messages from phone app
	app_message_register_inbox_received( inbox_received_callback );
	app_message_register_inbox_dropped( inbox_dropped_callback );
	app_message_open( app_message_inbox_size_maximum(), app_message_outbox_size_maximum() );
	
	APP_LOG(APP_LOG_LEVEL_INFO, "AppMessage callback registered" );
	
	show_loading_window();
	
	// Ask for something to show
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  dict_write_uint8( iter, 0, ACTION_SHOW_HOME );
  app_message_outbox_send();
	
}

void handle_deinit(void)
{
  menu_layer_destroy(menu);
  window_destroy(menu_window);
}

int main(void)
{
	APP_LOG(APP_LOG_LEVEL_INFO, "App started" );
	
	*current_path = 0;
	
  handle_init();
  app_event_loop();
  handle_deinit();
}
