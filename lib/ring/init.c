#include "ring.h"
#include "ring.private.h"
#include <liburing.h>

int ring_init(unsigned int entries, unsigned int flags)
{
    return io_uring_queue_init(entries, &ring, flags);
}