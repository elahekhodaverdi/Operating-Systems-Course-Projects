#include "fdset.h"

void set_fd(int socket, FDSet *fdset)
{
    FD_SET(socket, &fdset->master);
    if (socket > fdset->max)
        fdset->max = socket;
}

void clear_fd(int socket, FDSet *fdset)
{
    FD_CLR(socket, &fdset->master);
    if (socket == fdset->max)
        --fdset->max;
}