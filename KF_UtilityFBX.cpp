//--------------------------------------------------------------------------------
//
//�@gameObject3D.h
//	Author : Xu Wenjie
//	Date   : 2017-04-27
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  �C���N���[�h�t�@�C��
//--------------------------------------------------------------------------------
#include "KF_UtilityFBX.h"
#include "KF_Utility.h"
#include "manager.h"
#include "textureManager.h"
#include "rendererDX.h"

//--------------------------------------------------------------------------------
//  �\����
//--------------------------------------------------------------------------------
#ifdef USING_DIRECTX
//--------------------------------------------------------------------------------
//  VtxDX
//--------------------------------------------------------------------------------
bool VtxDX::operator==(const VtxDX& vValue) const
{
	if (vtx.vPos == vValue.vtx.vPos
		&& vtx.vNormal == vValue.vtx.vNormal
		&& vtx.vUV == vValue.vtx.vUV
		&& vtx.ulColor == vValue.vtx.ulColor
		&& vecBornRefarence.size() == vValue.vecBornRefarence.size())
	{
		for (int nCnt = 0; nCnt < (int)vecBornRefarence.size(); ++nCnt)
		{
			if (vecBornRefarence[nCnt].fWeight != vValue.vecBornRefarence[nCnt].fWeight
				|| vecBornRefarence[nCnt].ucIndex != vValue.vecBornRefarence[nCnt].ucIndex)
			{
				return false;
			}
		}
		return true;
	}
	return false;
}
#endif

//--------------------------------------------------------------------------------
//  �N���X
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

#ifdef USING_DIRECTX
		SAFE_RELEASE(mesh.m_pVtxBuffer);
		SAFE_RELEASE(mesh.m_pIdxBuffer);
#endif
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
//  RecursiveUpdate
//--------------------------------------------------------------------------------
void CMyNode::RecursiveUpdate(void)
{
	for (auto& mesh : vecMesh)
	{
		if (mesh.vecMtx.empty()) { continue; }

#ifdef USING_DIRECTX
		// ������i�܂胏���X�L���Ȃǁj
		// ���_�̍��W�ϊ� 
		CKFMtx44 mtx;
		VERTEX_3D* pVtx;
		mesh.m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);
		
//#pragma omp parallel for 
		for (int nCnt = 0; nCnt < (int)mesh.m_vecVtx.size(); ++nCnt)
		{
			auto& vtxDX = mesh.m_vecVtx[nCnt];
			ZeroMemory(&mtx, sizeof(CKFMtx44));
			for (auto& bornRefarence : vtxDX.vecBornRefarence)
			{
				mtx += mesh.vecMtx[bornRefarence.ucIndex] * bornRefarence.fWeight;
			}
			pVtx[nCnt].vPos = CKFMath::Vec3TransformCoord(vtxDX.vtx.vPos, mtx);
		}

		mesh.m_pVtxBuffer->Unlock();
#endif
	}

	//Child
	for (auto pNode : listChild)
	{
		pNode->RecursiveUpdate();
	}
}

