// Filename:-	animevents.cpp
//
// animevents.cfg file
//


#include "stdafx.h"
#include "includes.h"
#include "ModViewTreeView.h"
#include "glm_code.h"
#include "R_Model.h"
#include "R_Surface.h"
#include "textures.h"
#include "TEXT.H"
#include "sequence.h"
#include "model.h"
#include "GenericParser2.h"
//
#include "animevents.h"




CGenericParser2	theParserAnimEvents;

LPCSTR AnimEvents_GetExtension(void) 
{
	return sANIMEVENTS_EXTENSION;
}

LPCSTR AnimEvents_GetFilter(bool bStandAlone)
{
	static char sFilterString[1024];

	strcpy(sFilterString, "ModView animevents files (*" sANIMEVENTS_EXTENSION ")|*" sANIMEVENTS_EXTENSION "|");

	strcat(sFilterString,bStandAlone?"|":"");
	return sFilterString;
}

static void AnimEvents_Write_UpperEvents(void *pvData)
{
	FILE *fhText = (FILE *) pvData;
	
	/*
		
	AEV_SOUND		= animID(enum string), AEV_SOUND(enum string), framenum(int), soundpath(string), randomlow(int), randomhi(int), chancetoplay(int),
	AEV_FOOTSTEP	= animID, AEV_FOOTSTEP, framenum, footstepType, chancetoplay,
	AEV_EFFECT		= animID, AEV_EFFECT, framenum, effectpath, boltName, chancetoplay,
	AEV_FIRE		= animID, AEV_FIRE, framenum, altfire, chancetofire,
	AEV_MOVE		= animID, AEV_MOVE, framenum, forwardpush, rightpush, uppush,
	AEV_SOUNDCHAN	= animID, AEV_SOUNDCHAN, framenum, CHANNEL, soundpath, randomlow, randomhi, chancetoplay,
	AEV_SABER_SWING = animID, AEV_SABER_SWING, framenum, CHANNEL, randomlow, randomhi, chancetoplay,
	AEV_SABER_SPIN	= animID, AEV_SABER_SPIN, framenum, CHANNEL, chancetoplay,

	*/

	char *AnimID = "animID";
    char *soundPath = "soundPath";
    char *effectPath = "effectPath";
    char *boltName = "boltName";
    char *soundChannel = "soundChannel";
    int animFrame = 0;
    int randomLow = 0;
    int randomHigh = 0;
    int chanceToPlay = 0;

	if (AEV_SOUND)
		fprintf(fhText, "\t%s\tAEV_SOUND\t%i\t%s\t%i\t%i\t%i\n", AnimID, animFrame, soundPath, randomLow, randomHigh, chanceToPlay);

	if (AEV_EFFECT)
		fprintf(fhText, "\t%s\tAEV_EFFECT\t%i\t%s\t%s\t%i\n", AnimID, animFrame, effectPath, boltName, chanceToPlay);
	
	if (AEV_SOUNDCHAN)
		fprintf(fhText, "\t%s\tAEV_SOUNDCHAN\t%i\t%s\t%s\t%i\t%i\t%i\n", AnimID, animFrame, soundChannel, soundPath, randomLow, randomHigh, chanceToPlay);
	
	if (AEV_SABER_SWING)
		fprintf(fhText, "\t%s\tAEV_SABER_SWING\t%i\t%s\t%i\t%i\t%i\n", AnimID, animFrame, soundChannel, randomLow, randomHigh, chanceToPlay);
	
	if (AEV_SABER_SPIN)
		fprintf(fhText, "\t%s\tAEV_SABER_SPIN\t%i\t%s\t%i\n", AnimID, animFrame, soundChannel, chanceToPlay);
}

