//
// osc.h
//
// OSC interface
//

// Set up connection to SooperLooper server and listening server for update
// messages
int osc_init(const char* sl_url, const char* own_url);

// Cleanup
void osc_end();

// Send an OSC message to SooperLooper
int osc_send();

// Attach a handler to an OSC path
void osc_add_method();
