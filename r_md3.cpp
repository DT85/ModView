// Filename:-	R_MD3.cpp
//
// contains a whole wodge of code pasted from our other codebases in order to quickly get this format up and running...
//
#include "stdafx.h"
#include "includes.h"
#include "R_Common.h"
//
#include "r_md3.h"
#include "R_Model.h"
#include "textures.h"


/*
=================
R_LoadMD3
=================
*/
qboolean R_LoadMD3(model_t *mod, int lod, void *buffer, const char *mod_name) {
	int					i, j;
	md3Header_t			*pinmodel;
	md3Frame_t			*frame;
	md3Surface_t		*surf;
	md3Shader_t			*shader;
	md3Triangle_t		*tri;
	md3St_t				*st;
	md3XyzNormal_t		*xyz;
	md3Tag_t			*tag;
	int					version;
	int					size;

	pinmodel = (md3Header_t *)buffer;

	version = LL(pinmodel->version);
	if (version != MD3_VERSION) {
		ri.Printf(PRINT_WARNING, "R_LoadMD3: %s has wrong version (%i should be %i)\n",
			mod_name, version, MD3_VERSION);
		return qfalse;
	}

	mod->type = MOD_MESH;
	size = LL(pinmodel->ofsEnd);
	mod->dataSize += size;
	mod->md3[lod] = (md3Header_t *)ri.Hunk_Alloc(size);

	memcpy(mod->md3[lod], buffer, LL(pinmodel->ofsEnd));

	LL(mod->md3[lod]->ident);
	LL(mod->md3[lod]->version);
	LL(mod->md3[lod]->numFrames);
	LL(mod->md3[lod]->numTags);
	LL(mod->md3[lod]->numSurfaces);
	LL(mod->md3[lod]->ofsFrames);
	LL(mod->md3[lod]->ofsTags);
	LL(mod->md3[lod]->ofsSurfaces);
	LL(mod->md3[lod]->ofsEnd);

	if (mod->md3[lod]->numFrames < 1) {
		ri.Printf(PRINT_WARNING, "R_LoadMD3: %s has no frames\n", mod_name);
		return qfalse;
	}

	// swap all the frames
	frame = (md3Frame_t *)((byte *)mod->md3[lod] + mod->md3[lod]->ofsFrames);
	for (i = 0; i < mod->md3[lod]->numFrames; i++, frame++) {
		frame->radius = LF(frame->radius);
		for (j = 0; j < 3; j++) {
			frame->bounds[0][j] = LF(frame->bounds[0][j]);
			frame->bounds[1][j] = LF(frame->bounds[1][j]);
			frame->localOrigin[j] = LF(frame->localOrigin[j]);
		}
	}

	// swap all the tags
	tag = (md3Tag_t *)((byte *)mod->md3[lod] + mod->md3[lod]->ofsTags);
	for (i = 0; i < mod->md3[lod]->numTags * mod->md3[lod]->numFrames; i++, tag++) {
		for (j = 0; j < 3; j++) {
			tag->origin[j] = LF(tag->origin[j]);
			tag->axis[0][j] = LF(tag->axis[0][j]);
			tag->axis[1][j] = LF(tag->axis[1][j]);
			tag->axis[2][j] = LF(tag->axis[2][j]);
		}
	}

	// swap all the surfaces
	surf = (md3Surface_t *)((byte *)mod->md3[lod] + mod->md3[lod]->ofsSurfaces);
	for (i = 0; i < mod->md3[lod]->numSurfaces; i++) {

		LL(surf->ident);
		LL(surf->flags);
		LL(surf->numFrames);
		LL(surf->numShaders);
		LL(surf->numTriangles);
		LL(surf->ofsTriangles);
		LL(surf->numVerts);
		LL(surf->ofsShaders);
		LL(surf->ofsSt);
		LL(surf->ofsXyzNormals);
		LL(surf->ofsEnd);

		if (surf->numVerts > (bQ3RulesApply ? SHADER_MAX_VERTEXES : ACTUAL_SHADER_MAX_VERTEXES)) {
			ri.Error(ERR_DROP, "R_LoadMD3: %s has more than %i verts on a surface (%i)",
				mod_name, (bQ3RulesApply ? SHADER_MAX_VERTEXES : ACTUAL_SHADER_MAX_VERTEXES), surf->numVerts);
		}
		if (surf->numTriangles * 3 > (bQ3RulesApply ? SHADER_MAX_INDEXES : ACTUAL_SHADER_MAX_INDEXES)) {
			ri.Error(ERR_DROP, "R_LoadMD3: %s has more than %i triangles on a surface (%i)",
				mod_name, (bQ3RulesApply ? SHADER_MAX_INDEXES : ACTUAL_SHADER_MAX_INDEXES) / 3, surf->numTriangles);
		}

		// change to surface identifier
		surf->ident = SF_MD3;

		// set pointer to surface in the model surface pointer array
		assert(i != MD3_MAX_SURFACES);
		mod->md3surf[lod][i] = surf;

		// lowercase the surface name so skin compares are faster
		_strlwr(surf->name);

		// strip off a trailing _1 or _2
		// this is a crutch for q3data being a mess
		j = strlen(surf->name);
		if (j > 2 && surf->name[j - 2] == '_') {
			surf->name[j - 2] = 0;
		}

		// register the shaders
		shader = (md3Shader_t *)((byte *)surf + surf->ofsShaders);

		if ((strchr(shader->name, '/') || strchr(shader->name, '\\')))
		{
			shader->shaderIndex = Texture_Load(shader->name);
		}
		else
		{
			shader->shaderIndex = -1;
		}

		// swap all the triangles
		tri = (md3Triangle_t *)((byte *)surf + surf->ofsTriangles);
		for (j = 0; j < surf->numTriangles; j++, tri++) {
			LL(tri->indexes[0]);
			LL(tri->indexes[1]);
			LL(tri->indexes[2]);
		}

		// swap all the ST
		st = (md3St_t *)((byte *)surf + surf->ofsSt);
		for (j = 0; j < surf->numVerts; j++, st++) {
			st->st[0] = LF(st->st[0]);
			st->st[1] = LF(st->st[1]);
		}

		// swap all the XyzNormals
		xyz = (md3XyzNormal_t *)((byte *)surf + surf->ofsXyzNormals);
		for (j = 0; j < surf->numVerts * surf->numFrames; j++, xyz++)
		{
			xyz->xyz[0] = LS(xyz->xyz[0]);
			xyz->xyz[1] = LS(xyz->xyz[1]);
			xyz->xyz[2] = LS(xyz->xyz[2]);

			xyz->normal = LS(xyz->normal);
		}


		// find the next surface
		surf = (md3Surface_t *)((byte *)surf + surf->ofsEnd);
	}

	return qtrue;
}

