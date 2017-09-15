//--------------------------------------------------------------------------------
//
//　gameObject3D.h
//	Author : Xu Wenjie
//	Date   : 2017-04-27
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  インクルードファイル
//--------------------------------------------------------------------------------
#include "KF_UtilityFBX.h"
#include "KF_Utility.h"
#include "manager.h"
#include "textureManager.h"

//--------------------------------------------------------------------------------
//  クラス
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//
//  MyNode
//
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  RecursiveDraw
//--------------------------------------------------------------------------------
void CMyNode::Release(void)
{
	//Texture
	for (auto& tex : vecTex)
	{
		tex.strName.clear();
		tex.strUVSetName.clear();
		CMain::GetManager()->GetTextureManager()->DisuseTexture(tex.strName);
	}
	vecTex.clear();

	//Mesh
	for (auto& mesh : vecMesh)
	{
		mesh.vecMtx.clear();
		mesh.vecNormal.clear();
		mesh.vecNormalIdx.clear();
		for (auto& point : mesh.vecPoint)
		{
			point.vecBornRefarence.clear();
		}
		mesh.vecPoint.clear();
		mesh.vecPointIdx.clear();
		for (auto& UVSet : mesh.vecUVSet)
		{
			UVSet.strUVSetName.clear();
			UVSet.vecUV.clear();
			UVSet.vecUVIdx.clear();
		}
		mesh.vecUVSet.clear();
	}
	vecMesh.clear();

	//Child
	for (auto pChild : listChild)
	{
		pChild->Release();
	}
	listChild.clear();

	//Other
	strName.clear();
	vecAttributeName.clear();

	delete this;
}

//--------------------------------------------------------------------------------
//  RecursiveDraw
//--------------------------------------------------------------------------------
void CMyNode::RecursiveDraw(const bool& bDrawNormal)
{
	//glPushMatrix();
	//glScalef(vScale.m_fX, vScale.m_fY, vScale.m_fZ);
	//glRotatef(vRot.m_fZ, 0.0f, 0.0f, 1.0f);
	//glRotatef(vRot.m_fX, 1.0f, 0.0f, 0.0f);
	//glRotatef(vRot.m_fY, 0.0f, 1.0f, 0.0f);
	//glTranslatef(vTrans.m_fX, vTrans.m_fY, vTrans.m_fZ);

	////Mesh
	//for (auto& mesh : vecMesh)
	//{
	//	//Texture
	//	if (!vecTex.empty())
	//	{
	//		glBindTexture(GL_TEXTURE_2D, vecTex[mesh.nMaterialIndex].nID);
	//	}

	//	// ポリゴン描画         
	//	glBegin(GL_TRIANGLES);

	//	if (mesh.vecMtx.empty())
	//	{// 骨なし（つまり剛体の塊）             
	//		for (int nCnt = 0; nCnt < (int)mesh.vecPointIdx.size(); ++nCnt)
	//		{
	//			glColor4f(255.0f, 255.0f, 255.0f, 255.0f);
	//			glNormal3f(
	//				mesh.vecNormal[mesh.vecNormalIdx[nCnt]].m_fX,
	//				mesh.vecNormal[mesh.vecNormalIdx[nCnt]].m_fY,
	//				mesh.vecNormal[mesh.vecNormalIdx[nCnt]].m_fZ);
	//			glTexCoord2f(
	//				mesh.vecUVSet[0].vecUV[mesh.vecUVSet[0].vecUVIdx[nCnt]].m_fX,
	//				mesh.vecUVSet[0].vecUV[mesh.vecUVSet[0].vecUVIdx[nCnt]].m_fY);
	//			glVertex3f(
	//				mesh.vecPoint[mesh.vecPointIdx[nCnt]].vPos.m_fX,
	//				mesh.vecPoint[mesh.vecPointIdx[nCnt]].vPos.m_fY,
	//				mesh.vecPoint[mesh.vecPointIdx[nCnt]].vPos.m_fZ);
	//		}
	//	}
	//	else
	//	{// 骨あり（つまりワンスキンなど） 
	//	 // 頂点の座標変換 
	//		vector<CKFVec3> vecPos;
	//		vecPos.reserve(mesh.vecPoint.size());
	//		CKFMtx44 mtx;
	//		for (auto& point : mesh.vecPoint)
	//		{
	//			ZeroMemory(&mtx, sizeof(CKFMtx44));
	//			for (auto& bornRefarence : point.vecBornRefarence)
	//			{
	//				mtx += mesh.vecMtx[bornRefarence.ucIndex] * bornRefarence.fWeight;
	//			}
	//			CKFVec3 vPos = point.vPos;
	//			CKFMath::Vec3TransformCoord(&vPos, mtx);
	//			vecPos.push_back(vPos);
	//		}

	//		for (int nCnt = 0; nCnt < (int)mesh.vecPointIdx.size(); ++nCnt)
	//		{
	//			glColor4f(255.0f, 255.0f, 255.0f, 255.0f);
	//			glNormal3f(
	//				mesh.vecNormal[mesh.vecNormalIdx[nCnt]].m_fX,
	//				mesh.vecNormal[mesh.vecNormalIdx[nCnt]].m_fY,
	//				mesh.vecNormal[mesh.vecNormalIdx[nCnt]].m_fZ);
	//			glTexCoord2f(
	//				mesh.vecUVSet[0].vecUV[mesh.vecUVSet[0].vecUVIdx[nCnt]].m_fX,
	//				mesh.vecUVSet[0].vecUV[mesh.vecUVSet[0].vecUVIdx[nCnt]].m_fY);
	//			glVertex3f(
	//				vecPos[mesh.vecPointIdx[nCnt]].m_fX,
	//				vecPos[mesh.vecPointIdx[nCnt]].m_fY,
	//				vecPos[mesh.vecPointIdx[nCnt]].m_fZ);
	//		}
	//	}

	//	glEnd();
	//}

	////Child
	//for (auto pNode : listChild)
	//{
	//	pNode->RecursiveDraw(bDrawNormal);
	//}
	//glPopMatrix();
}

