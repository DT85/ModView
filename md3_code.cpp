// Filename:-	md3_code.cpp
//

#include "stdafx.h"
#include "includes.h"
#include "special_defines.h"
//
#include "R_Model.h"
#include "md3_format.h"
#include "shader.h"
#include "skins.h"
#include "textures.h"	// for some stats stuff
//
#include "md3_code.h"


LPCSTR MD3Model_GetSurfaceName(ModelHandle_t hModel, int iSurfaceIndex)
{
	model_t	*mod = R_GetModelByHandle(hModel);
	ModelContainer_t *pContainer = ModelContainer_FindFromModelHandle(hModel);

	md3Surface_t *pSurf = mod->md3surf[0][pContainer->md3_slist->surface];

	assert(iSurfaceIndex < mod->md3[0]->numSurfaces);
	if (iSurfaceIndex < mod->md3[0]->numSurfaces)
	{
		pSurf = mod->md3surf[0][pContainer->md3_slist[iSurfaceIndex].surface];

		return pSurf->name;
	}

	return "MD3Model_GetSurfaceName(): Bad surface index";
}

LPCSTR MD3Model_GetTagName(ModelHandle_t hModel, int iTagIndex)
{
	model_t	*mod = R_GetModelByHandle(hModel);
	ModelContainer_t *pContainer = ModelContainer_FindFromModelHandle(hModel);

	md3Tag_t *pTag = mod->md3tag[0][pContainer->md3_tlist->tag];

	assert(iTagIndex < mod->md3[0]->numTags);
	if (iTagIndex < mod->md3[0]->numTags)
	{
		pTag = mod->md3tag[0][pContainer->md3_tlist[iTagIndex].tag];

		return pTag->name;
	}

	return "MD3Model_GetTagName(): Bad tag index";
}

LPCSTR MD3Model_GetSurfaceShaderName(ModelHandle_t hModel, int iSurfaceIndex)
{
	model_t	*mod = R_GetModelByHandle(hModel);
	ModelContainer_t *pContainer = ModelContainer_FindFromModelHandle(hModel);
	md3Header_t		*pMD3Header = mod->md3[0];
	md3Surface_t	*pMD3Surface = mod->md3surf[0][pContainer->md3_slist->surface];
	md3Shader_t		*pMD3Shader = (md3Shader_t *)((byte *)pMD3Surface + pMD3Surface->ofsShaders);

	assert(iSurfaceIndex < pMD3Header->numSurfaces);
	if (iSurfaceIndex < pMD3Header->numSurfaces)
	{
		pMD3Surface = mod->md3surf[0][pContainer->md3_slist[iSurfaceIndex].surface];
		return pMD3Shader->name;
	}

	assert(0);
	return "MD3Model_GetSurfaceShaderName(): Bad surface index";
}

LPCSTR MD3Model_SurfaceInfo(ModelHandle_t hModel, int iSurfaceIndex)
{
	model_t			*mod = R_GetModelByHandle(hModel);
	ModelContainer_t *pContainer = ModelContainer_FindFromModelHandle(hModel);
	md3Header_t		*pMD3Header = mod->md3[0];
	md3Surface_t	*pMD3Surface = mod->md3surf[0][pContainer->md3_slist[iSurfaceIndex].surface];
	md3Shader_t		*pMD3Shader = (md3Shader_t *)((byte *)pMD3Surface + pMD3Surface->ofsShaders);

	assert(iSurfaceIndex < pMD3Header->numSurfaces);

	static string str;

	str = va("Surface %d/%d:  '%s'\n\n", iSurfaceIndex, pMD3Header->numSurfaces, pMD3Surface->name);
	str += va("    Shader:\t%s\n", pMD3Shader->name);
	str += "\n";

	for (int iLOD = 0; iLOD < mod->numLods; iLOD++)
	{
		pMD3Surface = mod->md3surf[iLOD][pContainer->md3_slist[iSurfaceIndex].surface];

		str += va("    LOD %d/%d:\n", iLOD, mod->numLods);
		str += va("        # Verts:\t%d\n", pMD3Surface->numVerts);
		str += va("        # Tris:\t%d\n", pMD3Surface->numTriangles);
		str += "\n";
	}

	return str.c_str();
}

static LPCSTR MD3Model_CreateSurfaceName(LPCSTR psSurfaceName)
{
	static CString string;

	string = psSurfaceName;	// Do NOT use in constructor form since this is a static (that got me first time... :-)

	return (LPCSTR)string;
}

