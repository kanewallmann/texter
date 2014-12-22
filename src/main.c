#include <pebble.h>
#include "loading_window.h"
#include "select_window.h"
#include "message_sent_window.h"
#include "constants.h"
	
static SelectWindow* contact_list = NULL;
static SelectWindow* home_window = NULL;
static SelectWindow* combine_message_window = NULL;

// Setup the current path
char* current_path = path;
bool combine_messages = false;

// Options
bool combine_messages;

const char send_message[] = "Send Message";
const char combine_message[] = "Combine Message";
const char exit_message[] = "Exit Texter";

void home_select_handler( SelectItem* item, unsigned short index )
{	
		if( index == 0 || index == 1 || index == 2 )
		{
			if( strcmp( item->value, "" ) == 0 )
				return;
			
			strncpy( address, item->value, sizeof( address ) );
			strncpy( person, item->title, sizeof( person ) );
						
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
	else if( index == 4 )		
		{
			DictionaryIterator *iter;
			app_message_outbox_begin(&iter);
			dict_write_uint8( iter, 0, ACTION_RECENT_CALLS );
			app_message_outbox_send();

			show_loading_window();
		}
		else if( index == 5 )		
		{
			DictionaryIterator *iter;
			app_message_outbox_begin(&iter);
			dict_write_uint8( iter, 0, ACTION_FAVOURITE );
			app_message_outbox_send();
			
			person[0] = 0;
			
			show_loading_window();
		}
		
		APP_LOG(APP_LOG_LEVEL_INFO, "Selected contact %s", item->value );
}

void contact_select_handler( SelectItem* item, unsigned short index )
{	
		if( strcmp( item->title, "" ) == 0 )	
			return;
	
		strncpy( address, item->value, sizeof( address ) );
		strncpy( person, item->title, sizeof( person ) );
			
		DictionaryIterator *iter;
		app_message_outbox_begin(&iter);
		dict_write_uint8( iter, 0, ACTION_SELECT_CONTACT );
		app_message_outbox_send();
	
		show_loading_window();
}

void combine_select_handler( SelectItem* item, unsigned short index )
{	
	// Send message
	if( index == 0 )
	{
			DictionaryIterator *iter;
			app_message_outbox_begin(&iter);
			dict_write_uint8( iter, 0, ACTION_NAVIGATE );
			dict_write_cstring( iter, 1, path );
			dict_write_cstring( iter, 2, address );
			app_message_outbox_send();	
				
			window_stack_pop( true );
			show_loading_window();
	}
	
	// Combine message
	else if( index == 1 )
	{
		char* last_path = path;		
		size_t path_len = strlen( path ) - 1;
		
		// Find the most recent path
		for( size_t i = 0; i < path_len; i++ )
		{
			if( path[i] == '.' )
				last_path = path + i;
		}
		
		// Skip the period if it exists (won't exist if it is the first path)
		if( *last_path == '.' )
			last_path++;
		
		// Copy the last path after a period separator, - 1 because we don't want to include the message we just clicked
		path_len = strlen( last_path ) - 1;
		
		for( size_t i = 0; i < path_len; i ++ )
		{
			*(current_path+i+1) = last_path[i];
		}
		
		// Add the period separator, move the cursor forward and add a terminator
		*current_path = '.';
		current_path += path_len + 2;
		*current_path = 0;
		
		// Pop the combine message prompt window
		window_stack_pop( true );
	}
	
	// Exit
	else if ( index == 2 )
	{
		window_stack_pop_all( false );
	}
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
		
		// Check if it's a menu or message
	
		if( item->title[0] == '+' )
		{
			DictionaryIterator *iter;
			app_message_outbox_begin(&iter);
			dict_write_uint8( iter, 0, ACTION_NAVIGATE );
			dict_write_cstring( iter, 1, path );
			app_message_outbox_send();	
			
			show_loading_window();
		}
		else
		{
			if( combine_messages )
			{
					combine_message_window = select_window_create();
					combine_message_window->handler = combine_select_handler;
				
					strncpy( combine_message_window->item_up.title, send_message, sizeof( combine_message_window->item_up.title ) );
					strncpy( combine_message_window->item_select.title, combine_message, sizeof( combine_message_window->item_select.title ) );
					strncpy( combine_message_window->item_down.title, exit_message, sizeof( combine_message_window->item_down.title ) );
				
					strncpy( combine_message_window->status, "...", sizeof( combine_message_window->status ) );
				
					select_window_refresh( combine_message_window );
				
					DictionaryIterator *iter;
					app_message_outbox_begin(&iter);
					dict_write_uint8( iter, 0, ACTION_COMBINE );
					dict_write_cstring( iter, 1, path );
					app_message_outbox_send();	
			}
			else
			{
					DictionaryIterator *iter;
					app_message_outbox_begin(&iter);
					dict_write_uint8( iter, 0, ACTION_NAVIGATE );
					dict_write_cstring( iter, 1, path );
					dict_write_cstring( iter, 2, address );
					app_message_outbox_send();	
				
					show_loading_window();
			}
		}
	
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
		home_window = select_window_create();
		
		home_window->handler = home_select_handler;
		
		t = dict_find( iterator, 1 ); strncpy( home_window->item_up.title, t->value->cstring, sizeof( home_window->item_up.title ) );
		t = dict_find( iterator, 2 ); strncpy( home_window->item_up.value, t->value->cstring, sizeof( home_window->item_up.value ) );
		t = dict_find( iterator, 3 ); strncpy( home_window->item_down.title, t->value->cstring, sizeof( home_window->item_down.title ) );
		t = dict_find( iterator, 4 ); strncpy( home_window->item_down.value, t->value->cstring, sizeof( home_window->item_down.value ) );

		t = dict_find( iterator, 5 );
		
		if( t != NULL && t->value->uint8 == 1 )
		{
				strncpy( home_window->status, "UPDATE REQUIRED", sizeof( home_window->status ) );
		}
		else
		{
				strncpy( home_window->status, "Texter", sizeof( home_window->status ) );	
		}
		
		// Combine messages
		t = dict_find( iterator, 6 );
		combine_messages = t != NULL && ( t->value->uint8 == 1 );
		
		strncpy( home_window->item_long_up.title, "+ Conversations", sizeof( home_window->item_long_up.title ) );
		strncpy( home_window->item_long_down.title, "+ Favourites", sizeof( home_window->item_long_down.title ) );
		
		select_window_refresh( home_window );
		
		hide_loading_window();
						
		APP_LOG( APP_LOG_LEVEL_INFO, "Received selection" );
	}
	else if( action == ACTION_HOME_DATA_EXTRA )
	{
		t = dict_find( iterator, 1 ); strncpy( home_window->item_select.title, t->value->cstring, sizeof( home_window->item_select.title ) );
		t = dict_find( iterator, 2 ); strncpy( home_window->item_select.value, t->value->cstring, sizeof( home_window->item_select.value ) );
		strncpy( home_window->item_long_select.title, "+ Call Log", sizeof( home_window->item_long_select.title ) );
		select_window_refresh( home_window );
	}
	else if( action == ACTION_CONTACTS )
	{
		contact_list = select_window_create();
		
		contact_list->handler = contact_select_handler;
		
		t = dict_find( iterator, 1 ); strncpy( contact_list->item_up.title, t->value->cstring, sizeof( contact_list->item_up.title ) );
		t = dict_find( iterator, 2 ); strncpy( contact_list->item_up.value, t->value->cstring, sizeof( contact_list->item_up.value ) );
		t = dict_find( iterator, 3 ); strncpy( contact_list->item_select.title, t->value->cstring, sizeof( contact_list->item_select.title ) );
		t = dict_find( iterator, 4 ); strncpy( contact_list->item_select.value, t->value->cstring, sizeof( contact_list->item_select.value ) );
		t = dict_find( iterator, 5 ); strncpy( contact_list->item_down.title, t->value->cstring, sizeof( contact_list->item_down.title ) );
		t = dict_find( iterator, 6 ); strncpy( contact_list->item_down.value, t->value->cstring, sizeof( contact_list->item_down.value ) );
		
		contact_list->item_long_up.title[0] = 0;
		contact_list->item_long_select.title[0] = 0;
		contact_list->item_long_down.title[0] = 0;
		
		contact_list->status[0] = 0;
		
		select_window_refresh( contact_list );
		
		hide_loading_window();
						
		APP_LOG( APP_LOG_LEVEL_INFO, "Received contacts" );
	}
	else if( action == ACTION_CONTACTS_EXTRA )
	{
		if( contact_list != NULL )
		{
			t = dict_find( iterator, 1 ); strncpy( contact_list->item_long_up.title, t->value->cstring, sizeof( contact_list->item_long_up.title ) );
			t = dict_find( iterator, 2 ); strncpy( contact_list->item_long_up.value, t->value->cstring, sizeof( contact_list->item_long_up.value ) );
			t = dict_find( iterator, 3 ); strncpy( contact_list->item_long_select.title, t->value->cstring, sizeof( contact_list->item_long_select.title ) );
			t = dict_find( iterator, 4 ); strncpy( contact_list->item_long_select.value, t->value->cstring, sizeof( contact_list->item_long_select.value ) );
			t = dict_find( iterator, 5 ); strncpy( contact_list->item_long_down.title, t->value->cstring, sizeof( contact_list->item_long_down.title ) );
			t = dict_find( iterator, 6 ); strncpy( contact_list->item_long_down.value, t->value->cstring, sizeof( contact_list->item_long_down.value ) );

			select_window_refresh( contact_list );

			APP_LOG( APP_LOG_LEVEL_INFO, "Received contact extra" );
		}
	}
	else if( action == ACTION_MESSAGE_BRANCH )
	{
		SelectWindow* select_window = select_window_create();
		select_window->handler = navigate_select_handler;
		
		APP_LOG( APP_LOG_LEVEL_INFO, "Created window" );
		
		t = dict_find( iterator, 1 ); strncpy( select_window->item_up.title, t->value->cstring, sizeof( select_window->item_up.title ) );
		t = dict_find( iterator, 2 ); strncpy( select_window->item_select.title, t->value->cstring, sizeof( select_window->item_select.title ) );
		t = dict_find( iterator, 3 ); strncpy( select_window->item_down.title, t->value->cstring, sizeof( select_window->item_down.title ) );
		
		t = dict_find( iterator, 4 ); strncpy( select_window->item_long_up.title, t->value->cstring, sizeof( select_window->item_long_up.title ) );
		t = dict_find( iterator, 5 ); strncpy( select_window->item_long_select.title, t->value->cstring, sizeof( select_window->item_long_select.title ) );
		t = dict_find( iterator, 6 ); strncpy( select_window->item_long_down.title, t->value->cstring, sizeof( select_window->item_long_down.title ) );
		
		snprintf( select_window->status, sizeof( select_window->status ), "To: %s", person );
		
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
		t = dict_find( iterator, 1 );	strncpy( person, t->value->cstring, sizeof( person ) );
		t = dict_find( iterator, 2 );	strncpy( message, t->value->cstring, sizeof( message ) );
		
		t = dict_find( iterator, 3 );
	
		bool success = true;
		
		// < v1.2 companion app didn't send this byte
		if( t != NULL )
			success = (t->value->uint8 == 1);
		
		// Display the message sent window for some time
		show_message_sent_window( person, message, success );
	}
	else if( action == ACTION_CURRENT_MESSAGE )
	{
		if( combine_message_window != NULL )
		{
			t = dict_find( iterator, 1 ); 
			snprintf( combine_message_window->status, sizeof( combine_message_window->status ), "\"%s\"", t->value->cstring );
			select_window_refresh( combine_message_window );
		}
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
	
	// Tell the app what version we are running in case it's out of date
	dict_write_uint8( iter, 1, VERSION_MAJOR );
	dict_write_uint8( iter, 2, VERSION_MINOR );
	
  app_message_outbox_send();
}

int main(void)
{
	APP_LOG(APP_LOG_LEVEL_INFO, "App started" );
	
	*current_path = 0;
	
  handle_init();
  app_event_loop();
}
