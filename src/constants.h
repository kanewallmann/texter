#pragma once 
	
// Incoming actions
#define ACTION_HOME_DATA 0 
#define ACTION_MESSAGE_BRANCH 1
#define ACTION_MESSAGE_SENT 2
#define ACTION_CONTACTS 3
#define ACTION_CONTACTS_EXTRA 4
#define ACTION_HOME_DATA_EXTRA 5
#define ACTION_CURRENT_MESSAGE 6

// Outgoing actions
#define ACTION_SHOW_HOME 0
#define ACTION_SELECT_CONTACT 1
#define ACTION_NAVIGATE 2
#define ACTION_RECENT 3
#define ACTION_FAVOURITE 4
#define ACTION_RECENT_CALLS 5
#define ACTION_COMBINE 6
	
#define VERSION_MAJOR 1
#define VERSION_MINOR 4

// Comment out for debug
#undef APP_LOG
#define APP_LOG(level, fmt, args... )
	
char path[24];
char* current_path;

char address[12];
char person[24];
char message[64];