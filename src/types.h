
/*
 * Fundamental types, some must match os and radio
 */

// TODO OS wrapper

/*
 * MAC id of radio, unique system/unit identifier.
 * Content of sync msg to identify  master.
 */
typedef long SystemID;
/*
 * Content of sync msg to adjust sync.
 */
typedef long SyncOffset;
/*
 * Used in scheduling to count slots and periods.
 */
typedef short ScheduleCount;