//--------------------------------------------------------------------------------
//  analyzePos
//--------------------------------------------------------------------------------
void CMyNode::analyzePos(FbxMesh* pMesh)
{
	//頂点数の取得
	auto nNumPoint = pMesh->GetControlPointsCount();

	//頂点バッファの作成
	auto& meshNow = vecMesh.back();
	meshNow.vecPoint.resize(nNumPoint);

	//頂点情報取得
	auto pFbxV4 = pMesh->GetControlPoints();
	for (int nCnt = 0; nCnt < nNumPoint; ++nCnt)
	{
		meshNow.vecPoint[nCnt].vPos.m_fX = static_cast<float>(pFbxV4[nCnt][0]);
		meshNow.vecPoint[nCnt].vPos.m_fY = static_cast<float>(pFbxV4[nCnt][1]);
		meshNow.vecPoint[nCnt].vPos.m_fZ = static_cast<float>(pFbxV4[nCnt][2]);
	}

	//インデックスの取得
	auto nNumIdx = pMesh->GetPolygonVertexCount();
	meshNow.vecPointIdx.resize(nNumIdx);
	auto pIdx = pMesh->GetPolygonVertices();
	for (int nCnt = 0; nCnt < nNumIdx; ++nCnt)
	{
		meshNow.vecPointIdx[nCnt] = static_cast<unsigned int>(pIdx[nCnt]);
	}

}

