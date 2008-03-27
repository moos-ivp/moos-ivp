#ifndef _AisTargetDataIncludes_h__
#define _AisTargetDataIncludes_h__

#define AIS_MAX_TARGET_SPEC_LINE_LENGTH		256
#define AIS_MAX_DIRECTORY_PATH_NAME_LENGTH	512

/*
#ifdef RAN_BUILD
#define AIS_PATH_TO_SPEC_FILES "/h/SMMTT/data/shared/ais/"
#else
#define AIS_PATH_TO_SPEC_FILES "/net/ifs/h/SMMTT/data/ais/"
#endif
*/

// Data to be sent to CC as AIS info (plus some SMMTT unique data)
typedef struct 
{
	double	ref_point_to_bow;		// A, in meters
	double	ref_point_to_stern;		// B, in meters
	double	ref_point_to_port;		// C, in meters
	double	ref_point_to_starboard;	// D, in meters
} TargetReferencePointType;

// Types of Ships, as defined in ITU-R M.1371-2
// DO NOT CHANGE THIS LIST.  ENUMS CORRESPOND TO SPEC VALUES
typedef enum
{
	AIS_SHIP_TYPE_NONE,				// 0 Not used
	AIS_SHIP_TYPE_RESERVED,			// 1.
	AIS_SHIP_TYPE_WIG,				// 2. Wing In Ground (Hovercrafts)
	AIS_SHIP_TYPE_SPECIAL_CRAFT,	// 3. Fishing, Towing, Sailing
	AIS_SHIP_TYPE_HIGH_SPEED_CRAFT,	// 4.
	AIS_SHIP_TYPE_WORKING_VESSEL,	// 5. Tugs, port tenders, law enforcement
	AIS_SHIP_TYPE_PASSENGER_SHIP,	// 6.
	AIS_SHIP_TYPE_CARGO_SHIP,		// 7.
	AIS_SHIP_TYPE_TANKER,			// 8.
	AIS_SHIP_TYPE_OTHER				// 9.
	
} AisShipTypeEnumeration;


// Types of Cargo/Operations, per ITU-R M.1371-2

// Cargo Type - applies to WIG, High Speed Crafts, Passenger Ships, Cargo Ships
// Tankers and "Other" types
// DO NOT CHANGE THIS LIST.  ENUMS CORRESPOND TO SPEC VALUES
typedef enum
{
	AIS_CARGO_NO_HAZARDS,		// 0
	AIS_CARGO_CATEGORY_A,		// 1
	AIS_CARGO_CATEGORY_B,		// 2
	AIS_CARGO_CATEGORY_C,		// 3
	AIS_CARGO_CATEGORY_D,		// 4
	AIS_CARGO_RESERVED_5,		// 5
	AIS_CARGO_RESERVED_6,		// 6
	AIS_CARGO_RESERVED_7,		// 7
	AIS_CARGO_RESERVED_8,		// 8
	AIS_CARGO_NO_INFORMATION 	// 9	

} AisCargoEnum;

// Special Craft Operation Options
// DO NOT CHANGE THIS LIST.  ENUMS CORRESPOND TO SPEC VALUES
typedef enum
{
	AIS_SPECIAL_FISHING,		// 0
	AIS_SPECIAL_TOWING,			// 1
	AIS_SPECIAL_TOWING_LARGE,	// 2
	AIS_SPECIAL_DREDGING,		// 3
	AIS_SPECIAL_DIVING,			// 4
	AIS_SPECIAL_MILITARY,		// 5
	AIS_SPECIAL_SAILING,		// 6
	AIS_SPECIAL_PLEASURE,		// 7
	AIS_SPECIAL_RESERVED_8,		// 8
	AIS_SPECIAL_RESERVED_9		// 9
	
} AisSpecialEnum;

// Working Vessel Operations
// DO NOT CHANGE THIS LIST.  ENUMS CORRESPOND TO SPEC VALUES
typedef enum
{
	AIS_SPECIAL_PILOT,				// 0
	AIS_WORKING_SAR,				// 1
	AIS_WORKING_TUG,				// 2
	AIS_WORKING_TENDER,				// 3
	AIS_WORKING_ANTI_POLLUTION,		// 4
	AIS_WORKING_LAW_ENFORCEMENT,	// 5
	AIS_WORKING_LOCAL_VESSEL_6,		// 6	
	AIS_WORKING_LOCAL_VESSEL_7,		// 7	
	AIS_WORKING_MEDICAL_TRANSPORT,	// 8
	AIS_WORKING_RR					// 9

} AisWorkingEnum;

typedef union
{
	AisCargoEnum	cargo_type;
	AisSpecialEnum	special_type;
	AisWorkingEnum	working_type;
	long			value_type;

} AisCargoAndOperationsUnion;

// TargetStaticDataType includes Voyage data as well
// If update this structure, update OAis_Entity::GetTargetAisStaticData function as well!
// This is the data needed for Message 5 (Static and Voyage Data)
typedef struct
{
	long	mmsi_number;		// 30 bits
	short	ais_version_no;		// 2 bits
	long	imo_number;			// 30 bits
	char	call_sign[8];		// actually 7 six bit ASCII characters
	char	name[24];			// actually 20 six bit ASCII characters
	long	type_of_ship;
	TargetReferencePointType ship_dimensions;
	long	position_fixing_device;
	long	ETA;
	double	maximum_present_draught;
	char	destination[24];	// actually 20 six bit ASCII characters
	char	country[64];		// AIS Country
	short	dte_available;		// 1 bit (not used anymore (HC to "Yes"))
	bool	classb_type;		// true = Class B vehicle.  False = Class A vehicle
} TargetStaticDataType;

// Dynamic Data
// If update this structure, update OAis_Entity::GetTargetAisDynamicData function as well!
// This is the data needed for Message 1 (, 2, 3, and others)
typedef struct
{
	short	navigational_status;
	double	turn_rate;			// degrees/second
	double	speed_over_ground;	// in Knots
	short	position_accuracy;	// 1 bit
	double	longitude;			// in degrees, E positive
	double	latitude;			// in degrees, N positive
	double	course_over_ground;	// in degrees
	double	true_heading;
	long	utc_time_hours;		// UTC (hours only)
	long	utc_time_minutes;	// UTC (minutes only)
	long	utc_time_seconds;	// UTC (seconds only)
	short	raim_flag;			// 1 bit
	double	depth;				// in meters.  Used by SAR Aircraft (msg 9)
} TargetDynamicDataType;

// SMMTT Unique Data
typedef struct
{
	long 	smmtt_target_number;
	short	repeat_indicator;	// 2 bits.  Used in both Static and Dynamic Message
	short	transponder_status;	// 1 = on, 0 = off	
	short	target_type;		// for/from RDBMS
	short	target_subtype;		// for/from RDBMS
} SMMTTUniqueAISData;

// A structure that has everything there is to know about the targeg
typedef struct
{
	SMMTTUniqueAISData		smmtt_data;
	TargetStaticDataType	static_data;
	TargetDynamicDataType	dynamic_data;

} AISTargetDataType;

#endif // _AisTargetDataIncludes_h__