static bool R_MD3_AddSurfaceToTree(ModelHandle_t hModel, HTREEITEM htiParent, int iThisSurfaceIndex, LPCSTR ThisSurfaceName)
{
	bool bReturn = true;

	TreeItemData_t	TreeItemData				= { 0 };
					TreeItemData.iItemType		= TREEITEMTYPE_MD3_SURFACE;
					TreeItemData.iModelHandle	= hModel;
					TreeItemData.iItemNumber	= iThisSurfaceIndex;

	HTREEITEM htiThis = NULL;

	htiThis = ModelTree_InsertItem(MD3Model_CreateSurfaceName(ThisSurfaceName),	// LPCTSTR psName, 
									htiParent,			// HTREEITEM hParent
									TreeItemData.uiData, // TREEITEMTYPE_MD3_SURFACE | iThisSurfaceIndex	// UINT32 uiUserData
									TVI_LAST
									);

	return bReturn;
}

static bool R_MD3_AddTagToTree(ModelHandle_t hModel, HTREEITEM htiParent, int iThisTagIndex, LPCSTR ThisTagName)
{
	bool bReturn = true;

	TreeItemData_t	TreeItemData = { 0 };
					TreeItemData.iItemType = TREEITEMTYPE_MD3_TAGSURFACE;
					TreeItemData.iModelHandle = hModel;
					TreeItemData.iItemNumber = iThisTagIndex;

	HTREEITEM htiThis = NULL;

	htiThis = ModelTree_InsertItem(MD3Model_CreateSurfaceName(ThisTagName),	// LPCTSTR psName, 
									htiParent,			// HTREEITEM hParent
									TreeItemData.uiData, // TREEITEMTYPE_MD3_SURFACE | iThisSurfaceIndex	// UINT32 uiUserData
									TVI_SORT
	);

	return bReturn;
}

// Note, this function is only really supposed to be called once, to setup the Container that owns this model
static int MD3Model_GetNumFrames(ModelHandle_t hModel)
{
	md3Header_t	*pMD3Header = (md3Header_t	*)RE_GetModelData(hModel);

	return pMD3Header->numFrames;
}

// Note, this function is only really supposed to be called once, to setup the Container that owns this model
static int MD3Model_GetNumSurfaces(ModelHandle_t hModel)
{
	md3Header_t	*pMD3Header = (md3Header_t	*)RE_GetModelData(hModel);

	return pMD3Header->numSurfaces;
}

// Note, this function is only really supposed to be called once, to setup the Container that owns this model
static int MD3Model_GetNumTags(ModelHandle_t hModel)
{
	md3Header_t	*pMD3Header = (md3Header_t	*)RE_GetModelData(hModel);

	return pMD3Header->numTags;
}

// Note, this function is only really supposed to be called once, to setup the Container that owns this model
static int MD3Model_GetNumLODs(ModelHandle_t hModel)
{
	model_t	*mod = R_GetModelByHandle(hModel);
	
	return mod->numLods;
}

// These next 2 functions are closely related, the GetCount function fills in public data which the other reads on query
extern set <string> stringSet;
static int MD3Model_GetUniqueShaderCount(ModelHandle_t hModel)
{
	md3Header_t		*pMD3Header = (md3Header_t	*)RE_GetModelData(hModel);
	md3Surface_t	*pMD3Surface = (md3Surface_t *)((byte *)pMD3Header + pMD3Header->ofsSurfaces);

	stringSet.clear();

	for (int iSurfaceIndex = 0; iSurfaceIndex < pMD3Header->numSurfaces; iSurfaceIndex++)
	{
		md3Shader_t		*pMD3Shader = (md3Shader_t *)((byte *)pMD3Surface + pMD3Surface->ofsShaders);
		string			strShader(pMD3Shader->name);

		stringSet.insert(stringSet.end(), strShader);

		// Onto the next ones
		pMD3Surface = (md3Surface_t *)((byte *)pMD3Surface + pMD3Surface->ofsEnd);

		stringSet.insert(stringSet.end(), strShader);
	}

	return stringSet.size();
}

static LPCSTR MD3Model_GetUniqueShader(int iShader)
{
	assert(iShader < stringSet.size());

	for (set <string>::iterator it = stringSet.begin(); it != stringSet.end(); ++it)
	{
		if (!iShader--)
		{
			return (*it).c_str();
		}
	}

	return "(Error)"; // Should never get here.
}