//--------------------------------------------------------------------------------
//  RecursiveDraw
//--------------------------------------------------------------------------------
void CMyNode::RecursiveDraw(const bool& bDrawNormal, const CKFMtx44& mtxParent)
{
#ifdef USING_DIRECTX
	auto pDevice = CMain::GetManager()->GetRenderer()->GetDevice();

	CKFMtx44 mtxThis;

	//�g�k
	mtxThis.m_af[0][0] = vScale.m_fX;
	mtxThis.m_af[1][1] = vScale.m_fY;
	mtxThis.m_af[2][2] = vScale.m_fZ;

	//��]
	CKFMtx44 mtxRot;
	CKFMath::MtxRotationYawPitchRoll(mtxRot, vRot);

	//���s�ړ�
	CKFMtx44 mtxPos;
	CKFMath::MtxTranslation(mtxPos, vTrans);
	mtxThis *= mtxPos;

	//�e�̍s��Ƃ�����
	mtxThis *= mtxParent;

	//Mesh
	for (auto& mesh : vecMesh)
	{
		//Texture
		if (!vecTex.empty())
		{
			auto pTexture = CMain::GetManager()->GetTextureManager()->GetTexture(vecTex[mesh.nMaterialIndex].strName);
			pDevice->SetTexture(0, pTexture);
		}

		D3DXMATRIX mtx = mtxThis;
		pDevice->SetTransform(D3DTS_WORLD, &mtx);

		// ���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
		pDevice->SetStreamSource(
			0,						//�X�g���[���ԍ�
			mesh.m_pVtxBuffer,		//���_�o�b�t�@
			0,						//�I�t�Z�b�g�i�J�n�ʒu�j
			sizeof(VERTEX_3D));		//�X�g���C�h��

		// ���_�C���f�b�N�X�̐ݒ�
		pDevice->SetIndices(mesh.m_pIdxBuffer);

		// ���_�t�H�[�}�b�g�̐ݒ�
		pDevice->SetFVF(FVF_VERTEX_3D);

		// �}�e���A���̐ݒ�
		//D3DMATERIAL9 mat = CMain::GetManager()->GetMaterialManager()->GetMaterial(m_usMatID);
		//pDevice->SetMaterial(&mat);

		//�v���~�e�B�u�`��
		pDevice->DrawIndexedPrimitive(
			D3DPT_TRIANGLELIST,
			0,
			0,
			mesh.m_nNumVtx,
			0,
			mesh.m_nNumPolygon);
	}

	//Child
	for (auto pNode : listChild)
	{
		pNode->RecursiveDraw(bDrawNormal, mtxThis);
	}
#else
	glPushMatrix();
	glScalef(vScale.m_fX, vScale.m_fY, vScale.m_fZ);
	glRotatef(vRot.m_fZ, 0.0f, 0.0f, 1.0f);
	glRotatef(vRot.m_fX, 1.0f, 0.0f, 0.0f);
	glRotatef(vRot.m_fY, 0.0f, 1.0f, 0.0f);
	glTranslatef(vTrans.m_fX, vTrans.m_fY, vTrans.m_fZ);

	//Mesh
	for (auto& mesh : vecMesh)
	{
		//Texture
		if (!vecTex.empty())
		{
			glBindTexture(GL_TEXTURE_2D, vecTex[mesh.nMaterialIndex].nID);
		}

		// �|���S���`��         
		glBegin(GL_TRIANGLES);

		if (mesh.vecMtx.empty())
		{// ���Ȃ��i�܂荄�̂̉�j             
			for (int nCnt = 0; nCnt < (int)mesh.vecPointIdx.size(); ++nCnt)
			{
				glColor4f(255.0f, 255.0f, 255.0f, 255.0f);
				glNormal3f(
					mesh.vecNormal[mesh.vecNormalIdx[nCnt]].m_fX,
					mesh.vecNormal[mesh.vecNormalIdx[nCnt]].m_fY,
					mesh.vecNormal[mesh.vecNormalIdx[nCnt]].m_fZ);
				glTexCoord2f(
					mesh.vecUVSet[0].vecUV[mesh.vecUVSet[0].vecUVIdx[nCnt]].m_fX,
					mesh.vecUVSet[0].vecUV[mesh.vecUVSet[0].vecUVIdx[nCnt]].m_fY);
				glVertex3f(
					mesh.vecPoint[mesh.vecPointIdx[nCnt]].vPos.m_fX,
					mesh.vecPoint[mesh.vecPointIdx[nCnt]].vPos.m_fY,
					mesh.vecPoint[mesh.vecPointIdx[nCnt]].vPos.m_fZ);
			}
		}
		else
		{// ������i�܂胏���X�L���Ȃǁj 
		 // ���_�̍��W�ϊ� 
			vector<CKFVec3> vecPos;
			vecPos.reserve(mesh.vecPoint.size());
			CKFMtx44 mtx;
			for (auto& point : mesh.vecPoint)
			{
				ZeroMemory(&mtx, sizeof(CKFMtx44));
				for (auto& bornRefarence : point.vecBornRefarence)
				{
					mtx += mesh.vecMtx[bornRefarence.ucIndex] * bornRefarence.fWeight;
				}
				CKFVec3 vPos = point.vPos;
				CKFMath::Vec3TransformCoord(&vPos, mtx);
				vecPos.push_back(vPos);
			}

			for (int nCnt = 0; nCnt < (int)mesh.vecPointIdx.size(); ++nCnt)
			{
				glColor4f(255.0f, 255.0f, 255.0f, 255.0f);
				glNormal3f(
					mesh.vecNormal[mesh.vecNormalIdx[nCnt]].m_fX,
					mesh.vecNormal[mesh.vecNormalIdx[nCnt]].m_fY,
					mesh.vecNormal[mesh.vecNormalIdx[nCnt]].m_fZ);
				glTexCoord2f(
					mesh.vecUVSet[0].vecUV[mesh.vecUVSet[0].vecUVIdx[nCnt]].m_fX,
					mesh.vecUVSet[0].vecUV[mesh.vecUVSet[0].vecUVIdx[nCnt]].m_fY);
				glVertex3f(
					vecPos[mesh.vecPointIdx[nCnt]].m_fX,
					vecPos[mesh.vecPointIdx[nCnt]].m_fY,
					vecPos[mesh.vecPointIdx[nCnt]].m_fZ);
			}
		}

		glEnd();
	}

	//Child
	for (auto pNode : listChild)
	{
		pNode->RecursiveDraw(bDrawNormal);
	}
	glPopMatrix();
#endif
}

