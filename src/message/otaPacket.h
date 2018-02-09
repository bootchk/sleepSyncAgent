
/*
 * Over-the-air payload.
 *
 * Constants for serializer.
 */
class OTAPayload {
public:

	// Type and TSS packed in first byte
	static const int TypeTSSIndex = 0;
	static const int TypeTSSLength = 1;

	static const int MasterIndex = 1;
	static const int MasterIDLength = 6;

	static const int OffsetIndex = 7;
	static const int OffsetLength = 3;

	static const int WorkIndex = 10;
	static const int WorkLength = 1;


	// Total length defined in platform/radio.h
	// If you add a field, change that def also.
};
