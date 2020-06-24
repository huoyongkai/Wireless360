/*!
	\file
  \brief Some definitions used in SPHERE_PACKING and EXIT_CHART classes.
  \author Ming Jiang

	\date 14/04/2003 19:00
*/

#ifndef _DEF_H_
#define _DEF_H_

#define Sphere_Packing 

#define sphere_packing_L 16
#define sphere_packing_dim 4 
//#define FEC
#define Sphere_Packing_Soft_Decoding  

#define Mapping 4 // (0=Gray Mapping), (1=AGM distance 3), 
                   //(2=Random AGM), (3=Set Partioning), (>3 && <=11 randomly permuted) //

//#define STBC_Soft_Decoding

//! Enum types offset.
#define ENUM_OFFSET 100

//! Modulation methods.
enum MAP_METHODS
{
	MAP_UNKNOWN = ENUM_OFFSET+0,
	MAP_BPSK = ENUM_OFFSET+1,
	MAP_QPSK = ENUM_OFFSET+2,
	MAP_8PSK = ENUM_OFFSET+3,
	MAP_4QAM = ENUM_OFFSET+4,
	MAP_16QAM = ENUM_OFFSET+5,
	MAP_64QAM = ENUM_OFFSET+6,
	MAP_SPHERE_PACKING =ENUM_OFFSET+7, 
};

//! Channel models.
enum CHANNEL_TYPES
{
	CHN_UNKNOWN = ENUM_OFFSET+7,
	CHN_AWGN = ENUM_OFFSET+8,
	CHN_UNCOR_RAY = ENUM_OFFSET+9,
	CHN_COR_RAY = ENUM_OFFSET+10,
	CHN_MULTIPATH_RAY = ENUM_OFFSET+11,
};

//! STBC classes IDs.
enum STBC_CLASS_ID
{
	STBC_UNKNOWN = ENUM_OFFSET+12,
	STBC_G2 = ENUM_OFFSET+13,
	STBC_G3 = ENUM_OFFSET+14,
	STBC_G4 = ENUM_OFFSET+15,
	STBC_H3 = ENUM_OFFSET+16,
	STBC_H4 = ENUM_OFFSET+17,
};

#endif



	
