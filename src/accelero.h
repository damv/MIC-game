#ifndef __ACCELERO_H__
#define __ACCELERO_H__

// INIT
// Set config
void accelero_init();

// BEGIN
// Initiate connection
void accelero_begin();

// READ
void accelero_read(unsigned char *readByte);

#endif