//--------------------------------------------------------------------------------
//  RecursiveRecalculateVtx
//--------------------------------------------------------------------------------
void CMyNode::RecursiveRecalculateVtx(void)
{
#ifdef USING_DIRECTX
	for (auto& mesh : vecMesh)
	{
		list<VtxDX> listVtx;
		list<int> listIdx;

		for (int nCnt = 0; nCnt < (int)mesh.vecPointIdx.size(); ++nCnt)
		{
			VtxDX vtxDX;
			auto& point = mesh.vecPoint[mesh.vecPointIdx[nCnt]];
			vtxDX.vtx.ulColor = CKFMath::sc_cWhite;
			vtxDX.vtx.vNormal = mesh.vecNormal[mesh.vecNormalIdx[nCnt]];
			vtxDX.vtx.vUV = mesh.vecUVSet[0].vecUV[mesh.vecUVSet[0].vecUVIdx[nCnt]];
			vtxDX.vtx.vPos = point.vPos;
			vtxDX.vecBornRefarence.assign(point.vecBornRefarence.begin(), point.vecBornRefarence.end());

			int nIdx = CKFUtilityFBX::FindRepetition(listVtx, vtxDX);
			if (nIdx >= 0) { listIdx.push_back(nIdx); }
			else
			{
				listIdx.push_back((int)listVtx.size());
				listVtx.push_back(vtxDX);
			}
		}

		mesh.m_nNumVtx = (int)listVtx.size();
		mesh.m_nNumIdx = (int)listIdx.size();
		mesh.m_nNumPolygon = mesh.m_nNumIdx / 3;

		auto pDevice = CMain::GetManager()->GetRenderer()->GetDevice();
		HRESULT hr;

		//���_�o�b�t�@
		hr = pDevice->CreateVertexBuffer(
			sizeof(VERTEX_3D) * mesh.m_nNumVtx,	//�쐬���������_�o�b�t�@�̃T�C�Y
			D3DUSAGE_WRITEONLY,					//���_�o�b�t�@�̎g�p���@
			FVF_VERTEX_3D,						//�����Ȃ��Ă����v
			D3DPOOL_MANAGED,					//�������Ǘ����@(managed�F�f�o�C�X�ɂ��C��)
			&mesh.m_pVtxBuffer,					//���_�o�b�t�@�̃|�C���^
			NULL);

		if (FAILED(hr))
		{
			MessageBox(NULL, "CMeshManager : CreateVertexBuffer ERROR!!", "�G���[", MB_OK | MB_ICONWARNING);
			continue;
		}

		VERTEX_3D *pVtx;
		mesh.m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);
		mesh.m_vecVtx.reserve(mesh.m_nNumVtx);
		auto itrVtx = listVtx.begin();
		for (int nCnt = 0; nCnt < mesh.m_nNumVtx; ++nCnt)
		{
			mesh.m_vecVtx.push_back(*itrVtx);
			pVtx[nCnt] = itrVtx->vtx;
			++itrVtx;
		}
		mesh.m_pVtxBuffer->Unlock();

		//�C���f�b�N�X�o�b�t�@�̍쐬
		hr = pDevice->CreateIndexBuffer(
			sizeof(WORD) * mesh.m_nNumIdx,
			D3DUSAGE_WRITEONLY,
			D3DFMT_INDEX16,
			D3DPOOL_MANAGED,
			&mesh.m_pIdxBuffer,
			NULL);

		if (FAILED(hr))
		{
			MessageBox(NULL, "CMeshManager : CreateIndexBuffer ERROR!!", "�G���[", MB_OK | MB_ICONWARNING);
			continue;
		}

		WORD *pIdx;
		mesh.m_pIdxBuffer->Lock(0, 0, (void**)&pIdx, 0);
		auto itrIdx = listIdx.begin();
		for (int nCnt = 0; nCnt < mesh.m_nNumIdx; ++nCnt)
		{
			pIdx[nCnt] = *itrIdx;
			++itrIdx;
		}
		mesh.m_pIdxBuffer->Unlock();

	}

	//Child
	for (auto pNode : listChild)
	{
		pNode->RecursiveRecalculateVtx();
	}