static void AnimEvents_Write_LowerEvents(void *pvData)
{
	FILE *fhText = (FILE *) pvData;
	
	/*
		
	AEV_SOUND		= animID(enum string), AEV_SOUND(enum string), framenum(int), soundpath(string), randomlow(int), randomhi(int), chancetoplay(int),
	AEV_FOOTSTEP	= animID, AEV_FOOTSTEP, framenum, footstepType, chancetoplay,
	AEV_EFFECT		= animID, AEV_EFFECT, framenum, effectpath, boltName, chancetoplay,
	AEV_FIRE		= animID, AEV_FIRE, framenum, altfire, chancetofire,
	AEV_MOVE		= animID, AEV_MOVE, framenum, forwardpush, rightpush, uppush,
	AEV_SOUNDCHAN	= animID, AEV_SOUNDCHAN, framenum, CHANNEL, soundpath, randomlow, randomhi, chancetoplay,
	AEV_SABER_SWING = animID, AEV_SABER_SWING, framenum, CHANNEL, randomlow, randomhi, chancetoplay,
	AEV_SABER_SPIN	= animID, AEV_SABER_SPIN, framenum, CHANNEL, chancetoplay,

	*/

	ModelContainer_t *pContainer = &AppVars.Container;
	Sequence_t *pLockedSequence = NULL;

	if (Model_MultiSeq_IsActive(pContainer, true))
	{
		int iSequenceNumber = Model_MultiSeq_SeqIndexFromFrame(pContainer, pContainer->iCurrentFrame_Primary, true, false );
		pLockedSequence		= (iSequenceNumber == -1) ? NULL : &pContainer->SequenceList[iSequenceNumber];
	}
	else
	{
		int iSequenceNumber = pContainer->iSequenceLockNumber_Primary;
		pLockedSequence		= (iSequenceNumber == -1) ? NULL : &pContainer->SequenceList[iSequenceNumber];
	}

	pLockedSequence = Sequence_DeriveFromFrame( pContainer->iCurrentFrame_Primary, pContainer );

	LPCSTR AnimID = Sequence_GetName(pLockedSequence);
    char *soundPath = "soundPath";
    char *effectPath = "effectPath";
    char *boltName = "boltName";
    char *soundChannel = "soundChannel";
    int animFrame = pContainer->iCurrentFrame_Primary - pLockedSequence->iStartFrame;
    int randomLow = 0;
    int randomHigh = 0;
    int chanceToPlay = 0;

	if (AEV_SOUND)
		fprintf(fhText, "\t%s\tAEV_SOUND\t%i\t%s\t%i\t%i\t%i\n", AnimID, animFrame, soundPath, randomLow, randomHigh, chanceToPlay);

	if (AEV_FOOTSTEP)
	{
		//if (footstep_r)
			fprintf(fhText, "\t%s\tAEV_FOOTSTEP\t%i\tfootstep_r\t%i\n", AnimID, animFrame, chanceToPlay);

		//if (footstep_l)
			fprintf(fhText, "\t%s\tAEV_FOOTSTEP\t%i\tfootstep_l\t%i\n", AnimID, animFrame, chanceToPlay);
	}
	
	if (AEV_EFFECT)
		fprintf(fhText, "\t%s\tAEV_EFFECT\t%i\t%s\t%s\t%i\n", AnimID, animFrame, effectPath, boltName, chanceToPlay);
	
	if (AEV_SOUNDCHAN)
		fprintf(fhText, "\t%s\tAEV_SOUNDCHAN\t%i\t%s\t%s\t%i\t%i\t%i\n", AnimID, animFrame, soundChannel, soundPath, randomLow, randomHigh, chanceToPlay);
}

