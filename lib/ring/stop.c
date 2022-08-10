#include "ring.h"
#include "ring.private.h"

void ring_stop()
{
    io_uring_queue_exit(&ring);
}