//--------------------------------------------------------------------------------
//  analyzeNormal
//--------------------------------------------------------------------------------
void CMyNode::analyzeNormal(FbxMesh* pMesh)
{
	int nLayerCnt = pMesh->GetLayerCount();

	for (int nCnt = 0; nCnt < nLayerCnt; ++nCnt)
	{
		auto pElementNormal = pMesh->GetElementNormal(nCnt);

		if (!pElementNormal) { continue; }

		//法線データの取得
		auto& meshNow = vecMesh.back();
		auto nNumNormal = pElementNormal->GetDirectArray().GetCount();
		meshNow.vecNormal.resize(nNumNormal);
		for (int nCnt = 0; nCnt < nNumNormal; ++nCnt)
		{
			meshNow.vecNormal[nCnt].m_fX = (float)pElementNormal->GetDirectArray()[nCnt][0];
			meshNow.vecNormal[nCnt].m_fY = (float)pElementNormal->GetDirectArray()[nCnt][1];
			meshNow.vecNormal[nCnt].m_fZ = (float)pElementNormal->GetDirectArray()[nCnt][2];
		}

		//マッピングモード、リファレンスモード取得
		auto mappingMode = pElementNormal->GetMappingMode();
		auto referenceMode = pElementNormal->GetReferenceMode();
		if (mappingMode == FbxLayerElement::eByPolygonVertex)
		{//法線独自のインデックスを使用
			if (referenceMode == FbxLayerElement::eDirect)
			{// インデックス参照の必要なし
				meshNow.vecNormalIdx.resize(nNumNormal);
				for (int nCnt = 0; nCnt < nNumNormal; ++nCnt)
				{
					meshNow.vecNormalIdx[nCnt] = nCnt;
				}
			}
			else if (referenceMode == FbxLayerElement::eIndexToDirect
				|| referenceMode == FbxLayerElement::eIndex)
			{// 独自インデックスを所持 
			 // インデックス数の取得                 
				int nNumNormalIdx = pElementNormal->GetIndexArray().GetCount();

				// 法線インデックス格納用コンテナの領域予約
				meshNow.vecNormalIdx.resize(nNumNormalIdx);

				// 法線インデックスの取得  
				for (int nCnt = 0; nCnt < nNumNormalIdx; ++nCnt)
				{
					meshNow.vecNormalIdx[nCnt] = pElementNormal->GetIndexArray()[nCnt];
				}
			}
		}

		else if (mappingMode == FbxLayerElement::eByControlPoint)
		{//頂点バッファと同じインデックス
			if (referenceMode == FbxLayerElement::eDirect)
			{
				meshNow.vecNormalIdx.resize(meshNow.vecPointIdx.size());
				meshNow.vecNormalIdx.assign(meshNow.vecPointIdx.begin(), meshNow.vecPointIdx.end());
			}
		}
		else
		{
			MessageBox(NULL, "Normal Mode ERROR!!", "未実装", MB_OK);
		}

		break;
	}
}

//--------------------------------------------------------------------------------
//  analyzeUV
//--------------------------------------------------------------------------------
void CMyNode::analyzeUV(FbxMesh* pMesh)
{
	int nUVLayerCnt = pMesh->GetElementUVCount();
	if (!nUVLayerCnt)
	{
		MessageBox(NULL, "レイヤーを持っていないメッシュを確認", "analyzeUV", MB_OK);
		return;
	}
	auto& meshNow = vecMesh.back();
	meshNow.vecUVSet.resize(nUVLayerCnt);

	for (int nCnt = 0; nCnt < nUVLayerCnt; ++nCnt)
	{
		auto& UVSet = meshNow.vecUVSet[nCnt];

		auto pElementUV = pMesh->GetLayer(nCnt)->GetUVs();

		if (!pElementUV) { continue; }

		//UVSetName
		UVSet.strUVSetName = pElementUV->GetName();

		//マッピングモード、リファレンスモード取得
		auto mappingMode = pElementUV->GetMappingMode();
		auto referenceMode = pElementUV->GetReferenceMode();

		if (mappingMode == FbxLayerElement::eByPolygonVertex)
		{
			if (referenceMode == FbxLayerElement::eDirect)
			{
				int nNumUV = pElementUV->GetDirectArray().GetCount();
				UVSet.vecUV.resize(nNumUV);
				UVSet.vecUVIdx.resize(nNumUV);

				for (int nCnt = 0; nCnt < nNumUV; ++nCnt)
				{
					UVSet.vecUV[nCnt].m_fX = static_cast<float>(pElementUV->GetDirectArray()[nCnt][0]);
					UVSet.vecUV[nCnt].m_fY = static_cast<float>(pElementUV->GetDirectArray()[nCnt][1]);
					UVSet.vecUVIdx[nCnt] = nCnt;
				}
			}

			else if (referenceMode == FbxLayerElement::eIndexToDirect
				|| referenceMode == FbxLayerElement::eIndex)
			{
				//Idx
				int nNumUVIdx = pElementUV->GetIndexArray().GetCount();
				UVSet.vecUVIdx.resize(nNumUVIdx);
				for (int nCnt = 0; nCnt < nNumUVIdx; ++nCnt)
				{
					UVSet.vecUVIdx[nCnt] = pElementUV->GetIndexArray()[nCnt];
				}

				//UV
				int nNumUV = pElementUV->GetDirectArray().GetCount();
				UVSet.vecUV.resize(nNumUV);
				for (int nCnt = 0; nCnt < nNumUV; ++nCnt)
				{
					UVSet.vecUV[nCnt].m_fX = static_cast<float>(pElementUV->GetDirectArray()[nCnt][0]);
					UVSet.vecUV[nCnt].m_fY = static_cast<float>(pElementUV->GetDirectArray()[nCnt][1]);
				}
			}
		}
		else
		{
			MessageBox(NULL, "マッピングモード非対応", "analyzeUV", MB_OK);
		}

		//else if (mappingMode == FbxLayerElement::eByControlPoint)
		//{
		//	if (referenceMode == FbxLayerElement::eDirect)
		//	{
		//		part.nNumVT = pElementUV->GetDirectArray().GetCount();
		//		//int nCntIdx = pElementUV->GetIndexArray().GetCount();
		//		part.vecUV.resize(part.nNumVT);
		//		for (int nCnt = 0; nCnt < part.nNumVT; nCnt++)
		//		{
		//			int nIdx = pElementUV->GetIndexArray().GetAt(nCnt);
		//			part.vecUV[nCnt].m_fX = (float)pElementUV->GetDirectArray()[nIdx][0];
		//			part.vecUV[nCnt].m_fY = (float)pElementUV->GetDirectArray()[nIdx][1];
		//			part.vecIdx[nCnt].nVt = nCnt;
		//		}
		//	}
		//}
	}
}

