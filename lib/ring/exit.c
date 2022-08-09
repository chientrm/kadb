#include "ring.h"
#include "ring.private.h"

void ring_exit()
{
    io_uring_queue_exit(&ring);
}