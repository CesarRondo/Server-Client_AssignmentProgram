/*
By:
Cesar Rondo Rodriguez
email: c.rondoro12@gmail.com
*/

void readServerMessage(int client_fd, char *buffer);            
void writeSereverMessage(int client_fd, char *initialMessage, int mode);
void signalHandlerINT();
void signalHandlerTERM();