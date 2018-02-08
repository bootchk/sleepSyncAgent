
/*
 * Over-the-air payload.
 *
 * Constants for serializer.
 */
class OTAPayload {
public:

	// Message.type hardcoded to index 0, length 1
	static const int TypeTSSIIndex = 0;
	static const int TypeTSSILength = 1;

	static const int MasterIndex = 1;
	static const int MasterIDLength = 6;

	static const int OffsetIndex = 7;
	static const int OffsetLength = 3;

	static const int WorkIndex = 10;
	static const int WorkLength = 1;


	// Total length defined in platform/radio.h
	// If you add a field, change that def also.
};
