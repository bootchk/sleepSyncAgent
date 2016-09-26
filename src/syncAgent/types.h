
#include <inttypes.h>

/*
 * Fundamental types used by SyncAgent
 */

/*
 * Time offset.  2-bytes, 16-bits, 128k
 * Content of sync msg to adjust sync.
 */
typedef uint16_t SyncOffset;


/*
 * Used in scheduling to count slots and periods.
 */
typedef short ScheduleCount;