//======================================================================
//
// Surface Manipulation code				  
void R_MD3RenderSurfaces(surfaceInfo_t *md3_slist, trRefEntity_t *ent, int iLOD)
{
	shader_t	*shader = 0;
	GLuint		gluiTextureBind = 0;

	md3Surface_t	*surface = tr.currentModel->md3surf[iLOD][md3_slist->surface];
	md3Shader_t		*md3Shader = (md3Shader_t *)((byte *)surface + surface->ofsShaders);

	if (AppVars.iSurfaceNumToHighlight)
	{
		md3_slist->surfaceData = (void *)surface;

		if (AppVars.bForceWhite)
		{
			gluiTextureBind = 0;
		}
		else
		{
			if (md3Shader->shaderIndex == -1)
			{
				gluiTextureBind = AnySkin_GetGLBind(ent->e.hModel, md3Shader->name, shader->name);
			}
			else
			{
				gluiTextureBind = Texture_GetGLBind(md3Shader->shaderIndex);
			}

		}

		if (gluiTextureBind != (GLuint)-1)
		{
			R_AddDrawSurf((surfaceType_t *)md3_slist, gluiTextureBind);
		}
	}
}

void R_AddMD3Surfaces(trRefEntity_t *ent)
{
	int				i;
	md3Header_t		*header = 0;
	md3Surface_t	*surface = 0;
	md3Shader_t		*md3Shader = 0;
	shader_t		*shader = 0;
	shader_t		*main_shader = 0;
	int				lod;
	/*MODVIEWREM
	int				fogNum;
	qboolean		personalModel;
	int				cull;

	// don't add third_person objects if not in a portal
	personalModel = (qboolean)((ent->e.renderfx & RF_THIRD_PERSON) && !tr.viewParms.isPortal);
	*/

	if (ent->e.renderfx & RF_CAP_FRAMES) {
		if (ent->e.iFrame_Primary > tr.currentModel->md3[0]->numFrames - 1)
			ent->e.iFrame_Primary = tr.currentModel->md3[0]->numFrames - 1;
		if (ent->e.iOldFrame_Primary > tr.currentModel->md3[0]->numFrames - 1)
			ent->e.iOldFrame_Primary = tr.currentModel->md3[0]->numFrames - 1;
	}
	else if (ent->e.renderfx & RF_WRAP_FRAMES) {
		ent->e.iFrame_Primary %= tr.currentModel->md3[0]->numFrames;
		ent->e.iOldFrame_Primary %= tr.currentModel->md3[0]->numFrames;
	}

	//
	// Validate the frames so there is no chance of a crash.
	// This will write directly into the entity structure, so
	// when the surfaces are rendered, they don't need to be
	// range checked again.
	//
	if ((ent->e.iFrame_Primary >= tr.currentModel->md3[0]->numFrames)
		|| (ent->e.iFrame_Primary < 0)
		|| (ent->e.iOldFrame_Primary >= tr.currentModel->md3[0]->numFrames)
		|| (ent->e.iOldFrame_Primary < 0))
	{
		ri.Printf(PRINT_ALL, "R_AddMD3Surfaces: no such frame %d to %d for '%s'\n",
			ent->e.iOldFrame_Primary, ent->e.iFrame_Primary,
			tr.currentModel->name);
		ent->e.iFrame_Primary = 0;
		ent->e.iOldFrame_Primary = 0;
	}

	//
	// compute LOD
	//
	lod = R_ComputeLOD(ent);	

	header = tr.currentModel->md3[lod];

	extern ModelContainer_t* gpContainerBeingRendered;
	if (gpContainerBeingRendered)
	{
		gpContainerBeingRendered->iNumLODs = lod + 1; // + 1 so the LOD count text isn't screwed
	}

	/*MODVIEWREM
	//
	// cull the entire model if merged bounding box of both frames
	// is outside the view frustum.
	//
	cull = R_CullModel(header, ent);
	if (cull == CULL_OUT) {
		return;
	}

	//
	// set up lighting now that we know we aren't culled
	//
	if (!personalModel || r_shadows->integer > 1) {
		R_SetupEntityLighting(&tr.refdef, ent);
	}

	//
	// see if we are in a fog volume
	//
	fogNum = R_ComputeFogNum(header, ent);

	//
	// draw all surfaces
	//
	main_shader = R_GetShaderByHandle(ent->e.customShader);
	*/

	surface = (md3Surface_t *)((byte *)header + header->ofsSurfaces);
	for (i = 0; i < header->numSurfaces; i++) 
	{
		/*MODVIEWREM
		if (ent->e.customShader) {// a little more efficient
			shader = main_shader;
		}
		else if (ent->e.customSkin > 0 && ent->e.customSkin < tr.numSkins) {
			skin_t *skin;
			int		j;

			skin = R_GetSkinByHandle(ent->e.customSkin);

			// match the surface name to something in the skin file
			shader = tr.defaultShader;
			for (j = 0; j < skin->numSurfaces; j++) {
				// the names have both been lowercased
				if (!strcmp(skin->surfaces[j]->name, surface->name)) {
					shader = skin->surfaces[j]->shader;
					break;
				}
			}
		}
		else if (surface->numShaders <= 0) {
			shader = tr.defaultShader;
		}
		else {
			md3Shader = (md3Shader_t *)((byte *)surface + surface->ofsShaders);
			md3Shader += ent->e.skinNum % surface->numShaders;
			shader = tr.shaders[md3Shader->shaderIndex];
		}
		*/

		surface = (md3Surface_t *)((byte *)surface + surface->ofsEnd);
	}

	for (int iSurfaceIndex = 0; iSurfaceIndex < header->numSurfaces; iSurfaceIndex++)
	{
		R_MD3RenderSurfaces(&ent->e.md3_slist[iSurfaceIndex], ent, lod);
	}
}

