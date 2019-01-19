// Filename:-	R_MD3.h
//
// basic header to access a pile of code pasted from other projects to get support for this format up and running...
//

#ifndef R_MD3_H
#define	R_MD3_H


extern qboolean R_LoadMD3 (model_t *mod, int lod, void *buffer, const char *name );
void R_AddMD3Surfaces( trRefEntity_t *ent );
bool MD3Model_Parse(struct ModelContainer *pContainer, LPCSTR psLocalFilename, HTREEITEM hTreeItem_Parent = NULL);
void RB_SurfaceMesh(md3Surface_t *surface);
bool R_MD3Model_Tree_ReEvalSurfaceText(ModelHandle_t hModel, HTREEITEM hTreeItem = NULL);
bool MD3Model_SurfaceIsTag(ModelHandle_t hModel, int iSurfaceindex);
LPCSTR MD3Model_GetSurfaceName(ModelHandle_t hModel, int iSurfaceIndex);

#endif	// #ifndef R_MD3_H

/////////////////////// eof /////////////////////

