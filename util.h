// if 1, code will print statements to a log file
// considering making it an or set up 
// (e.g. #define CONTROLLER_DBG 1, TABLE_DBG 2... #define DEBUG_LOG (CONTROLLER_DBG | TABLE_DBG))
#define DEBUG_LOG 0

// Prints the payload in an easy to read format for a tcp packet, full packet
void printTCPpayload(char payload[1600]);

// Prints the payload in an easy to read format for a tcp packet, up to len characters
void printTCPpayload(char payload[1600], int len);

// Writes a message to the log file with the tag specified
void writeToLog(char* message, char* tag);