//--------------------------------------------------------------------------------
//  analyzeTexture
//--------------------------------------------------------------------------------
void CMyNode::analyzeTexture(FbxNode* pNode)
{
	//マテリアル数の取得
	int nMaterialCnt = pNode->GetMaterialCount();
	vecTex.resize(nMaterialCnt);

	for (int nCnt = 0; nCnt < nMaterialCnt; ++nCnt)
	{
		auto pMaterial = pNode->GetMaterial(nCnt);
		if (!pMaterial) { continue; }

		// ディフューズ情報の取得     
		auto diffuseProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
		int nLayeredTextureCnt = diffuseProperty.GetSrcObjectCount<FbxLayeredTexture>();

		if (nLayeredTextureCnt == 0)
		{
			int nTextureCount = diffuseProperty.GetSrcObjectCount<FbxFileTexture>();

			//各テクスチャについてテクスチャ情報取得
			for (int nCntTex = 0; nCntTex < nTextureCount; ++nCntTex)
			{//nCnt番目のテクスチャオブジェクトの取得
				auto pTexture = diffuseProperty.GetSrcObject<FbxFileTexture>(nCntTex);
				if (!pTexture) { continue; }

				//テクスチャファイル名
				string strPath = pTexture->GetFileName();
				string strType;
				CKFUtility::AnalyzeTexPath(strPath, vecTex[nCnt].strName, strType);
				//vecTex[nCnt].nID = CKFTexture2D::LoadTGA("data/textures/" + strFileName + '/' + vecTex[nCnt].strName + ".tga");

				//UVSet名
				vecTex[nCnt].strUVSetName = pTexture->UVSet.Get().Buffer();

				//とりあえず0番だけとる
				break;
			}
		}
		else
		{
			//各テクスチャについてテクスチャ情報取得
			int nCntTex = 0;
			for (int nCntLayer = 0; nCntLayer < nLayeredTextureCnt; ++nCntLayer)
			{//nCnt番目のテクスチャオブジェクトの取得
				auto pLayeredTexture = diffuseProperty.GetSrcObject<FbxLayeredTexture>(nCntLayer);
				int nTextureCount = pLayeredTexture->GetSrcObjectCount<FbxFileTexture>();

				//各テクスチャについてテクスチャ情報取得
				for (int nCnt = 0; nCnt < nTextureCount; ++nCnt)
				{//nCnt番目のテクスチャオブジェクトの取得
					auto pTexture = pLayeredTexture->GetSrcObject<FbxFileTexture>(nCnt);
					if (!pTexture) { continue; }

					//テクスチャファイル名
					string strPath = pTexture->GetFileName();
					string strType;
					CKFUtility::AnalyzeTexPath(strPath, vecTex[nCntTex].strName, strType);
					vecTex[nCntTex].strName += '.' + strType;
					CMain::GetManager()->GetTextureManager()->UseTexture(vecTex[nCntTex].strName);

					//UVSet名
					vecTex[nCntTex].strUVSetName = pTexture->UVSet.Get().Buffer();

					++nCntTex;
				}
			}
		}
	}
}

