Logging is intended for debug/development only.

All logging should go through Logger.  (Some direct uses of nRF5x logging might remain in the code.)

For release configuration, functions would be defined empty.

Uses logging functions of nRF5x library.  If it is built without logging, then Logger will be impotent too.
That defines where the log is (currently uses Segger RTT to a console.)