// Filename:-	md3_code.h
//


#ifndef MD3_CODE_H
#define MD3_CODE_H

#include "md3_format.h"


typedef struct {
	int			ident;			// ident of this surface - required so the materials renderer knows what sort of surface this refers to 
	int			surface;		// index into array held inside the model definition of pointers to the actual surface data loaded in - used by both client and game
	void		*surfaceData;	// pointer to surface data loaded into file - only used by client renderer DO NOT USE IN GAME SIDE - if there is a vid restart this will be out of wack on the game
} md3SurfaceInfo_t;

typedef struct {
	int			ident;			// ident of this tag - required so the materials renderer knows what sort of tag this refers to 
	int			tag;			// index into array held inside the model definition of pointers to the actual tag data loaded in - used by both client and game
	void		*tagData;		// pointer to tag data loaded into file - only used by client renderer DO NOT USE IN GAME SIDE - if there is a vid restart this will be out of wack on the game
} md3TagInfo_t;

bool MD3Model_Parse(struct ModelContainer *pContainer, LPCSTR psLocalFilename, HTREEITEM hTreeItem_Parent = NULL);
bool R_MD3Model_Tree_ReEvalSurfaceText(ModelHandle_t hModel, HTREEITEM hTreeItem = NULL);
bool MD3Model_IsTag(ModelHandle_t hModel, int iTagindex);
LPCSTR MD3Model_GetSurfaceName(ModelHandle_t hModel, int iSurfaceIndex);
LPCSTR MD3Model_GetTagName(ModelHandle_t hModel, int iTagIndex);
void MD3_GetSurfaceList(qhandle_t model, md3SurfaceInfo_t *md3_slist);
void MD3_GetTagList(qhandle_t model, md3TagInfo_t *md3_tlist);
bool MD3Model_GetBounds(ModelHandle_t hModel, int iLODNumber, int iFrameNumber, vec3_t &v3Mins, vec3_t &v3Maxs);
LPCSTR MD3Model_SurfaceInfo(ModelHandle_t hModel, int iSurfaceIndex);

#endif	// #ifndef MD3_CODE_H


////////////// eof ////////////