//--------------------------------------------------------------------------------
//  analyzeMaterial
//--------------------------------------------------------------------------------
void CMyNode::analyzeMaterial(FbxMesh* pMesh)
{
	auto& meshNow = vecMesh.back();
	int nNumLayer = pMesh->GetLayerCount();

	for (int nCnt = 0; nCnt < nNumLayer; ++nCnt)
	{
		auto pElementMaterial = pMesh->GetLayer(nCnt)->GetMaterials();
		if (!pElementMaterial) { continue; }

		int nNumMaterialIdx = pElementMaterial->GetIndexArray().GetCount();
		if (nNumMaterialIdx == 0) { continue; }

		auto mappingMode = pElementMaterial->GetMappingMode();
		auto referenceMode = pElementMaterial->GetReferenceMode();

		if (mappingMode == FbxLayerElement::eAllSame)
		{
			if (referenceMode == FbxLayerElement::eIndexToDirect)
			{// メッシュ全部がこのマテリアルインデックス 
				meshNow.nMaterialIndex = pElementMaterial->GetIndexArray()[0];
			}
			else
			{
				MessageBox(NULL, "...未対応のリファレンスモードを取得した", "Material MappingMode = eAllSame", MB_OK);
			}
		}
		else if (mappingMode == FbxLayerElement::eByControlPoint)
		{
			MessageBox(NULL, "...未対応のマッピングモード[eByControlPoint]を取得した", "Material MappingMode", MB_OK);
		}
		else if (mappingMode == FbxLayerElement::eByPolygon)
		{// マテリアル分割されているはずだから、一番はじめのだけでいい         
			meshNow.nMaterialIndex = pElementMaterial->GetIndexArray()[0];
		}
		else if (mappingMode == FbxLayerElement::eByEdge)
		{
			MessageBox(NULL, "...未対応のマッピングモード[eByEdge]を取得した", "Material MappingMode", MB_OK);
		}
		else
		{
			MessageBox(NULL, "...未対応のマッピングモードを取得した", "Material MappingMode", MB_OK);
		}
	}
}

//--------------------------------------------------------------------------------
//  analyzeCluster
//--------------------------------------------------------------------------------
void CMyNode::analyzeCluster(FbxMesh* pMesh)
{
	auto& meshNow = vecMesh.back();
	CKFMtx44 mtxIdentity;

	// スキンの数を取得 
	int nNumSkin = pMesh->GetDeformerCount(FbxDeformer::eSkin);

	for (int nCntSkin = 0; nCntSkin < nNumSkin; ++nCntSkin)
	{
		// スキンを取得 
		auto pSkin = (FbxSkin*)pMesh->GetDeformer(nCntSkin, FbxDeformer::eSkin);

		// クラスターの数を取得 
		int nNumCluster = pSkin->GetClusterCount();

		for (int nCntCluster = 0; nCntCluster < nNumCluster; ++nCntCluster)
		{
			// クラスタを取得
			auto pCluster = pSkin->GetCluster(nCntCluster);

			// このクラスタが影響を及ぼす頂点インデックスの個数を取得 
			int nNumPointIdx = pCluster->GetControlPointIndicesCount();

			if (!nNumPointIdx)
			{// このメッシュにおいて、このクラスタは無視していいと思う...                 
				meshNow.vecMtx.push_back(mtxIdentity);
				continue;
			}

			// 初期姿勢行列の取得 
			FbxAMatrix lReferenceGlobalInitPosition;
			FbxAMatrix lReferenceGlobalCurrentPosition;
			FbxAMatrix lClusterGlobalInitPosition;

			pCluster->GetTransformMatrix(lReferenceGlobalInitPosition);
			// lReferenceGlobalCurrentPosition = pGlobalPosition; // <- たぶんワールド座標変換行列ではないかと  

			// Multiply lReferenceGlobalInitPosition by Geometric Transformation
			auto lReferenceGeometry = getGeometry(pMesh->GetNode());
			lReferenceGlobalInitPosition *= lReferenceGeometry;

			// Get the link initial global position and the link current global position.
			pCluster->GetTransformLinkMatrix(lClusterGlobalInitPosition);
			auto lClusterGlobalCurrentPosition = pCluster->GetLink()->EvaluateGlobalTransform(1);

			// Compute the initial position of the link relative to the reference. 
			auto lClusterRelativeInitPosition = lClusterGlobalInitPosition.Inverse() * lReferenceGlobalInitPosition;

			// Compute the current position of the link relative to the reference. 
			auto lClusterRelativeCurrentPositionInverse = lReferenceGlobalCurrentPosition.Inverse() * lClusterGlobalCurrentPosition;

			// Compute the shift of the link relative to the reference. 
			auto VertexTransformMatrix = lClusterRelativeCurrentPositionInverse * lClusterRelativeInitPosition;

			// ↑ 初期姿勢行列も考慮されたモーションボーン行列なので、これで頂点座標を変換するだけで良い 
			CKFMtx44 mtx;
			for (int nY = 0; nY < 4; ++nY)
			{
				for (int nX = 0; nX < 4; ++nX)
				{
					mtx.m_af[nY][nX] = static_cast<float>(VertexTransformMatrix.Get(nY, nX));
				}
			}

			meshNow.vecMtx.push_back(mtx);

			auto pPointIndexArray = pCluster->GetControlPointIndices();
			auto pWeightArray = pCluster->GetControlPointWeights();
			for (int nCnt = 0; nCnt < nNumPointIdx; ++nCnt)
			{
				meshNow.vecPoint[pPointIndexArray[nCnt]].vecBornRefarence.push_back(
					BornRefarence(nCntCluster, static_cast<float>(pWeightArray[nCnt])));
			}
		}
	}
}

