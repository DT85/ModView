// Filename:-	md3_code.cpp
//

#include "stdafx.h"
#include "includes.h"
#include "special_defines.h"
//
//#include "anims.h"
#include "R_Model.h"
#include "md3_format.h"
//#include "sequence.h"
//#include "parser.h"
#include "shader.h"
#include "skins.h"
#include "textures.h"	// for some stats stuff
//
#include "md3_code.h"


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

bool MD3Model_SurfaceIsTag(ModelHandle_t hModel, int iSurfaceIndex)
{
	md3Header_t	*pMD3Header = (md3Header_t	*)RE_GetModelData(hModel);

	assert(iSurfaceIndex < pMD3Header->numSurfaces);
	if (iSurfaceIndex < pMD3Header->numSurfaces)
	{
		md3Surface_t *pSurf = (md3Surface_t *)((byte *)pMD3Header + pMD3Header->ofsSurfaces);

		for (int i = 0; i < pMD3Header->numSurfaces; i++)
		{
			LPCSTR psSurfaceName = MD3Model_GetSurfaceName(hModel, i);

			if (!_stricmp("_off", &psSurfaceName[strlen(psSurfaceName) - 4]))
				return (pSurf->flags & MD3SURFACEFLAG_ISBOLT);
		}
	}

	return false;
}

static LPCSTR MD3Model_CreateSurfaceName(LPCSTR psSurfaceName)
{
	static CString string;

	string = psSurfaceName;	// do NOT use in constructor form since this is a static (that got me first time... :-)

	return (LPCSTR)string;
}

static bool R_MD3_AddSurfaceToTree(ModelHandle_t hModel, HTREEITEM htiParent, int iThisSurfaceIndex, bool bTagsOnly)
{
	bool bReturn = true;

	md3Header_t	*pMD3Header = (md3Header_t	*)RE_GetModelData(hModel);
	md3Surface_t *pSurf = (md3Surface_t *)((byte *)pMD3Header + pMD3Header->ofsSurfaces);
	md3Tag_t *pTag = (md3Tag_t *)((byte *)pMD3Header + pMD3Header->ofsTags);

	// insert me...
	//
	HTREEITEM htiThis = NULL;

	if (!bTagsOnly)
	{
		TreeItemData_t	TreeItemData = { 0 };
		TreeItemData.iItemType = TREEITEMTYPE_MD3_SURFACE;
		TreeItemData.iModelHandle = hModel;
		TreeItemData.iItemNumber = iThisSurfaceIndex;

		htiThis = ModelTree_InsertItem(MD3Model_CreateSurfaceName(pSurf->name),	// LPCTSTR psName, 
			htiParent,			// HTREEITEM hParent
			TreeItemData.uiData,// TREEITEMTYPE_MD3_SURFACE | iThisSurfaceIndex	// UINT32 uiUserData
			TVI_LAST
		);
	}
	else
	{
		TreeItemData_t	TreeItemData = { 0 };
		TreeItemData.iItemType = TREEITEMTYPE_MD3_TAGSURFACE;
		TreeItemData.iModelHandle = hModel;
		TreeItemData.iItemNumber = iThisSurfaceIndex;

		htiThis = ModelTree_InsertItem(MD3Model_CreateSurfaceName(pTag->name),	// LPCTSTR psName, 
			htiParent,			// HTREEITEM hParent
			TreeItemData.uiData,// TREEITEMTYPE_MD3_SURFACE | iThisSurfaceIndex	// UINT32 uiUserData
			TVI_SORT
		);
	}
	
	return bReturn;
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

// Note, this function is only really supposed to be called once, to setup the Container that owns this model
//
static int MD3Model_GetNumLODs(ModelHandle_t hModel)
{
	//mdxmHeader_t *pMDXMHeader = (mdxmHeader_t *)RE_GetModelData(hModel);
	model_t	*mod = R_GetModelByHandle(hModel);
	
	return mod->numLods;
}

// these next 2 functions are closely related, the GetCount function fills in public data which the other reads on query
//
extern set <string> stringSet;
static int MD3Model_GetUniqueShaderCount(ModelHandle_t hModel)
{
	md3Header_t	*pMD3Header = (md3Header_t	*)RE_GetModelData(hModel);
	md3Surface_t	*pMD3Surface = (md3Surface_t *)((byte *)pMD3Header + pMD3Header->ofsSurfaces);

	stringSet.clear();

	for (int iSurfaceIndex = 0; iSurfaceIndex < pMD3Header->numSurfaces; iSurfaceIndex++)
	{
		md3Shader_t	*pMD3Sshader = (md3Shader_t *)((byte *)pMD3Surface + pMD3Surface->ofsShaders);

		string strShader(pMD3Sshader->name);

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
			return (*it).c_str();
	}

	return "(Error)";	// should never get here
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

	// work out what types of surfaces we have for extra info...
	//
	int iNumTagSurfaces = 0;
	iNumTagSurfaces = pMD3Header->numTags;

	str += va("    ->numSurfaces:\t%d", pMD3Header->numSurfaces + iNumTagSurfaces);

	if (iNumTagSurfaces)
	{
		str += va("  ( = %d default", (pMD3Header->numSurfaces));

		if (iNumTagSurfaces)
		{
			str += va(" + %d TAG", iNumTagSurfaces);
		}
		str += " )";
	}
	str += "\n";

	// show shader usage...
	//
	if (pContainer->OldSkinSets.size())
	{
		// model is using old EF1/ID type skins...
		//
		str += va("\n\nSkin Info:\n\nSkin File:\t\t%s\n", pContainer->strCurrentSkinFile.c_str());
	}
	else
	{
		// standard shaders...
		//
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
					ModelTree_SetItemText(hTreeItem, MD3Model_CreateSurfaceName(psSurfaceName));
				}
			}

			// process siblings...
			//
			/*HTREEITEM hTreeItem_Sibling = ModelTree_GetNextSiblingItem(hTreeItem);
			if (hTreeItem_Sibling)
				R_MD3Model_Tree_ReEvalSurfaceText(hModel, hTreeItem_Sibling);
			*/
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
				pContainer->pModelGetSurfaceNameFunction = MD3Model_GetSurfaceName;
				pContainer->pModelGetSurfaceBoltNameFunction = MD3Model_GetSurfaceName;	// same thing in this format
				pContainer->iNumFrames = MD3Model_GetNumFrames(hModel);
				pContainer->iNumLODs = MD3Model_GetNumLODs(hModel);
				pContainer->iNumSurfaces = MD3Model_GetNumSurfaces(hModel);

				pContainer->iSurfaceBolt_MaxBoltPoints = pContainer->iNumSurfaces;	// ... since these are pretty much the same in this format

				MD3Model_ReadSkinFiles(pContainer->hTreeItem_ModelName, pContainer, psLocalFilename);
			}
		}
	}

	return bReturn;
}

//////////////// eof //////////////
