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



void R_AddMD3Surfaces( trRefEntity_t *ent )
{
	int				i;
	md3Header_t		*header = 0;
	md3Surface_t	*surface = 0;
	md3Shader_t		*md3Shader = 0;
	shader_t		*shader = 0;
	shader_t		*main_shader = 0;
	int				lod;
	int				fogNum;
/*MODVIEWREM
	qboolean		personalModel;
	int				cull;
*/

	// don't add third_person objects if not in a portal
//MODVIEWREM	personalModel = (qboolean)((ent->e.renderfx & RF_THIRD_PERSON) && !tr.viewParms.isPortal);

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
}

/*
=============
RB_SurfaceMesh
=============
*/
void RB_SurfaceMesh(md3Surface_t *surface) {
	int				j;
	float			backlerp;
	int				*triangles;
	float			*texCoords;
	int				indexes;
	int				Bob, Doug;
	int				numVerts;

/*MODVIEWREM
	if (backEnd.currentEntity->e.oldframe == backEnd.currentEntity->e.frame) {
		backlerp = 0;
	}
	else {
		backlerp = backEnd.currentEntity->e.backlerp;
	}
*/

	RB_CheckOverflow(surface->numVerts, surface->numTriangles * 3);

	//LerpMeshVertexes(surface, backlerp);

	triangles = (int *)((byte *)surface + surface->ofsTriangles);
	indexes = surface->numTriangles * 3;
	Bob = tess.numIndexes;
	Doug = tess.numVertexes;
	for (j = 0; j < indexes; j++) {
		tess.indexes[Bob + j] = Doug + triangles[j];
	}
	tess.numIndexes += indexes;

	texCoords = (float *)((byte *)surface + surface->ofsSt);

	numVerts = surface->numVerts;
	for (j = 0; j < numVerts; j++) {
		tess.texCoords[Doug + j][0][0] = texCoords[j * 2 + 0];
		tess.texCoords[Doug + j][0][1] = texCoords[j * 2 + 1];
		// FIXME: fill in lightmapST for completeness?
	}

	tess.numVertexes += surface->numVerts;
}

LPCSTR MD3Model_GetSurfaceName(ModelHandle_t hModel, int iSurfaceIndex)
{
	md3Header_t	*pMD3Header = (md3Header_t	*)RE_GetModelData(hModel);

	assert(iSurfaceIndex < pMD3Header->numSurfaces);
	if (iSurfaceIndex < pMD3Header->numSurfaces)
	{
		md3Surface_t *pSurf = (md3Surface_t *)((byte *)pMD3Header + pMD3Header->ofsSurfaces);
		return pSurf->name;
	}

	return "MD3Model_GetSurfaceName(): Bad surface index";
}