//--------------------------------------------------------------------------------
//  getGeometry
//	Get the geometry offset to a node. It is never inherited by the children
//--------------------------------------------------------------------------------
FbxAMatrix CMyNode::getGeometry(FbxNode* pNode)
{
	const FbxVector4 lT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 lR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 lS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);
	return FbxAMatrix(lT, lR, lS);
}

//--------------------------------------------------------------------------------
//
//  CKFUtilityFBX
//
//--------------------------------------------------------------------------------
CMyNode* CKFUtilityFBX::Load(const string& strFilePath)
{
	//FBX読込実験コード
	auto lSdkManager = FbxManager::Create();

	// Create the IO settings object.
	auto ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
	lSdkManager->SetIOSettings(ios);

	// Create an importer using the SDK manager.
	auto lImporter = FbxImporter::Create(lSdkManager, "");

	// Use the first argument as the filename for the importer.
	if (!lImporter->Initialize(strFilePath.c_str(), -1, lSdkManager->GetIOSettings()))
	{
		char buf[256];
		wsprintf(buf, "Call to FbxImporter::Initialize() failed.\nError returned: %s\n\n", lImporter->GetStatus().GetErrorString());
		MessageBox(NULL, buf, "error", MB_OK);
		lImporter->Destroy();
		lSdkManager->Destroy();
		return nullptr;
	}

	// Create a new scene so that it can be populated by the imported file.
	auto lScene = FbxScene::Create(lSdkManager, "myScene");

	// Import the contents of the file into the scene.
	lImporter->Import(lScene);

	// 三角分割
	FbxGeometryConverter lConverter(lSdkManager);
	lConverter.Triangulate(lScene, true);

	// 縮退ポリゴンの削除
	lConverter.RemoveBadPolygonsFromMeshes(lScene);

	// マテリアルごとにメッシュ分離
	lConverter.SplitMeshesPerMaterial(lScene, true);

	auto pRootNode = recursiveNode(lSdkManager, lScene->GetRootNode());

	lImporter->Destroy();
	lScene->Destroy();
	lSdkManager->Destroy();

	return pRootNode;
}

