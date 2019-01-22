// Filename:-	R_MD3.h
//
// basic header to access a pile of code pasted from other projects to get support for this format up and running...
//

#ifndef R_MD3_H
#define	R_MD3_H


extern qboolean R_LoadMD3 (model_t *mod, int lod, void *buffer, const char *name );
void R_AddMD3Surfaces( trRefEntity_t *ent );
void RB_SurfaceMesh(surfaceInfo_t *surf);
qboolean R_LoadMD3(model_t *mod, int lod, void *buffer, const char *mod_name);


#endif	// #ifndef R_MD3_H

/////////////////////// eof /////////////////////