/*
** LerpMeshVertexes
*/
static void LerpMeshVertexes(md3Surface_t *surf, float backlerp)
{
	short	*oldXyz, *newXyz, *oldNormals, *newNormals;
	float	*outXyz, *outNormal;
	float	oldXyzScale, newXyzScale;
	float	oldNormalScale, newNormalScale;
	int		vertNum;
	unsigned lat, lng;
	int		numVerts;

	outXyz = tess.xyz[tess.numVertexes];
	outNormal = tess.normal[tess.numVertexes];

	newXyz = (short *)((byte *)surf + surf->ofsXyzNormals)
		+ (tr.currentEntity->e.iFrame_Primary * surf->numVerts * 4);
	newNormals = newXyz + 3;

	newXyzScale = MD3_XYZ_SCALE * (1.0 - backlerp);
	newNormalScale = 1.0 - backlerp;

	numVerts = surf->numVerts;

	if (backlerp == 0) {
		//
		// just copy the vertexes
		//
		for (vertNum = 0; vertNum < numVerts; vertNum++,
			newXyz += 4, newNormals += 4,
			outXyz += 4, outNormal += 4)
		{

			outXyz[0] = newXyz[0] * newXyzScale;
			outXyz[1] = newXyz[1] * newXyzScale;
			outXyz[2] = newXyz[2] * newXyzScale;

			lat = (newNormals[0] >> 8) & 0xff;
			lng = (newNormals[0] & 0xff);
			lat *= (FUNCTABLE_SIZE / 256);
			lng *= (FUNCTABLE_SIZE / 256);

			// decode X as cos( lat ) * sin( long )
			// decode Y as sin( lat ) * sin( long )
			// decode Z as cos( long )
			outNormal[0] = tr.sinTable[(lat + (FUNCTABLE_SIZE / 4))&FUNCTABLE_MASK] * tr.sinTable[lng];
			outNormal[1] = tr.sinTable[lat] * tr.sinTable[lng];
			outNormal[2] = tr.sinTable[(lng + (FUNCTABLE_SIZE / 4))&FUNCTABLE_MASK];
		}
	}
	else {
		//
		// interpolate and copy the vertex and normal
		//
		oldXyz = (short *)((byte *)surf + surf->ofsXyzNormals)
			+ (tr.currentEntity->e.iOldFrame_Primary * surf->numVerts * 4);
		oldNormals = oldXyz + 3;

		oldXyzScale = MD3_XYZ_SCALE * backlerp;
		oldNormalScale = backlerp;

		for (vertNum = 0; vertNum < numVerts; vertNum++,
			oldXyz += 4, newXyz += 4, oldNormals += 4, newNormals += 4,
			outXyz += 4, outNormal += 4)
		{
			vec3_t uncompressedOldNormal, uncompressedNewNormal;

			// interpolate the xyz
			outXyz[0] = oldXyz[0] * oldXyzScale + newXyz[0] * newXyzScale;
			outXyz[1] = oldXyz[1] * oldXyzScale + newXyz[1] * newXyzScale;
			outXyz[2] = oldXyz[2] * oldXyzScale + newXyz[2] * newXyzScale;

			// FIXME: interpolate lat/long instead?
			lat = (newNormals[0] >> 8) & 0xff;
			lng = (newNormals[0] & 0xff);
			lat *= 4;
			lng *= 4;
			uncompressedNewNormal[0] = tr.sinTable[(lat + (FUNCTABLE_SIZE / 4))&FUNCTABLE_MASK] * tr.sinTable[lng];
			uncompressedNewNormal[1] = tr.sinTable[lat] * tr.sinTable[lng];
			uncompressedNewNormal[2] = tr.sinTable[(lng + (FUNCTABLE_SIZE / 4))&FUNCTABLE_MASK];

			lat = (oldNormals[0] >> 8) & 0xff;
			lng = (oldNormals[0] & 0xff);
			lat *= 4;
			lng *= 4;

			uncompressedOldNormal[0] = tr.sinTable[(lat + (FUNCTABLE_SIZE / 4))&FUNCTABLE_MASK] * tr.sinTable[lng];
			uncompressedOldNormal[1] = tr.sinTable[lat] * tr.sinTable[lng];
			uncompressedOldNormal[2] = tr.sinTable[(lng + (FUNCTABLE_SIZE / 4))&FUNCTABLE_MASK];

			outNormal[0] = uncompressedOldNormal[0] * oldNormalScale + uncompressedNewNormal[0] * newNormalScale;
			outNormal[1] = uncompressedOldNormal[1] * oldNormalScale + uncompressedNewNormal[1] * newNormalScale;
			outNormal[2] = uncompressedOldNormal[2] * oldNormalScale + uncompressedNewNormal[2] * newNormalScale;

			VectorNormalize(outNormal);
		}
	}
}

