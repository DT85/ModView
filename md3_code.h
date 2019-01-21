// Filename:-	md3_code.h
//


#ifndef MD3_CODE_H
#define MD3_CODE_H

#include "md3_format.h"


bool MD3Model_Parse(struct ModelContainer *pContainer, LPCSTR psLocalFilename, HTREEITEM hTreeItem_Parent = NULL);
bool R_MD3Model_Tree_ReEvalSurfaceText(ModelHandle_t hModel, HTREEITEM hTreeItem = NULL);
bool MD3Model_SurfaceIsTag(ModelHandle_t hModel, int iSurfaceindex);
LPCSTR MD3Model_GetSurfaceName(ModelHandle_t hModel, int iSurfaceIndex);
void MD3_GetSurfaceList(qhandle_t model, surfaceInfo_t *md3_slist);

#endif	// #ifndef MD3_CODE_H


////////////// eof ////////////

