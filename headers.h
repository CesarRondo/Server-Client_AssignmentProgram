/*
By:
Cesar Rondo Rodriguez
Student num: 22779089
email: cesar.rondorodriguez@ucdconnect.ie
*/

void readServerMessage(int client_fd, char *buffer);            
void writeSereverMessage(int client_fd, char *initialMessage, int mode);
void signalHandlerINT();
void signalHandlerTERM();