#endif
}

//--------------------------------------------------------------------------------
//  analyzePos
//--------------------------------------------------------------------------------
void CMyNode::analyzePos(FbxMesh* pMesh)
{
	//���_���̎擾
	auto nNumPoint = pMesh->GetControlPointsCount();

	//���_�o�b�t�@�̍쐬
	auto& meshNow = vecMesh.back();
	meshNow.vecPoint.resize(nNumPoint);

	//���_���擾
	auto pFbxV4 = pMesh->GetControlPoints();
	for (int nCnt = 0; nCnt < nNumPoint; ++nCnt)
	{
		meshNow.vecPoint[nCnt].vPos.m_fX = static_cast<float>(pFbxV4[nCnt][0]);
		meshNow.vecPoint[nCnt].vPos.m_fY = static_cast<float>(pFbxV4[nCnt][1]);
		meshNow.vecPoint[nCnt].vPos.m_fZ = static_cast<float>(pFbxV4[nCnt][2]);
	}

	//�C���f�b�N�X�̎擾
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

		//�@���f�[�^�̎擾
		auto& meshNow = vecMesh.back();
		auto nNumNormal = pElementNormal->GetDirectArray().GetCount();
		meshNow.vecNormal.resize(nNumNormal);
		for (int nCnt = 0; nCnt < nNumNormal; ++nCnt)
		{
			meshNow.vecNormal[nCnt].m_fX = (float)pElementNormal->GetDirectArray()[nCnt][0];
			meshNow.vecNormal[nCnt].m_fY = (float)pElementNormal->GetDirectArray()[nCnt][1];
			meshNow.vecNormal[nCnt].m_fZ = (float)pElementNormal->GetDirectArray()[nCnt][2];
		}

		//�}�b�s���O���[�h�A���t�@�����X���[�h�擾
		auto mappingMode = pElementNormal->GetMappingMode();
		auto referenceMode = pElementNormal->GetReferenceMode();
		if (mappingMode == FbxLayerElement::eByPolygonVertex)
		{//�@���Ǝ��̃C���f�b�N�X���g�p
			if (referenceMode == FbxLayerElement::eDirect)
			{// �C���f�b�N�X�Q�Ƃ̕K�v�Ȃ�
				meshNow.vecNormalIdx.resize(nNumNormal);
				for (int nCnt = 0; nCnt < nNumNormal; ++nCnt)
				{
					meshNow.vecNormalIdx[nCnt] = nCnt;
				}
			}
			else if (referenceMode == FbxLayerElement::eIndexToDirect
				|| referenceMode == FbxLayerElement::eIndex)
			{// �Ǝ��C���f�b�N�X������ 
			 // �C���f�b�N�X���̎擾                 
				int nNumNormalIdx = pElementNormal->GetIndexArray().GetCount();

				// �@���C���f�b�N�X�i�[�p�R���e�i�̗̈�\��
				meshNow.vecNormalIdx.resize(nNumNormalIdx);

				// �@���C���f�b�N�X�̎擾  
				for (int nCnt = 0; nCnt < nNumNormalIdx; ++nCnt)
				{
					meshNow.vecNormalIdx[nCnt] = pElementNormal->GetIndexArray()[nCnt];
				}
			}
		}

		else if (mappingMode == FbxLayerElement::eByControlPoint)
		{//���_�o�b�t�@�Ɠ����C���f�b�N�X
			if (referenceMode == FbxLayerElement::eDirect)
			{
				meshNow.vecNormalIdx.resize(meshNow.vecPointIdx.size());
				meshNow.vecNormalIdx.assign(meshNow.vecPointIdx.begin(), meshNow.vecPointIdx.end());
			}
		}
		else
		{
			MessageBox(NULL, "Normal Mode ERROR!!", "������", MB_OK);
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
		MessageBox(NULL, "���C���[�������Ă��Ȃ����b�V�����m�F", "analyzeUV", MB_OK);
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

		//�}�b�s���O���[�h�A���t�@�����X���[�h�擾
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
			MessageBox(NULL, "�}�b�s���O���[�h��Ή�", "analyzeUV", MB_OK);
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
	//�}�e���A�����̎擾
	int nMaterialCnt = pNode->GetMaterialCount();
	vecTex.resize(nMaterialCnt);

	for (int nCnt = 0; nCnt < nMaterialCnt; ++nCnt)
	{
		auto pMaterial = pNode->GetMaterial(nCnt);
		if (!pMaterial) { continue; }

		// �f�B�t���[�Y���̎擾     
		auto diffuseProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
		int nLayeredTextureCnt = diffuseProperty.GetSrcObjectCount<FbxLayeredTexture>();

		if (nLayeredTextureCnt == 0)
		{
			int nTextureCount = diffuseProperty.GetSrcObjectCount<FbxFileTexture>();

			//�e�e�N�X�`���ɂ��ăe�N�X�`�����擾
			for (int nCntTex = 0; nCntTex < nTextureCount; ++nCntTex)
			{//nCnt�Ԗڂ̃e�N�X�`���I�u�W�F�N�g�̎擾
				auto pTexture = diffuseProperty.GetSrcObject<FbxFileTexture>(nCntTex);
				if (!pTexture) { continue; }

				//�e�N�X�`���t�@�C����
				string strPath = pTexture->GetFileName();
				string strType;
				
				CKFUtility::AnalyzeTexPath(strPath, vecTex[nCnt].strName, strType);
				CKFUtility::CorrectTexType(strType);
				vecTex[nCnt].strName += '.' + strType;
				CMain::GetManager()->GetTextureManager()->UseTexture(vecTex[nCnt].strName);

				//UVSet��
				vecTex[nCnt].strUVSetName = pTexture->UVSet.Get().Buffer();

				//�Ƃ肠����0�Ԃ����Ƃ�
				break;
			}
		}
		else
		{
			//�e�e�N�X�`���ɂ��ăe�N�X�`�����擾
			int nCntTex = 0;
			for (int nCntLayer = 0; nCntLayer < nLayeredTextureCnt; ++nCntLayer)
			{//nCnt�Ԗڂ̃e�N�X�`���I�u�W�F�N�g�̎擾
				auto pLayeredTexture = diffuseProperty.GetSrcObject<FbxLayeredTexture>(nCntLayer);
				int nTextureCount = pLayeredTexture->GetSrcObjectCount<FbxFileTexture>();

				//�e�e�N�X�`���ɂ��ăe�N�X�`�����擾
				for (int nCnt = 0; nCnt < nTextureCount; ++nCnt)
				{//nCnt�Ԗڂ̃e�N�X�`���I�u�W�F�N�g�̎擾
					auto pTexture = pLayeredTexture->GetSrcObject<FbxFileTexture>(nCnt);
					if (!pTexture) { continue; }

					//�e�N�X�`���t�@�C����
					string strPath = pTexture->GetFileName();
					string strType;
					CKFUtility::AnalyzeTexPath(strPath, vecTex[nCnt].strName, strType);
					CKFUtility::CorrectTexType(strType);
					vecTex[nCnt].strName += '.' + strType;
					CMain::GetManager()->GetTextureManager()->UseTexture(vecTex[nCnt].strName);

					//UVSet��
					vecTex[nCnt].strUVSetName = pTexture->UVSet.Get().Buffer();

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
			{// ���b�V���S�������̃}�e���A���C���f�b�N�X 
				meshNow.nMaterialIndex = pElementMaterial->GetIndexArray()[0];
			}
			else
			{
				MessageBox(NULL, "...���Ή��̃��t�@�����X���[�h���擾����", "Material MappingMode = eAllSame", MB_OK);
			}
		}
		else if (mappingMode == FbxLayerElement::eByControlPoint)
		{
			MessageBox(NULL, "...���Ή��̃}�b�s���O���[�h[eByControlPoint]���擾����", "Material MappingMode", MB_OK);
		}
		else if (mappingMode == FbxLayerElement::eByPolygon)
		{// �}�e���A����������Ă���͂�������A��Ԃ͂��߂̂����ł���         
			meshNow.nMaterialIndex = pElementMaterial->GetIndexArray()[0];
		}
		else if (mappingMode == FbxLayerElement::eByEdge)
		{
			MessageBox(NULL, "...���Ή��̃}�b�s���O���[�h[eByEdge]���擾����", "Material MappingMode", MB_OK);
		}
		else
		{
			MessageBox(NULL, "...���Ή��̃}�b�s���O���[�h���擾����", "Material MappingMode", MB_OK);
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

	// �X�L���̐����擾 
	int nNumSkin = pMesh->GetDeformerCount(FbxDeformer::eSkin);

	for (int nCntSkin = 0; nCntSkin < nNumSkin; ++nCntSkin)
	{
		// �X�L�����擾 
		auto pSkin = (FbxSkin*)pMesh->GetDeformer(nCntSkin, FbxDeformer::eSkin);

		// �N���X�^�[�̐����擾 
		int nNumCluster = pSkin->GetClusterCount();

		for (int nCntCluster = 0; nCntCluster < nNumCluster; ++nCntCluster)
		{
			// �N���X�^���擾
			auto pCluster = pSkin->GetCluster(nCntCluster);

			// ���̃N���X�^���e�����y�ڂ����_�C���f�b�N�X�̌����擾 
			int nNumPointIdx = pCluster->GetControlPointIndicesCount();

			if (!nNumPointIdx)
			{// ���̃��b�V���ɂ����āA���̃N���X�^�͖������Ă����Ǝv��...                 
				meshNow.vecMtx.push_back(mtxIdentity);
				continue;
			}

			// �����p���s��̎擾 
			FbxAMatrix lReferenceGlobalInitPosition;
			FbxAMatrix lReferenceGlobalCurrentPosition;
			FbxAMatrix lClusterGlobalInitPosition;

			pCluster->GetTransformMatrix(lReferenceGlobalInitPosition);
			// lReferenceGlobalCurrentPosition = pGlobalPosition; // <- ���Ԃ񃏁[���h���W�ϊ��s��ł͂Ȃ�����  

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

			// �� �����p���s����l�����ꂽ���[�V�����{�[���s��Ȃ̂ŁA����Œ��_���W��ϊ����邾���ŗǂ� 
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
//--------------------------------------------------------------------------------
//  Load
//--------------------------------------------------------------------------------
CMyNode* CKFUtilityFBX::Load(const string& strFilePath)
{
	//FBX�Ǎ������R�[�h
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

	// �O�p����
	FbxGeometryConverter lConverter(lSdkManager);
	lConverter.Triangulate(lScene, true);

	// �k�ރ|���S���̍폜
	lConverter.RemoveBadPolygonsFromMeshes(lScene);

	// �}�e���A�����ƂɃ��b�V������
	lConverter.SplitMeshesPerMaterial(lScene, true);

	auto pRootNode = recursiveNode(lSdkManager, lScene->GetRootNode());

	lImporter->Destroy();
	lScene->Destroy();
	lSdkManager->Destroy();

	return pRootNode;
}

#ifdef USING_DIRECTX
//--------------------------------------------------------------------------------
//  FindRepetition
//	�d�˂���ԍ���Ԃ��A��������Ȃ��Ȃ�-1��Ԃ�
//--------------------------------------------------------------------------------
int CKFUtilityFBX::FindRepetition(const list<VtxDX>& listVtx, const VtxDX& vtx)
{
	int nCnt = 0;
	for (auto itr = listVtx.begin(); itr != listVtx.end(); ++itr, ++nCnt)
	{
		if (vtx == *itr) { return nCnt; }
	}

	return -1;
}
#endif

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

	// �}�e���A�����̉�́i�}�e���A�����X�g���j                
	pMyNode->analyzeTexture(pNode);

	for (int nCnt = 0; nCnt < pNode->GetNodeAttributeCount(); nCnt++)
	{
		auto type = pNode->GetNodeAttributeByIndex(nCnt)->GetAttributeType();
		pMyNode->vecAttributeName.push_back(getAttributeTypeName(type));

		if (type == FbxNodeAttribute::eMesh)
		{//Mesh���
			pMyNode->vecMesh.push_back(Mesh());

			// ���b�V�����̎擾                 
			FbxMesh* pMesh = FbxCast<FbxMesh>(pNode->GetNodeAttributeByIndex(nCnt));

			// ���_���W���                 
			pMyNode->analyzePos(pMesh);

			// �@�����                 
			pMyNode->analyzeNormal(pMesh);

			// UV���                
			pMyNode->analyzeUV(pMesh);

			// �}�e���A����́i�Q�Ə��̎擾�j                 
			pMyNode->analyzeMaterial(pMesh);

			// �{�[����� 
			pMyNode->analyzeCluster(pMesh);
		}
		else if (type == FbxNodeAttribute::eSkeleton)
		{//Bone
		 // ���b�V���ł͂Ȃ��A�g���r���[�g   
		 //MessageBox(NULL, getAttributeTypeName(type).c_str(), "�A�g���r���[�g", MB_OK); 
		}
		else
		{
			// ���b�V���ł͂Ȃ��A�g���r���[�g   
			MessageBox(NULL, getAttributeTypeName(type).c_str(), "�A�g���r���[�g", MB_OK);
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