// interesting use of static here, this function IS called externally, but only through a ptr. 
//	This is to stop people accessing it directly.
//
// return basic info on the supplied model arg...
//
static LPCSTR MD3Model_Info(ModelHandle_t hModel)
{
	// I should really try-catch these, but for now...
	//
	md3Header_t	*pMD3Header = (md3Header_t	*)RE_GetModelData(hModel);

	static string str;

	ModelContainer_t *pContainer = ModelContainer_FindFromModelHandle(hModel);

	str = va("Model: %s%s\n\n", Model_GetFilename(hModel), !pContainer ? "" : !pContainer->pBoneBolt_ParentContainer ? (!pContainer->pSurfaceBolt_ParentContainer ? "" : va("   ( Bolted to parent model via surface bolt '%s' )", Model_GetBoltName(pContainer->pSurfaceBolt_ParentContainer, pContainer->iSurfaceBolt_ParentBoltIndex, false))) : va("   ( Bolted to parent model via bone bolt '%s' )", Model_GetBoltName(pContainer->pBoneBolt_ParentContainer, pContainer->iBoneBolt_ParentBoltIndex, true)));
	str += va("MD3 Info:\t\t( FileSize: %d bytes )\n\n", pMD3Header->ofsEnd);
	str += va("    ->ident :\t%X\n", pMD3Header->ident);	// extra space before ':' here to push tab to next column
	str += va("    ->version:\t%d\n", pMD3Header->version);
	str += va("    ->name:\t%s\n", pMD3Header->name);
	str += va("    ->numFrames:\t%d\n", pMD3Header->numFrames);
	str += va("    ->numSurfaces:\t%d\n", pMD3Header->numSurfaces);
	str += va("    ->numTags:\t%d\n", pMD3Header->numTags);
	//str += va("    ->numLODs:\t%d\n", pMD3Header->numLODs);

	// show shader usage...
	//
	if (pContainer->OldSkinSets.size())
	{
		// model is using old EF1/ID type skins...
		//
		str += va("\n\nSkin Info:\n\nSkin File:\t\t%s\n", pContainer->strCurrentSkinFile.c_str());
	}
	/*else
	{
		// standard shaders...
		//
		int iUniqueShaderCount = GLMModel_GetUniqueShaderCount(hModel);
		str += va("\n\nShader Info:\t( %d unique shaders )\n\n", iUniqueShaderCount);
		for (int iUniqueShader = 0; iUniqueShader < iUniqueShaderCount; iUniqueShader++)
		{
			bool bFound = false;

			LPCSTR	psShaderName = GLMModel_GetUniqueShader(iUniqueShader);
			LPCSTR	psLocalTexturePath = R_FindShader(psShaderName);

			if (psLocalTexturePath && strlen(psLocalTexturePath))
			{
				TextureHandle_t hTexture = TextureHandle_ForName(psLocalTexturePath);
				GLuint			uiBind = (hTexture == -1) ? 0 : Texture_GetGLBind(hTexture);
				bFound = (uiBind || !_stricmp(psShaderName, "[NoMaterial]"));
			}
			str += va("     %s%s\n", String_EnsureMinLength(psShaderName[0] ? psShaderName : "<blank>", 16), (!bFound) ? "\t(Not Found)" : "");
		}
	}
	*/

	return str.c_str();
}

bool MD3Model_SurfaceIsTag(ModelHandle_t hModel, int iSurfaceIndex)
{
	md3Header_t	*pMD3Header = (md3Header_t	*)RE_GetModelData(hModel);

	assert(iSurfaceIndex < pMD3Header->numSurfaces);
	if (iSurfaceIndex < pMD3Header->numSurfaces)
	{
		md3Surface_t *pSurf = (md3Surface_t *)((byte *)pMD3Header + pMD3Header->ofsSurfaces);
		return (pSurf->flags & MD3SURFACEFLAG_ISBOLT);
	}

	return false;
}

// Note, this function is only really supposed to be called once, to setup the Container that owns this model
//
static int MD3Model_GetNumFrames(ModelHandle_t hModel)
{
	md3Header_t	*pMD3Header = (md3Header_t	*)RE_GetModelData(hModel);

	return pMD3Header->numFrames;
}

// Note, this function is only really supposed to be called once, to setup the Container that owns this model
//
static int MD3Model_GetNumSurfaces(ModelHandle_t hModel)
{
	md3Header_t	*pMD3Header = (md3Header_t	*)RE_GetModelData(hModel);

	return pMD3Header->numSurfaces;
}

// provides common functionality to save duping logic...
//
static LPCSTR MD3Model_CreateSurfaceName(LPCSTR psSurfaceName)
{
	static CString string;

	string = psSurfaceName;	// do NOT use in constructor form since this is a static (that got me first time... :-)

	return (LPCSTR)string;
}