// Interesting use of static here, this function IS called externally, but only through a ptr. 
// This is to stop people accessing it directly.
//
// Return basic info on the supplied model arg...
static LPCSTR MD3Model_Info(ModelHandle_t hModel)
{
	model_t	*mod = R_GetModelByHandle(hModel);
	md3Header_t	*pMD3Header = (md3Header_t	*)RE_GetModelData(hModel);

	static string str;

	ModelContainer_t *pContainer = ModelContainer_FindFromModelHandle(hModel);

	str = va("Model: %s%s\n\n", Model_GetFilename(hModel), !pContainer ? "" : !pContainer->pBoneBolt_ParentContainer ? (!pContainer->pSurfaceBolt_ParentContainer ? "" : va("   ( Bolted to parent model via surface bolt '%s' )", Model_GetBoltName(pContainer->pSurfaceBolt_ParentContainer, pContainer->iSurfaceBolt_ParentBoltIndex, false))) : va("   ( Bolted to parent model via bone bolt '%s' )", Model_GetBoltName(pContainer->pBoneBolt_ParentContainer, pContainer->iBoneBolt_ParentBoltIndex, true)));
	str += va("MD3 Info:\t\t( FileSize: %d bytes )\n\n", pMD3Header->ofsEnd);
	str += va("    ->ident :\t%X\n", pMD3Header->ident);	// extra space before ':' here to push tab to next column
	str += va("    ->version:\t%d\n", pMD3Header->version);
	str += va("    ->name:\t%s\n", pMD3Header->name);
	str += va("    ->numFrames:\t%d\n", pMD3Header->numFrames);
	str += va("    ->numLODs:\t%d\n", mod->numLods);
	str += va("    ->numSurfaces:\t%d\n", pMD3Header->numSurfaces);
	str += va("    ->numTags:\t%d\n", pMD3Header->numTags);

	// Show shader usage...
	if (pContainer->OldSkinSets.size())
	{
		// Model is using old EF1/ID type skins...
		str += va("\n\nSkin Info:\n\nSkin File:\t\t%s\n", pContainer->strCurrentSkinFile.c_str());
	}
	else
	{
		// Standard shaders...
		int iUniqueShaderCount = MD3Model_GetUniqueShaderCount(hModel);
		str += va("\n\nShader Info:\t( %d unique shaders )\n\n", iUniqueShaderCount);

		for (int iUniqueShader = 0; iUniqueShader < iUniqueShaderCount; iUniqueShader++)
		{
			bool bFound = false;

			LPCSTR	psShaderName = MD3Model_GetUniqueShader(iUniqueShader);
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

	return str.c_str();
}

// Call this to re-evaluate any part of the tree that has surfaces owned by this model, and set their text ok...
//
// hTreeItem = tree item to start from, pass NULL to start from root
bool R_MD3Model_Tree_ReEvalSurfaceText(ModelHandle_t hModel, HTREEITEM hTreeItem /* = NULL */)
{
	bool bReturn = false;

	if (!hTreeItem)
	{
		hTreeItem = ModelTree_GetRootItem();
	}

	if (hTreeItem)
	{
		// Process this tree item...
		TreeItemData_t	TreeItemData;
		TreeItemData.uiData = ModelTree_GetItemData(hTreeItem);

		if (TreeItemData.iModelHandle == hModel)
		{
			// Ok, tree item belongs to this model, so what is it?...
			ModelContainer_t *pContainer = ModelContainer_FindFromModelHandle(hModel);

			if (pContainer)
			{
				if (TreeItemData.iItemType == TREEITEMTYPE_MD3_SURFACE)
				{
					// It's a surface, so re-eval its text...
					LPCSTR psSurfaceName = MD3Model_GetSurfaceName(hModel, TreeItemData.iItemNumber);

					// A little harmless optimisation here...
					ModelTree_SetItemText(hTreeItem, MD3Model_CreateSurfaceName(psSurfaceName));
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

// Read an optional set of skin files, and if present, add them into the model tree...
//
// Return is success/fail (but it's an optional file, so return bool is just FYI really)
//    (note that partial failures still count as successes, as long as at least one file succeeds)
static bool MD3Model_ReadSkinFiles(HTREEITEM hParent, ModelContainer_t *pContainer, LPCSTR psLocalFilename)
{
	// Check for optional .skin files... (CHC-type)
	if (OldSkins_Read(psLocalFilename, pContainer))
	{
		return OldSkins_ApplyToTree(hParent, pContainer);
	}

	return false;
}

bool MD3Model_GetBounds(ModelHandle_t hModel, int iLODNumber, int iFrameNumber, vec3_t &v3Mins, vec3_t &v3Maxs)
{
	md3Header_t	*pMD3Header = (md3Header_t	*)RE_GetModelData(hModel);
	ModelContainer_t *pContainer = ModelContainer_FindFromModelHandle(hModel);

	if (iLODNumber >= pContainer->iNumLODs)
		return false;	// can't reference it if we don't have a LOD of this level of course

	if (iFrameNumber >= pMD3Header->numFrames)
		return false;

	//////////////////////////////
	//
	// remove if this code is cut/paste outside of ModView...
	//	
	if (!pContainer)
		return false;
	//
	//////////////////////////////

	md3Frame_t *pFrame = (md3Frame_t *)((byte *)pMD3Header + pMD3Header->ofsFrames);

	for (int i = 0; i < pMD3Header->numFrames; i++, pFrame++)
	{
		VectorCopy(pFrame->bounds[0], v3Mins);
		VectorCopy(pFrame->bounds[1], v3Maxs);
	}

	return true;
}

// If we get this far now then we no longer need to check the model data because RE_RegisterModel has already 
//	validated it. Oh well...
//
// This MUST be called after Jake's code has finished, since I read from his tables...
bool MD3Model_Parse(struct ModelContainer *pContainer, LPCSTR psLocalFilename, HTREEITEM hTreeItem_Parent)
{
	bool bReturn = false;

	ModelHandle_t hModel = pContainer->hModel;
	model_t	*mod = R_GetModelByHandle(hModel);
	md3Surface_t *pSurf = mod->md3surf[0][pContainer->md3_slist->surface];
	md3Tag_t *pTag = mod->md3tag[0][pContainer->md3_tlist->tag];

	HTREEITEM hTreeItem_Tags = NULL;

	if (mod->md3[0]->ident == MD3_IDENT)
	{
		if (mod->md3[0]->version == MD3_VERSION)
		{
			// Phew, all systems go...
			bReturn = true;

			TreeItemData_t	TreeItemData = { 0 };
			TreeItemData.iModelHandle = hModel;

			TreeItemData.iItemType = TREEITEMTYPE_MODELNAME;
			pContainer->hTreeItem_ModelName = ModelTree_InsertItem(va("==>  %s  <==", Filename_WithoutPath(/*Filename_WithoutExt*/(psLocalFilename))), hTreeItem_Parent, TreeItemData.uiData);

			TreeItemData.iItemType = TREEITEMTYPE_MD3SURFACEHEADER;
			HTREEITEM hTreeItem_Surfaces = ModelTree_InsertItem("Surfaces", pContainer->hTreeItem_ModelName, TreeItemData.uiData);

			TreeItemData.iItemType = TREEITEMTYPE_MD3TAGHEADER;
			hTreeItem_Tags = ModelTree_InsertItem("Tags", pContainer->hTreeItem_ModelName, TreeItemData.uiData);

			for (int iSurfaceIndex = 0; iSurfaceIndex < mod->md3[0]->numSurfaces; iSurfaceIndex++)
			{
				pSurf = mod->md3surf[0][pContainer->md3_slist[iSurfaceIndex].surface];

				R_MD3_AddSurfaceToTree(hModel, hTreeItem_Surfaces, iSurfaceIndex, pSurf->name);
			}

			for (int iTagIndex = 0; iTagIndex < mod->md3[0]->numTags; iTagIndex++)
			{
				pTag = mod->md3tag[0][pContainer->md3_tlist[iTagIndex].tag];

				R_MD3_AddTagToTree(hModel, hTreeItem_Tags, iTagIndex, pTag->name);
			}
		}
		else
		{
			ErrorBox(va("Wrong model format version number: %d (expecting %d)", mod->md3[0]->version, MD3_VERSION));
		}
	}
	else
	{
		ErrorBox(va("Wrong model Ident: %X (expecting %X)", mod->md3[0]->ident, MD3_IDENT));
	}

	if (bReturn)
	{
		bReturn = R_MD3Model_Tree_ReEvalSurfaceText(hModel);

		if (bReturn)
		{
			// Now fill in the fields we need in the container to avoid MD3-specific queries...
			pContainer->pModelInfoFunction = MD3Model_Info;
			pContainer->pModelGetSurfaceNameFunction = MD3Model_GetSurfaceName;
			pContainer->pModelGetSurfaceBoltNameFunction = MD3Model_GetTagName;
			pContainer->iNumFrames = MD3Model_GetNumFrames(hModel);
			pContainer->iNumLODs = MD3Model_GetNumLODs(hModel);
			pContainer->iNumSurfaces = MD3Model_GetNumSurfaces(hModel);

			pContainer->iSurfaceBolt_MaxBoltPoints = MD3Model_GetNumTags(hModel);

			MD3Model_ReadSkinFiles(pContainer->hTreeItem_ModelName, pContainer, psLocalFilename);
		}
	}

	return bReturn;
}

//////////////// eof //////////////