/*
=============
RB_SurfaceMesh
=============
*/
float VectorNormalize(vec3_t vec);
void RB_SurfaceMesh(surfaceInfo_t *surf) {
	int				j;
	int				*triangles;
	float			*texCoords;
	int				indexes;
	int				Bob, Doug;
	int				numVerts;

	//MODVIEWREM
	float			backlerp;

	if (tr.currentEntity->e.iOldFrame_Primary == tr.currentEntity->e.iFrame_Primary) {
		backlerp = 0;
	}
	else {
		backlerp = tr.currentEntity->e.backlerp;
	}
	

// grab the pointer to the surface info within the loaded mesh file
	md3Surface_t	*surface = (md3Surface_t *)surf->surfaceData;

	// stats...
	//
	giSurfaceVertsDrawn = surface->numVerts;
	giSurfaceTrisDrawn = surface->numTriangles;

	// first up, sanity check our numbers
	RB_CheckOverflow(surface->numVerts, surface->numTriangles);

	LerpMeshVertexes(surface, backlerp);

	// now copy the right number of verts to the temporary area for verts for this shader
	triangles = (int *)((byte *)surface + surface->ofsTriangles);
	indexes = surface->numTriangles * 3;
	Bob = tess.numIndexes;
	Doug = tess.numVertexes;
	for (j = 0; j < indexes; j++) {
		tess.indexes[Bob + j] = Doug + triangles[j];
	}
	tess.numIndexes += indexes;

	texCoords = (float *)((byte *)surface + surface->ofsSt);

	// whip through and actually transform each vertex
	numVerts = surface->numVerts;

	for (j = 0; j < numVerts; j++) {
		tess.texCoords[Doug + j][0][0] = texCoords[j * 2 + 0];
		tess.texCoords[Doug + j][0][1] = texCoords[j * 2 + 1];
		// FIXME: fill in lightmapST for completeness?
	}

	tess.numVertexes += surface->numVerts;
}