// call this to re-evaluate any part of the tree that has surfaces owned by this model, and set their text ok...
//
// hTreeItem = tree item to start from, pass NULL to start from root
//
bool R_MD3Model_Tree_ReEvalSurfaceText(ModelHandle_t hModel, HTREEITEM hTreeItem /* = NULL */)
{
	bool bReturn = false;

	if (!hTreeItem)
		hTreeItem = ModelTree_GetRootItem();

	if (hTreeItem)
	{
		// process this tree item...
		//
		TreeItemData_t	TreeItemData;
		TreeItemData.uiData = ModelTree_GetItemData(hTreeItem);

		if (TreeItemData.iModelHandle == hModel)
		{
			// ok, tree item belongs to this model, so what is it?...
			//
			ModelContainer_t *pContainer = ModelContainer_FindFromModelHandle(hModel);
			if (pContainer)
			{
				if (TreeItemData.iItemType == TREEITEMTYPE_MD3_SURFACE)
				{
					// it's a surface, so re-eval its text...
					//
					LPCSTR psSurfaceName = MD3Model_GetSurfaceName(hModel, TreeItemData.iItemNumber);

					// a little harmless optimisation here...
					//
					//ModelTree_SetItemText(hTreeItem, MD3Model_CreateSurfaceName(psSurfaceName));
				}
			}

			// process siblings...
			//
			HTREEITEM hTreeItem_Sibling = ModelTree_GetNextSiblingItem(hTreeItem);
			if (hTreeItem_Sibling)
				R_MD3Model_Tree_ReEvalSurfaceText(hModel, hTreeItem_Sibling);
		}
	}

	return true;
}

// read an optional set of skin files, and if present, add them into the model tree...
//
// return is success/fail (but it's an optional file, so return bool is just FYI really)
//    (note that partial failures still count as successes, as long as at least one file succeeds)
//
static bool MD3Model_ReadSkinFiles(HTREEITEM hParent, ModelContainer_t *pContainer, LPCSTR psLocalFilename)
{
	// check for optional .skin files... (CHC-type)
	//
	if (OldSkins_Read(psLocalFilename, pContainer))
	{
		return OldSkins_ApplyToTree(hParent, pContainer);
	}

	return false;
}

static bool R_MD3_AddSurfaceToTree(ModelHandle_t hModel, HTREEITEM htiParent, int iThisSurfaceIndex, bool bTagsOnly)
{
	bool bReturn = true;

	md3Header_t	*pMD3Header = (md3Header_t	*)RE_GetModelData(hModel);
	md3Surface_t *pSurf = (md3Surface_t *)((byte *)pMD3Header + pMD3Header->ofsSurfaces);

	// insert me...
	//
	TreeItemData_t	TreeItemData				= { 0 };
					TreeItemData.iItemType		= bTagsOnly ? TREEITEMTYPE_MD3_TAGSURFACE : TREEITEMTYPE_MD3_SURFACE;
					TreeItemData.iModelHandle	= hModel;
					TreeItemData.iItemNumber	= iThisSurfaceIndex;

	HTREEITEM htiThis = NULL;
	if (!bTagsOnly || (pSurf->flags & MD3SURFACEFLAG_ISBOLT) )
	{
		htiThis = ModelTree_InsertItem(MD3Model_CreateSurfaceName(pSurf->name),	// LPCTSTR psName, 
										htiParent,			// HTREEITEM hParent
										TreeItemData.uiData,// TREEITEMTYPE_MD3_SURFACE | iThisSurfaceIndex	// UINT32 uiUserData
										bTagsOnly ? TVI_SORT : TVI_LAST
										);
	}

	// insert the next surface...
	//
	//pSurf = (md3Surface_t *)((byte *)pSurf + pSurf->ofsEnd);

	//R_MD3_AddSurfaceToTree(hModel, bTagsOnly ? htiParent : htiThis, pSurf->ofsEnd, bTagsOnly);

	return bReturn;
}

