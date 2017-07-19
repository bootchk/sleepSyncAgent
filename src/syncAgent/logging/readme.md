Logging is intended for debug/development only.

All logging should go through Logger.

For release configuration, functions would be defined empty. (not implemented)

Uses logging functions of nRF5x library.  If it is built without logging, then Logger will be impotent too.
That defines where the log is (currently uses Segger RTT to a console.)