//--------------------------------------------------------------------------------
//  RecursiveNode
//--------------------------------------------------------------------------------
CMyNode* CKFUtilityFBX::recursiveNode(FbxManager* pManager, FbxNode* pNode)
{
	if (!pNode) { return NULL; }
	auto pMyNode = new CMyNode;
	pMyNode->strName = pNode->GetName();
	pMyNode->vTrans.m_fX = static_cast<float>(pNode->LclTranslation.Get()[0]);
	pMyNode->vTrans.m_fY = static_cast<float>(pNode->LclTranslation.Get()[1]);
	pMyNode->vTrans.m_fZ = static_cast<float>(pNode->LclTranslation.Get()[2]);
	pMyNode->vRot.m_fX = static_cast<float>(pNode->LclRotation.Get()[0]);
	pMyNode->vRot.m_fY = static_cast<float>(pNode->LclRotation.Get()[1]);
	pMyNode->vRot.m_fZ = static_cast<float>(pNode->LclRotation.Get()[2]);
	pMyNode->vScale.m_fX = static_cast<float>(pNode->LclScaling.Get()[0]);
	pMyNode->vScale.m_fY = static_cast<float>(pNode->LclScaling.Get()[1]);
	pMyNode->vScale.m_fZ = static_cast<float>(pNode->LclScaling.Get()[2]);

	// マテリアル情報の解析（マテリアルリスト化）                
	pMyNode->analyzeTexture(pNode);

	for (int nCnt = 0; nCnt < pNode->GetNodeAttributeCount(); nCnt++)
	{
		auto type = pNode->GetNodeAttributeByIndex(nCnt)->GetAttributeType();
		pMyNode->vecAttributeName.push_back(getAttributeTypeName(type));

		if (type == FbxNodeAttribute::eMesh)
		{//Mesh情報
			pMyNode->vecMesh.push_back(Mesh());

			// メッシュ情報の取得                 
			FbxMesh* pMesh = FbxCast<FbxMesh>(pNode->GetNodeAttributeByIndex(nCnt));

			// 頂点座標解析                 
			pMyNode->analyzePos(pMesh);

			// 法線解析                 
			pMyNode->analyzeNormal(pMesh);

			// UV解析                
			pMyNode->analyzeUV(pMesh);

			// マテリアル解析（参照情報の取得）                 
			pMyNode->analyzeMaterial(pMesh);

			// ボーン解析 
			pMyNode->analyzeCluster(pMesh);
		}
		else if (type == FbxNodeAttribute::eSkeleton)
		{//Bone
		 // メッシュではないアトリビュート   
		 //MessageBox(NULL, getAttributeTypeName(type).c_str(), "アトリビュート", MB_OK); 
		}
		else
		{
			// メッシュではないアトリビュート   
			MessageBox(NULL, getAttributeTypeName(type).c_str(), "アトリビュート", MB_OK);
		}
	}

	for (int nCnt = 0; nCnt < pNode->GetChildCount(); ++nCnt)
	{
		pMyNode->listChild.push_back(recursiveNode(pManager, pNode->GetChild(nCnt)));
	}

	return pMyNode;
}

//--------------------------------------------------------------------------------
//  GetAttributeTypeName
//--------------------------------------------------------------------------------
string CKFUtilityFBX::getAttributeTypeName(FbxNodeAttribute::EType type)
{
	switch (type)
	{
	case FbxNodeAttribute::eUnknown: return "unidentified";
	case FbxNodeAttribute::eNull: return "null";
	case FbxNodeAttribute::eMarker: return "marker";
	case FbxNodeAttribute::eSkeleton: return "skeleton";
	case FbxNodeAttribute::eMesh: return "mesh";
	case FbxNodeAttribute::eNurbs: return "nurbs";
	case FbxNodeAttribute::ePatch: return "patch";
	case FbxNodeAttribute::eCamera: return "camera";
	case FbxNodeAttribute::eCameraStereo: return "stereo";
	case FbxNodeAttribute::eCameraSwitcher: return "camera switcher";
	case FbxNodeAttribute::eLight: return "light";
	case FbxNodeAttribute::eOpticalReference: return "optical reference";
	case FbxNodeAttribute::eOpticalMarker: return "marker";
	case FbxNodeAttribute::eNurbsCurve: return "nurbs curve";
	case FbxNodeAttribute::eTrimNurbsSurface: return "trim nurbs surface";
	case FbxNodeAttribute::eBoundary: return "boundary";
	case FbxNodeAttribute::eNurbsSurface: return "nurbs surface";
	case FbxNodeAttribute::eShape: return "shape";
	case FbxNodeAttribute::eLODGroup: return "lodgroup";
	case FbxNodeAttribute::eSubDiv: return "subdiv";
	default: return "unknown";
	}
}