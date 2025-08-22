// Filename:-	animevents.h
//

#ifndef ANIMEVENTS_H
#define ANIMEVENTS_H


///////////////////////////////////////////////////////////////////
//
// I've externalised these #defines so that the NPC->ModView script generator can use the same keywords 
//	without worrying about keeping 2 version in sync...
//
#define sANY_NONBLANK_STRING "<any non-blank string>"	// for when I just need to write out a flag keyword,
														//	but GenericParser needs a string arg during reading in.
#define sANIMEVENTSKEYWORD_UPPEREVENTS	"UPPEREVENTS"
#define sANIMEVENTSKEYWORD_LOWEREVENTS	"LOWEREVENTS"
#define sANIMEVENTSKEYWORD_BASEDIR		"basedir"	// special hack use for writing temp MVS files during NPC->MVS conversion

//
#define sANIMEVENTS_EXTENSION			".cfg"
//
///////////////////////////////////////////////////////////////////

//size of Anim eventData array...
#define MAX_RANDOM_ANIM_SOUNDS		4
#define	AED_ARRAY_SIZE				(MAX_RANDOM_ANIM_SOUNDS+3)
//indices for AEV_SOUND data
#define	AED_SOUNDINDEX_START		0
#define	AED_SOUNDINDEX_END			(MAX_RANDOM_ANIM_SOUNDS-1)
#define	AED_SOUND_NUMRANDOMSNDS		(MAX_RANDOM_ANIM_SOUNDS)
#define	AED_SOUND_PROBABILITY		(MAX_RANDOM_ANIM_SOUNDS+1)
//indices for AEV_SOUNDCHAN data
#define	AED_SOUNDCHANNEL			(MAX_RANDOM_ANIM_SOUNDS+2)
//indices for AEV_FOOTSTEP data
#define	AED_FOOTSTEP_TYPE			0
#define	AED_FOOTSTEP_PROBABILITY	1
//indices for AEV_EFFECT data
#define	AED_EFFECTINDEX				0
#define	AED_BOLTINDEX				1
#define	AED_EFFECT_PROBABILITY		2
#define	AED_MODELINDEX				3
//indices for AEV_SABER_SWING data
#define	AED_SABER_SWING_SABERNUM	0
#define	AED_SABER_SWING_TYPE		1
#define	AED_SABER_SWING_PROBABILITY	2
//indices for AEV_SABER_SPIN data
#define	AED_SABER_SPIN_SABERNUM		0
#define	AED_SABER_SPIN_TYPE			1	//0 = saberspinoff, 1 = saberspin, 2-4 = saberspin1-saberspin3
#define	AED_SABER_SPIN_PROBABILITY	2

typedef enum
{
	AEV_NONE,
	AEV_SOUND,		//# animID AEV_SOUND framenum soundpath randomlow randomhi chancetoplay
	AEV_FOOTSTEP,	//# animID AEV_FOOTSTEP framenum footstepType chancetoplay
	AEV_EFFECT,		//# animID AEV_EFFECT framenum effectpath boltName chancetoplay
	AEV_SOUNDCHAN,  //# animID AEV_SOUNDCHAN framenum CHANNEL soundpath randomlow randomhi chancetoplay
	AEV_SABER_SWING,  //# animID AEV_SABER_SWING framenum CHANNEL randomlow randomhi chancetoplay
	AEV_SABER_SPIN,  //# animID AEV_SABER_SPIN framenum CHANNEL chancetoplay
	AEV_NUM_AEV
} animEventType_t;

typedef struct animevent_s
{
	animEventType_t	eventType;
	unsigned short	keyFrame;			//Frame to play event on
	signed short	eventData[AED_ARRAY_SIZE];	//Unique IDs, can be soundIndex of sound file to play OR effect index or footstep type, etc.
	char			*stringData;		//we allow storage of one string, temporarily (in case we have to look up an index later, then make sure to set stringData to NULL so we only do the look-up once)
} animevent_t;

///////////////////////////////////////////////////////////////////

LPCSTR AnimEvents_GetExtension(void);
LPCSTR AnimEvents_GetFilter(bool bStandAlone = true);
bool AnimEvents_Write(LPCSTR psFullPathedFilename);
bool AnimEvents_Read(LPCSTR psFullPathedFilename);

#endif	// #ifndef ANIMEVENTS_H

//////////////////// eof //////////////////////