bool AnimEvents_Write(LPCSTR psFullPathedFilename)
{
	FILE *fhText = NULL;

	if (Model_Loaded())
	{
		fhText = fopen(psFullPathedFilename,"wt");

		if (fhText)
		{
			fprintf(fhText, "//AEV_SOUND		= animID(enum string), AEV_SOUND(enum string), framenum(int), soundpath(string), randomlow(int), randomhi(int), chancetoplay(int)\n");
            fprintf(fhText, "//AEV_FOOTSTEP		= animID(enum string), AEV_FOOTSTEP(enum string), framenum(int), footstepType(string), chancetoplay(int)\n");
			fprintf(fhText, "//AEV_EFFECT		= animID(enum string), AEV_EFFECT(enum string), framenum(int), effectpath(string), boltName, chancetoplay(int)\n");
			fprintf(fhText, "//AEV_SOUNDCHAN		= animID(enum string), AEV_SOUNDCHAN(enum string), framenum(int), CHANNEL(enum string), soundpath(string), randomlow(int), randomhi(int), chancetoplay(int)\n");
			fprintf(fhText, "//AEV_SABER_SWING	= animID(enum string), AEV_SABER_SWING(enum string), framenum(int), CHANNEL(enum string), randomlow(int), randomhi(int), chancetoplay(int)\n");
			fprintf(fhText, "//AEV_SABER_SPIN	= animID(enum string), AEV_SABER_SPIN(enum string), framenum(int), CHANNEL(enum string), chancetoplay(int)\n\n");

			//UPPEREVENTS
			fprintf(fhText,va("%s\n{\n",sANIMEVENTSKEYWORD_UPPEREVENTS));

			AnimEvents_Write_UpperEvents(fhText);

			fprintf(fhText,"}\n");

			fprintf(fhText,"\n");	// cosmetic

			//LOWEREVENTS
			fprintf(fhText,va("%s\n{\n",sANIMEVENTSKEYWORD_LOWEREVENTS));

			AnimEvents_Write_LowerEvents(fhText);

			fprintf(fhText,"}\n");

			fprintf(fhText,"\n");	// cosmetic

			fclose(fhText);
		}
		// DT EDIT
		/*
		else
		{
			ErrorBox( va("Couldn't open file: %s\n", psFullPathedFilename));
			return false;
		}
		*/

	}

	return !!fhText;
}

bool AnimEvents_Read(LPCSTR psFullPathedFilename)
{
	LPCSTR psError = NULL;
	char *psData = NULL;	
	
	int iSize = LoadFile(psFullPathedFilename, (void**)&psData);
	if (iSize != -1)
	{
		SetQdirFromPath( psFullPathedFilename );

		char *psDataPtr = psData;
		
		// parse it...
		//
		if (theParserAnimEvents.Parse(&psDataPtr, true))
		{
			CGPGroup *pFileGroup = theParserAnimEvents.GetBaseParseGroup();

			CGPGroup *pParseGroup_UpperEvents = pFileGroup->FindSubGroup(sANIMEVENTSKEYWORD_UPPEREVENTS);//, true);

			if (pParseGroup_UpperEvents)
			{
				// special optional arg for NPC->MVS hackiness...
				//
				string strBaseDir = pParseGroup_UpperEvents->FindPairValue(sANIMEVENTSKEYWORD_BASEDIR, "");
				if (!strBaseDir.empty())
				{
					SetQdirFromPath( strBaseDir.c_str() );
				}
				
				//psError = AnimEvents_Parse_LoadModel(pParseGroup_LoadModel);
			}
			else
			{
				psError = "Unable to find keyword '" sANIMEVENTSKEYWORD_UPPEREVENTS "'";
			}

			CGPGroup *pParseGroup_LowerEvents = pFileGroup->FindSubGroup(sANIMEVENTSKEYWORD_LOWEREVENTS);//, true);

			if (pParseGroup_UpperEvents)
			{
				// special optional arg for NPC->MVS hackiness...
				//
				string strBaseDir = pParseGroup_LowerEvents->FindPairValue(sANIMEVENTSKEYWORD_BASEDIR, "");
				if (!strBaseDir.empty())
				{
					SetQdirFromPath( strBaseDir.c_str() );
				}
				
				//psError = AnimEvents_Parse_LoadModel(pParseGroup_LoadModel);
			}
			else
			{
				psError = "Unable to find keyword '" sANIMEVENTSKEYWORD_LOWEREVENTS "'";
			}
		}
		else
		{
			psError = va("{} - Brace mismatch error in file \"%s\"!",psFullPathedFilename);
		}
	}
	else
	{
		psError = "File not found";
	}

	SAFEFREE(psData);

	if (psError)
	{
		ErrorBox(va("Error while reading animevents file: \"%s\":\n\n%s",psFullPathedFilename,psError));
	}

	return !psError;
}


/////////////////// eof /////////////////