qboolean MD3_SetSurface(qhandle_t model, surfaceInfo_t *md3_slist, const char *surfaceName, const int surface)
{
	int i;
	md3Surface_t		*surf;
	model_t				*mod;

	// find the model we want
	mod = R_GetModelByHandle(model);

	// did we find an MD3 model or not?
	if (!mod->md3)
	{
		assert(0);
		return qfalse;
	}

	// first find if we already have this surface in the list
	for (i = 0; i<MD3_MAX_SURFACES; i++)
	{
		// are we at the end of the list?
		if (md3_slist[i].surface == -1)
		{
			break;
		}

		surf = mod->md3surf[0][md3_slist[i].surface];

		// same name as one already in?
		if (!_stricmp(surf->name, surfaceName))
		{
			assert(surface == i);
			return qtrue;
		}
	}

	// run out of space?
	if (i == MD3_MAX_SURFACES)
	{
		assert(0);
		return qfalse;
	}

	assert(surface == i);

	if (surface != i)
	{
		// fill this in later 
		//ri.Error (ERR_DROP, "R_LoadMD3: %s has more than %i verts on a surface (%i)",
	}


	// insert here then
	md3_slist[i].surface = surface;
	md3_slist[i].ident = SF_MD3;
	md3_slist[i].surfaceData = (void *)mod->md3surf[0][surface];

	// if we can, set the next surface pointer to a -1 to make the walking of the lists faster
	if (i + 1 < MD3_MAX_SURFACES)
	{
		md3_slist[i + 1].surface = -1;
	}
	return qtrue;
}

void MD3_GetSurfaceList(qhandle_t model, surfaceInfo_t *md3_slist)
{
	model_t				*mod;
	md3Shader_t			*surf;
	int					i;
	int					lod = 0;

	// init the surface list
	memset(md3_slist, 0, sizeof(md3_slist) * MD3_MAX_SURFACES);
	md3_slist[0].surface = -1;

	// find the model we want
	mod = R_GetModelByHandle(model);

	// did we find a ghoul 2 model or not?
	if (!mod->md3)
	{
		return;
	}

	// set up pointers to surface info
	surf = (md3Shader_t *)((byte *)mod->md3 + mod->md3[lod]->ofsSurfaces);
	//mdxmLODSurfOffset_t * pLODSurfOffset = (mdxmLODSurfOffset_t *)((byte *)mod->mdxm + mod->mdxm->ofsLODs + sizeof(mdxmLOD_t));

	for (i = 0; i < mod->md3[lod]->numSurfaces; i++)
	{
		md3Surface_t *surface = (md3Surface_t *)((byte*)mod->md3 + mod->md3[lod]->ofsSurfaces);
		MD3_SetSurface(model, md3_slist, surface->name, i);
		// find the next surface
		surface = (md3Surface_t *)((byte *)surface + surface->ofsEnd);
	}
}

//////////////////// eof ///////////