// if we get this far now then we no longer need to check the model data because RE_RegisterModel has already 
//	validated it. Oh well...
//
// this MUST be called after Jake's code has finished, since I read from his tables...
//
bool MD3Model_Parse(struct ModelContainer *pContainer, LPCSTR psLocalFilename, HTREEITEM hTreeItem_Parent /* = NULL */)
{
	bool bReturn = false;

	ModelHandle_t hModel = pContainer->hModel;

	md3Header_t	*pMD3Header = (md3Header_t	*)RE_GetModelData(hModel);

	HTREEITEM hTreeItem_Bones = NULL;

	if (pMD3Header->ident == MD3_IDENT)
	{
		if (pMD3Header->version == MD3_VERSION)
		{
			// phew, all systems go...
			//
			bReturn = true;

			TreeItemData_t	TreeItemData = { 0 };
			TreeItemData.iModelHandle = hModel;

			TreeItemData.iItemType = TREEITEMTYPE_MODELNAME;
			pContainer->hTreeItem_ModelName = ModelTree_InsertItem(va("==>  %s  <==", Filename_WithoutPath(/*Filename_WithoutExt*/(psLocalFilename))), hTreeItem_Parent, TreeItemData.uiData);

			TreeItemData.iItemType = TREEITEMTYPE_SURFACEHEADER;
			HTREEITEM hTreeItem_Surfaces = ModelTree_InsertItem("Surfaces", pContainer->hTreeItem_ModelName, TreeItemData.uiData);

			TreeItemData.iItemType = TREEITEMTYPE_TAGSURFACEHEADER;
			HTREEITEM hTreeItem_TagSurfaces = ModelTree_InsertItem("Tag Surfaces", pContainer->hTreeItem_ModelName, TreeItemData.uiData);

			R_MD3_AddSurfaceToTree(hModel, hTreeItem_Surfaces, 0, false);
			R_MD3_AddSurfaceToTree(hModel, hTreeItem_TagSurfaces, 0, true);

			/*if (!ModelTree_ItemHasChildren(hTreeItem_TagSurfaces))
			{
				ModelTree_DeleteItem(hTreeItem_TagSurfaces);
			}*/
		}
		else
		{
			ErrorBox(va("Wrong model format version number: %d (expecting %d)", pMD3Header->version, MD3_VERSION));
		}
	}
	else
	{
		ErrorBox(va("Wrong model Ident: %X (expecting %X)", pMD3Header->ident, MD3_IDENT));
	}

	if (bReturn)
	{
		bReturn = R_MD3Model_Tree_ReEvalSurfaceText(hModel);

		if (bReturn)
		{
			// let's try looking for "<modelname>.frames" in the same dir for simple sequence info...
			//
			{
				// now fill in the fields we need in the container to avoid GLM-specific queries...
				//
				pContainer->pModelInfoFunction = MD3Model_Info;
				//pContainer->pModelGetBoneNameFunction = GLMModel_GetBoneName;
				//pContainer->pModelGetBoneBoltNameFunction = GLMModel_GetBoneName;	// same thing in this format
				pContainer->pModelGetSurfaceNameFunction = MD3Model_GetSurfaceName;
				pContainer->pModelGetSurfaceBoltNameFunction = MD3Model_GetSurfaceName;	// same thing in this format
				pContainer->iNumFrames = MD3Model_GetNumFrames(hModel);
				//pContainer->iNumLODs = GLMModel_GetNumLODs(hModel);
				//pContainer->iNumBones = GLMModel_GetNumBones(hModel);
				pContainer->iNumSurfaces = MD3Model_GetNumSurfaces(hModel);

				//pContainer->iBoneBolt_MaxBoltPoints = pContainer->iNumBones;	// ... since these are pretty much the same in this format
				pContainer->iSurfaceBolt_MaxBoltPoints = pContainer->iNumSurfaces;	// ... since these are pretty much the same in this format

				MD3Model_ReadSkinFiles(pContainer->hTreeItem_ModelName, pContainer, psLocalFilename);
				//GLMModel_ReadSequenceInfo(pContainer->hTreeItem_ModelName, pContainer, pMDXMHeader->animName);
				//GLMModel_ReadBoneAliasFile(pContainer->hTreeItem_ModelName, hTreeItem_Bones, pContainer, pMDXMHeader->animName);
			}
		}
	}

	return bReturn;
}


//////////////////// eof ///////////

