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
#include "materialManager.h"
#include "rendererDX.h"
#include "meshManager.h"
#include <map>

//--------------------------------------------------------------------------------
//  �ÓI�����o�ϐ�
//--------------------------------------------------------------------------------
#ifdef USING_DIRECTX
LPD3DXMESH CMyNode::s_pMeshSphere = nullptr;
LPD3DXMESH CMyNode::s_pMeshCube = nullptr;
#endif

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
				|| vecBornRefarence[nCnt].sIndex != vValue.vecBornRefarence[nCnt].sIndex)
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
void CAnimator::UpdateBones(const Frame& current)
{
	for (int count = 0; count < Clusters.size(); ++count)
	{
		Clusters[count].Node->local = current.BoneFrames[count].Matrix;
	}
}

void CAnimator::UpdateClusterWorld(void)
{
	for (auto& cluster : Clusters)
	{
		cluster.World = cluster.RelativeInitPosition * cluster.Node->world;
	}
}

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
		tex.Name.clear();
		tex.strUVSetName.clear();
	}
	vecTex.clear();

	//Mesh
	for (auto& mesh : vecMesh)
	{
		//mesh.vecMtx.clear();
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
		CMain::GetManager()->GetTextureManager()->DisuseTexture(mesh.strTexName);
		mesh.strTexName.clear();

#ifdef USING_DIRECTX
		SAFE_RELEASE(mesh.m_pVtxBuffer);
		SAFE_RELEASE(mesh.m_pIdxBuffer);
		SAFE_RELEASE(s_pMeshCube);
		SAFE_RELEASE(s_pMeshSphere);
#endif
	}
	vecMesh.clear();

	//Collider
	listCollider.clear();

	//Child
	for (auto pChild : listChild)
	{
		pChild->Release();
	}
	listChild.clear();

	//Other
	Name.clear();
	vecAttributeName.clear();

	delete this;
}

//--------------------------------------------------------------------------------
//  RecursiveUpdateMatrix
//--------------------------------------------------------------------------------
void CMyNode::RecursiveUpdateMatrix(const CKFMtx44& parent)
{
	world = local;
	world *= parent;
	for (auto pNode : listChild)
	{
		pNode->RecursiveUpdateMatrix(world);
	}
}

//--------------------------------------------------------------------------------
//  RecursiveUpdateSkin
//--------------------------------------------------------------------------------
void CMyNode::RecursiveUpdateSkin(const vector<Cluster>& clusters)
{
	for (auto& mesh : vecMesh)
	{
#ifdef USING_DIRECTX
		// ������i�܂胏���X�L���Ȃǁj
		// ���_�̍��W�ϊ� 
		VERTEX_3D* pVtx;
		mesh.m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

//#pragma omp parallel for 
		for (int nCnt = 0; nCnt < (int)mesh.m_vecVtx.size(); ++nCnt)
		{
			CKFMtx44 mtx;
			auto& vtxDX = mesh.m_vecVtx[nCnt];
			if (!vtxDX.vecBornRefarence.empty())
			{
				ZeroMemory(&mtx, sizeof(CKFMtx44));
				for (auto& bornRefarence : vtxDX.vecBornRefarence)
				{
					mtx += clusters[bornRefarence.sIndex].World * bornRefarence.fWeight;
				}
			}
			pVtx[nCnt].vPos = CKFMath::Vec3TransformCoord(vtxDX.vtx.vPos, mtx);
		}

		mesh.m_pVtxBuffer->Unlock();
#endif
	}

	//Child
	for (auto pNode : listChild)
	{
		pNode->RecursiveUpdateSkin(clusters);
	}
}

//--------------------------------------------------------------------------------
//  RecursiveRecalculateClusterID
//--------------------------------------------------------------------------------
void CMyNode::RecursiveRecalculateClusterID(const Frame& initFrame)
{
	for (auto& mesh : vecMesh)
	{
//#pragma omp parallel for 
		for (int nCnt = 0; nCnt < (int)mesh.m_vecVtx.size(); ++nCnt)
		{
			auto& vtxDX = mesh.m_vecVtx[nCnt];
			for (auto& bornRefarence : vtxDX.vecBornRefarence)
			{
				for (int countBoneFrame = 0; countBoneFrame < initFrame.BoneFrames.size();++countBoneFrame)
				{
					if (bornRefarence.strBoneName == initFrame.BoneFrames[countBoneFrame].Name)
					{
						bornRefarence.sIndex = countBoneFrame;
						break;
					}
				}
			}
		}
	}

	//Child
	for (auto pNode : listChild)
	{
		pNode->RecursiveRecalculateClusterID(initFrame);
	}
}

void CMyNode::RecursiveTransformVtxToBoneSpace(const vector<Cluster>& clusters)
{
	for (auto& mesh : vecMesh)
	{
//#pragma omp parallel for 
		for (int nCnt = 0; nCnt < (int)mesh.m_vecVtx.size(); ++nCnt)
		{
			CKFMtx44 mtx;
			auto& vtxDX = mesh.m_vecVtx[nCnt];
			if (!vtxDX.vecBornRefarence.empty())
			{
				ZeroMemory(&mtx, sizeof(CKFMtx44));
				for (auto& bornRefarence : vtxDX.vecBornRefarence)
				{
					mtx += clusters[bornRefarence.sIndex].RelativeInitPosition * bornRefarence.fWeight;
				}
			}
			//vtxDX.vtx.vPos = CKFMath::Vec3TransformCoord(vtxDX.vtx.vPos, mtx);
		}
	}

	//Child
	for (auto pNode : listChild)
	{
		pNode->RecursiveTransformVtxToBoneSpace(clusters);
	}
}

//--------------------------------------------------------------------------------
//  RecursiveDraw
//--------------------------------------------------------------------------------
void CMyNode::RecursiveDraw(const bool& drawSkeleton, const bool& drawMesh, const bool& drawCollider)
{
#ifdef USING_DIRECTX
	auto pDevice = CMain::GetManager()->GetRenderer()->GetDevice();

	//Mesh
	if (drawMesh)
	{
		for (auto& mesh : vecMesh)
		{
			if (mesh.m_renderPriority == RP_3D_ALPHATEST)
			{
				pDevice->SetRenderState(D3DRS_ALPHAREF, (DWORD)0x00000001);
				pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
				pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
			}

			CRenderState* renderState = nullptr;
			if (mesh.m_bEnableLight)
			{
				if (mesh.m_bEnableCullFace) { renderState = &CDrawComponent::s_lightOnCullOn; }
				else { renderState = &CDrawComponent::s_lightOnCullOff; }
			}
			else
			{
				if (mesh.m_bEnableCullFace) { renderState = &CDrawComponent::s_lightOffCullOn; }
				else { renderState = &CDrawComponent::s_lightOffCullOff; }
			}

			renderState->SetRenderState();

			//Texture
			if (!mesh.strTexName.empty())
			{
				auto pTexture = CMain::GetManager()->GetTextureManager()->GetTexture(mesh.strTexName/*vecTex[mesh.nMaterialIndex].Name*/);
				pDevice->SetTexture(0, pTexture);
			}

			D3DXMATRIX mtx = world;
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

			renderState->ResetRenderState();

			if (mesh.m_renderPriority == RP_3D_ALPHATEST)
			{
				pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
			}
		}
	}

	if(!s_pMeshSphere)
	{
		CMain::GetManager()->GetTextureManager()->UseTexture("polygon.png");
		D3DXCreateSphere(pDevice, 1.0f, 10, 10, &s_pMeshSphere, nullptr); 
	}
	if(!s_pMeshCube){ D3DXCreateBox(pDevice, 1.0f, 1.0f, 1.0f, &s_pMeshCube, nullptr); }
	auto pTexture = CMain::GetManager()->GetTextureManager()->GetTexture("polygon.png");
	D3DMATERIAL9 material = CMain::GetManager()->GetMaterialManager()->GetMaterial(materialID);
	D3DMATERIAL9 matDef;
	pDevice->GetMaterial(&matDef);
	pDevice->SetTexture(0, pTexture);
	pDevice->SetMaterial(&material);

	// Skeleton
	if (drawSkeleton)
	{
		if (!vecAttributeName.empty() && vecAttributeName[0] == "skeleton")
		{
			D3DXMATRIX mtx = world;
			pDevice->SetTransform(D3DTS_WORLD, &mtx);
			s_pMeshSphere->DrawSubset(0);
		}
	}
	
	//Draw Collider
	if (drawCollider)
	{
		for (auto& col : listCollider)
		{
			CKFMtx44 mtxCol, mtxRot, mtxPos;

			//�g�k
			mtxCol.m_af[0][0] = col.vOffsetScale.m_fX;
			mtxCol.m_af[1][1] = col.vOffsetScale.m_fY;
			mtxCol.m_af[2][2] = col.vOffsetScale.m_fZ;

			//��]
			CKFMath::MtxRotationYawPitchRoll(mtxRot, col.vOffsetRot);
			mtxCol *= mtxRot;

			//���s�ړ�
			CKFMath::MtxTranslation(mtxPos, col.vOffsetPos);
			mtxCol *= mtxPos;
			mtxCol *= world;

			if (col.colType == CS::COL_SPHERE)
			{
				D3DXMATRIX mtx = mtxCol;
				pDevice->SetTransform(D3DTS_WORLD, &mtx);
				s_pMeshSphere->DrawSubset(0);
			}
			else if (col.colType == CS::COL_AABB)
			{
				mtxCol.m_af[0][1] = mtxCol.m_af[0][2]
					= mtxCol.m_af[1][0] = mtxCol.m_af[1][2]
					= mtxCol.m_af[2][0] = mtxCol.m_af[2][1] = 0.0f;
				D3DXMATRIX mtx = mtxCol;
				pDevice->SetTransform(D3DTS_WORLD, &mtx);
				s_pMeshCube->DrawSubset(0);
			}
			else if (col.colType == CS::COL_OBB)
			{
				D3DXMATRIX mtx = mtxCol;
				pDevice->SetTransform(D3DTS_WORLD, &mtx);
				s_pMeshCube->DrawSubset(0);
			}
		}
	}
	pDevice->SetTexture(0, nullptr);
	pDevice->SetMaterial(&matDef);

	//Child
	for (auto pNode : listChild)
	{
		pNode->RecursiveDraw(drawSkeleton, drawMesh, drawCollider);
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
					mtx += mesh.vecMtx[bornRefarence.sIndex] * bornRefarence.fWeight;
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
#endif

	//Child
	for (auto pNode : listChild)
	{
		pNode->RecursiveRecalculateVtx();
	}
}

//--------------------------------------------------------------------------------
//  RecursiveReverseTexV
//--------------------------------------------------------------------------------
void CMyNode::RecursiveReverseTexV(void)
{
#ifdef USING_DIRECTX
	for (auto& mesh : vecMesh)
	{
		VERTEX_3D *pVtx;
		mesh.m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);
		for (int nCnt = 0; nCnt < mesh.m_nNumVtx; ++nCnt)
		{
			mesh.m_vecVtx[nCnt].vtx.vUV.m_fY = 1.0f - mesh.m_vecVtx[nCnt].vtx.vUV.m_fY;
			pVtx[nCnt].vUV.m_fY = mesh.m_vecVtx[nCnt].vtx.vUV.m_fY;
		}
		mesh.m_pVtxBuffer->Unlock();
	}
#endif

	//Child
	for (auto pNode : listChild)
	{
		pNode->RecursiveReverseTexV();
	}
}

//--------------------------------------------------------------------------------
//  RecursiveReverseTexV
//--------------------------------------------------------------------------------
void CMyNode::RecalculateVtxByMatrix(const CKFMtx44& mtx)
{
#ifdef USING_DIRECTX
	for (auto& mesh : vecMesh)
	{
		VERTEX_3D *pVtx;
		mesh.m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);
		for (int nCnt = 0; nCnt < mesh.m_nNumVtx; ++nCnt)
		{
			auto& vtx = mesh.m_vecVtx[nCnt].vtx;
			vtx.vPos = CKFMath::Vec3TransformCoord(vtx.vPos, mtx);
			vtx.vNormal = CKFMath::Vec3TransformNormal(vtx.vNormal, mtx);
			CKFMath::VecNormalize(vtx.vNormal);
			pVtx[nCnt].vPos = vtx.vPos;
			pVtx[nCnt].vNormal = vtx.vNormal;
		}
		mesh.m_pVtxBuffer->Unlock();
	}
#endif

	for (auto pChild : listChild)
	{
		pChild->vTrans = CKFMath::Vec3TransformCoord(pChild->vTrans, mtx);
		pChild->RecalculateVtxByMatrix(mtx);
	}
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
	vecTex.reserve(nMaterialCnt);

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
				Texture texture;
				string strPath = pTexture->GetFileName();
				string strType;
				
				CKFUtility::AnalyzeFilePath(strPath, texture.Name, strType);
				CKFUtility::CorrectTexType(strType);
				texture.Name += '.' + strType;

				//UVSet��
				texture.strUVSetName = pTexture->UVSet.Get().Buffer();

				//�Ƃ肠����0�Ԃ����Ƃ�
				vecTex.push_back(texture);
				break;
			}
		}
		else
		{
			//�e�e�N�X�`���ɂ��ăe�N�X�`�����擾
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
					Texture texture;
					string strPath = pTexture->GetFileName();
					string strType;

					CKFUtility::AnalyzeFilePath(strPath, texture.Name, strType);
					CKFUtility::CorrectTexType(strType);
					texture.Name += '.' + strType;

					//UVSet��
					texture.strUVSetName = pTexture->UVSet.Get().Buffer();

					//�Ƃ肠����0�Ԃ����Ƃ�
					vecTex.push_back(texture);
					break;
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
				if (meshNow.nMaterialIndex < vecTex.size())
				{
					meshNow.strTexName = vecTex[meshNow.nMaterialIndex].Name;
					CMain::GetManager()->GetTextureManager()->UseTexture(meshNow.strTexName);
				}
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
			if (meshNow.nMaterialIndex < vecTex.size())
			{
				meshNow.strTexName = vecTex[meshNow.nMaterialIndex].Name;
				CMain::GetManager()->GetTextureManager()->UseTexture(meshNow.strTexName);
			}
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
	//CKFMtx44 mtxIdentity;

	// �X�L���̐����擾 
	int nNumSkin = pMesh->GetDeformerCount(FbxDeformer::eSkin);

	for (int nCntSkin = 0; nCntSkin < nNumSkin; ++nCntSkin)
	{
		// �X�L�����擾 
		auto pSkin = (FbxSkin*)pMesh->GetDeformer(nCntSkin, FbxDeformer::eSkin);

		// �N���X�^�[�̐����擾 
		int nNumCluster = pSkin->GetClusterCount();
		//vector<CKFMtx44> matrices;
		//matrices.resize(meshNow.vecPoint.size());
		//CKFMtx44 zero;
		//ZeroMemory(&zero, sizeof(CKFMtx44));
		//for (auto& mtx : matrices) { mtx = zero; }

		for (int nCntCluster = 0; nCntCluster < nNumCluster; ++nCntCluster)
		{
			// In FBX, to get the local coordinates in "LinkNode" space you need to do two steps:
			// 1. Control points are stored as in the mesh's object space. First move them from object space to world space.
			// Note: For all clusters related to one mesh, their Transform matrix is actually the same, because there is actually only one mesh.
			//FbxAMatrix meshGlobal;
			//auto lCluster = pSkin->GetCluster(nCntCluster);
			//lCluster->GetTransformMatrix(meshGlobal);
			////FbxAMatrix meshGeometry = pMesh->GetNode()->EvaluateLocalTransform();
			////meshGlobal *= meshGeometry;
			//auto& kfMeshClobal = CKFMtx44::FbxToMtx(meshGlobal);
			//
			//// 2. Transform control points from World Space to the Bone�fs space at binding moment.
			//FbxAMatrix boneBindingMatrix;
			//lCluster->GetTransformLinkMatrix(boneBindingMatrix);
			//auto& boneBindingMatrixInverse = CKFMtx44::FbxToMtx(boneBindingMatrix.Inverse());
			//auto& result = boneBindingMatrixInverse * kfMeshClobal;
			//for (auto& point : meshNow.vecPoint)
			//{
			//	// lDstVertex = meshGlobal.MultT(lSrcVertex);
			//	point.vPos = CKFMath::Vec3TransformCoord(point.vPos, kfMeshClobal);
			//	
			//	// lBoneBindingMatrix.Inverse() * lDstVertex;
			//	point.vPos = CKFMath::Vec3TransformCoord(point.vPos, boneBindingMatrixInverse);
			//}

			// �N���X�^���擾
			auto pCluster = pSkin->GetCluster(nCntCluster);
			string strClusterName = pCluster->GetLink()->GetName();

			// ���̃N���X�^���e�����y�ڂ����_�C���f�b�N�X�̌����擾 
			int nNumPointIdx = pCluster->GetControlPointIndicesCount();
			auto pPointIndexArray = pCluster->GetControlPointIndices();
			auto pWeightArray = pCluster->GetControlPointWeights();
			for (int nCnt = 0; nCnt < nNumPointIdx; ++nCnt)
			{
				meshNow.vecPoint[pPointIndexArray[nCnt]].vecBornRefarence.push_back(
					BornRefarence(nCntCluster, static_cast<float>(pWeightArray[nCnt]), strClusterName));
				
				// Test
				//matrices[pPointIndexArray[nCnt]] += result * static_cast<float>(pWeightArray[nCnt]);
			}
		}

		//for (int count = 0; count < meshNow.vecPoint.size(); ++count)
		//{
		//	meshNow.vecPoint[count].vPos = CKFMath::Vec3TransformCoord(meshNow.vecPoint[count].vPos, matrices[count]);
		//}
	}
}

//--------------------------------------------------------------------------------
//
//  CKFUtilityFBX
//
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  Load
//--------------------------------------------------------------------------------
MyModel CKFUtilityFBX::Load(const string& strFilePath)
{
	MyModel myModel;

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
		char buf[MAX_PATH];
		wsprintf(buf, "Call to FbxImporter::Initialize() failed.\nError returned: %s\n\n", lImporter->GetStatus().GetErrorString());
		MessageBox(NULL, buf, "error", MB_OK);
		lImporter->Destroy();
		lSdkManager->Destroy();
		return myModel;
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

	//Node
	myModel.pNode = recursiveNode(lSdkManager, lScene->GetRootNode());

	// Animation
	myModel.pAnimator = analyzeAnimation(lImporter, lScene);
	if (myModel.pAnimator)
	{
		matchClusterWithSkeleton(myModel.pAnimator->Clusters, myModel.pNode);
	}

	lImporter->Destroy();
	lScene->Destroy();
	lSdkManager->Destroy();

	return myModel;
}

//--------------------------------------------------------------------------------
//  Load
//--------------------------------------------------------------------------------
MyModel CKFUtilityFBX::LoadFromTxt(const string& strFilePath)
{
	MyModel myModel;
	FILE* filePointer = nullptr;
	fopen_s(&filePointer, strFilePath.c_str(), "r");
	if (!filePointer) return myModel;
	return myModel;
	////�p�[�c���̓ǂݍ���
	//string buffer;
	//string compare = "NUM_MODEL = ";
	//int partsNumber = 0;
	//if (CKFUtility::GetStringUntilString(filePointer, compare, buffer) > 0)
	//{
	//	char numberBuffer[16];
	//	for (int count = compare.length(), countNum = 0; buffer[count] != '\n'; ++count, ++countNum)
	//	{
	//		numberBuffer[countNum] = buffer[count];
	//	}
	//	partsNumber = atoi(numberBuffer);
	//}

	////Mesh�̓ǂݍ���
	//vector<string> modelPathes;
	//modelPathes.resize(partsNumber);
	//for (int countPart = 0; countPart < partsNumber; countPart++)
	//{
	//	compare = "MODEL_FILENAME = ";
	//	if (CKFUtility::GetStringUntilString(filePointer, compare, buffer) > 0)
	//	{
	//		for (int count = compare.length(), countPath = 0; buffer[count] != '\t'; ++count, countPath++)
	//		{
	//			modelPathes[countPart].push_back(buffer[count]);
	//		}
	//	}
	//}

	////parts info�̓ǂݍ���
	//vector<CMyNode*> nodes;
	//nodes.resize(partsNumber);
	//for (int countPart = 0; countPart < partsNumber; ++countPart)
	//{
	//	if (CKFUtility::GetStringUntilString(filePointer, "\tPARTSSET", buffer) > 0)
	//	{
	//		//Index
	//		fgets(&buffer[0], buffer.length(), filePointer);

	//		//Parent
	//		fgets(&buffer[0], buffer.length(), filePointer);
	//		char parentIDBuffer[4] = {};
	//		for (unsigned int count = strlen("\t\tPARENT = "), countParent = 0; buffer[count] != '\t'; ++count, ++countParent)
	//		{
	//			parentIDBuffer[countParent] = buffer[count];
	//		}
	//		int parentID = atoi(parentIDBuffer);

	//		//Pos
	//		fgets(&buffer[0], buffer.length(), filePointer);
	//		char positionBuffer[3][16] = {};
	//		int countAxis = 0;
	//		int countPos = 0;
	//		for (int count = strlen("\t\tPOS = "); buffer[count] != '\n'; ++count)
	//		{
	//			if (buffer[count] == ' ')
	//			{
	//				countAxis++;
	//				countPos = 0;
	//				continue;
	//			}
	//			positionBuffer[countAxis][countPos] = buffer[count];
	//			countPos++;
	//		}

	//		auto offsetPosition = CKFVec3((float)atof(positionBuffer[0]), (float)atof(positionBuffer[1]), (float)atof(positionBuffer[2]));

	//		//Rot
	//		fgets(&buffer[0], buffer.length(), filePointer);
	//		char rotationBuffer[3][16] = {};
	//		countAxis = 0;
	//		int countRot = 0;
	//		for (unsigned int count = strlen("\t\tROT = "); buffer[count] != '\n'; ++count)
	//		{
	//			if (buffer[count] == ' ')
	//			{
	//				countAxis++;
	//				countRot = 0;
	//				continue;
	//			}

	//			rotationBuffer[countAxis][countRot] = buffer[count];
	//			countRot++;
	//		}

	//		auto offsetRotation = CKFVec3((float)atof(rotationBuffer[0]), (float)atof(rotationBuffer[1]), (float)atof(rotationBuffer[2]));
	//		nodes[countPart] = new CMyNode;
	//		nodes[countPart]->vTrans = offsetPosition;
	//		nodes[countPart]->vRot = offsetRotation;
	//		nodes[countPart]->vScale = CKFVec3(1.0f);
	//		if (parentID >= 0)
	//		{
	//			nodes[parentID]->listChild.push_back(nodes[countPart]);
	//		}

	//		// mesh
	//		string Name, strType;
	//		CKFUtility::AnalyzeFilePath(modelPathes[countPart], Name, strType);
	//		if (strType._Equal("x"))
	//		{

	//		}
	//	}
	//}
	//myModel.pNode = nodes[0];

	////motion�̓ǂݍ���
	//int countMotion = 0;
	//while (CKFUtility::GetStringUntilString(filePointer, "MOTIONSET", buffer) > 0)
	//{
	//	MotionInfo motionInfo;

	//	//LOOP
	//	if (CKFUtility::GetStringUntilString(filePointer, "LOOP = ", buffer) > 0)
	//	{
	//		char loopBuffer[2] = {};
	//		for (int count = strlen("\tLOOP = "), countLoop = 0; buffer[count] != '\t'; ++count, ++countLoop)
	//		{
	//			loopBuffer[countLoop] = buffer[count];
	//		}
	//		motionInfo.IsLoop = atoi(loopBuffer) != 0 ? true : false;
	//	}

	//	//�L�[�t���[�����擾
	//	int keyNumber = 0;
	//	if (CKFUtility::GetStringUntilString(filePointer, "NUM_KEY = ", buffer) > 0)
	//	{
	//		char keyBuffer[2] = {};
	//		for (int count = strlen("\tNUM_KEY = "), countKey = 0; buffer[count] != '\t'; ++count, countKey++)
	//		{
	//			keyBuffer[countKey] = buffer[count];
	//		}
	//		keyNumber = atoi(keyBuffer);
	//	}

	//	vector<MotionFrame> keyFrames;
	//	vector<int> keyFrameNumbers;
	//	keyFrames.resize(keyNumber);
	//	keyFrameNumbers.resize(keyNumber);

	//	//�L�[�t���[���̓ǂݍ���
	//	for (int countKey = 0; countKey < keyNumber; ++countKey)
	//	{
	//		if (CKFUtility::GetStringUntilString(filePointer, "KEYSET", buffer) > 0)
	//		{
	//			//Frame
	//			if (CKFUtility::GetStringUntilString(filePointer, "FRAME = ", buffer) > 0)
	//			{
	//				char frameBuffer[4] = {};
	//				for (unsigned int count = strlen("\t\tFRAME = "), countFrame = 0; buffer[count] != '\n'; ++count, ++countFrame)
	//				{
	//					frameBuffer[countFrame] = buffer[count];
	//				}
	//				keyFrameNumbers[countKey] = atoi(frameBuffer);
	//			}

	//			//Set Key per Parts
	//			for (int countPart = 0; countPart < partsNumber; ++countPart)
	//			{
	//				CKFVec3 position;
	//				CKFQuaternion rotation;

	//				//Pos
	//				if (CKFUtility::GetStringUntilString(filePointer, "POS = ", buffer) > 0)
	//				{
	//					char positionBuffer[3][16] = {};
	//					int countAxis = 0;
	//					int countPos = 0;
	//					for (unsigned int count = strlen("\t\t\tPOS = "); buffer[count] != '\n'; ++count)
	//					{
	//						if (buffer[count] == ' ')
	//						{
	//							countAxis++;
	//							countPos = 0;
	//							continue;
	//						}

	//						positionBuffer[countAxis][countPos] = buffer[count];
	//						countPos++;
	//					}
	//					position = CKFVec3((float)atof(positionBuffer[0]), (float)atof(positionBuffer[1]), (float)atof(positionBuffer[2]));
	//				}

	//				//Rot
	//				if (CKFUtility::GetStringUntilString(filePointer, "ROT = ", buffer) > 0)
	//				{
	//					char rotationBuffer[3][16] = {};
	//					int countAxis = 0;
	//					int countRot = 0;
	//					for (unsigned int count = strlen("\t\t\tROT = "); buffer[count] != '\n'; ++count)
	//					{
	//						if (buffer[count] == ' ')
	//						{
	//							countAxis++;
	//							countRot = 0;
	//							continue;
	//						}
	//						rotationBuffer[countAxis][countRot] = buffer[count];
	//						countRot++;
	//					}
	//					rotation = CKFVec3((float)atof(rotationBuffer[0]), (float)atof(rotationBuffer[1]), (float)atof(rotationBuffer[2])).ToCKFQuaternion();
	//				}

	//				keyFrames[countKey].BoneFrames.push_back(BoneFrame(position, rotation));
	//			}
	//		}
	//	}

	//	//�t���[���ɓ]��
	//	int countFrame = 0;
	//	for (int countKey = 0; countKey < keyNumber; ++countKey)
	//	{
	//		motionInfo.MotionFrames.resize(motionInfo.MotionFrames.size() + keyFrameNumbers[countKey]);
	//		for (int countKeyFrame = 0; countKeyFrame < keyFrameNumbers[countKey]; ++countKeyFrame)
	//		{
	//			float rate = (float)countKeyFrame / keyFrameNumbers[countKey];
	//			auto currentIterator = keyFrames[countKey].BoneFrames.begin();

	//			if (!motionInfo.IsLoop && countKey == keyNumber - 1)
	//			{// �Ō�̃L�[�͕�ԂȂ�
	//				for (int countBone = 0; countBone < partsNumber; ++countBone)
	//				{
	//					motionInfo.MotionFrames[countFrame].BoneFrames.push_back(BoneFrame(currentIterator->Position, currentIterator->Rotation));
	//					++currentIterator;
	//				}
	//			}
	//			else
	//			{// ���
	//				auto nextIterator = keyFrames[(countKey + 1) % keyNumber].BoneFrames.begin();
	//				for (int countBone = 0; countBone < partsNumber; ++countBone)
	//				{
	//					auto& position = Math::Lerp(currentIterator->Position, nextIterator->Position, rate);
	//					auto& rotation = Math::Slerp(currentIterator->Rotation, nextIterator->Rotation, rate);
	//					motionInfo.MotionFrames[countFrame].BoneFrames.push_back(BoneFrame(position, rotation));
	//					++currentIterator;
	//					++nextIterator;
	//				}
	//			}
	//			++countFrame;
	//		}
	//	}
	//}
}

//--------------------------------------------------------------------------------
//  Load
//--------------------------------------------------------------------------------
void CKFUtilityFBX::LoadAnimation(const string& strFilePath, CAnimator* animator)
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
		char buf[MAX_PATH];
		wsprintf(buf, "Call to FbxImporter::Initialize() failed.\nError returned: %s\n\n", lImporter->GetStatus().GetErrorString());
		MessageBox(NULL, buf, "error", MB_OK);
		lImporter->Destroy();
		lSdkManager->Destroy();
		return;
	}

	// Create a new scene so that it can be populated by the imported file.
	auto lScene = FbxScene::Create(lSdkManager, "myScene");

	// Import the contents of the file into the scene.
	lImporter->Import(lScene);

	analyzeAnimation(lImporter, lScene, animator);

	lImporter->Destroy();
	lScene->Destroy();
	lSdkManager->Destroy();
}

//--------------------------------------------------------------------------------
//  Save
//--------------------------------------------------------------------------------
bool CKFUtilityFBX::Save(CMyNode* pRootNode, const string& strFileName)
{
	//Model�t�@�C���̕ۑ�
	string Name = "data/MODEL/" + strFileName + ".model";

	//���邩�ǂ������`�F�b�N
	FILE *pFile;
	fopen_s(&pFile, Name.c_str(), "rb");
	if (pFile)
	{//�㏑���m�F
		fclose(pFile);
		auto nID = MessageBox(NULL, "�㏑�����܂����H", "�m�F", MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2);
		if (nID == IDNO) { return false; }
	}

	//Save
	fopen_s(&pFile, Name.c_str(), "wb");
	recursiveSaveNode(pFile, pRootNode, strFileName);
	fclose(pFile);
	return true;
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
//  recursiveNode
//--------------------------------------------------------------------------------
CMyNode* CKFUtilityFBX::recursiveNode(FbxManager* pManager, FbxNode* pNode)
{
	if (!pNode) { return NULL; }
	auto pMyNode = new CMyNode;
	pMyNode->Name = pNode->GetName();
	pMyNode->vTrans.m_fX = static_cast<float>(pNode->LclTranslation.Get()[0]);
	pMyNode->vTrans.m_fY = static_cast<float>(pNode->LclTranslation.Get()[1]);
	pMyNode->vTrans.m_fZ = static_cast<float>(pNode->LclTranslation.Get()[2]);
	pMyNode->vRot.m_fX = static_cast<float>(pNode->LclRotation.Get()[0]);
	pMyNode->vRot.m_fY = static_cast<float>(pNode->LclRotation.Get()[1]);
	pMyNode->vRot.m_fZ = static_cast<float>(pNode->LclRotation.Get()[2]);
	pMyNode->vRot /= 180.0f * KF_PI; //Degree to Radian
	pMyNode->vScale.m_fX = static_cast<float>(pNode->LclScaling.Get()[0]);
	pMyNode->vScale.m_fY = static_cast<float>(pNode->LclScaling.Get()[1]);
	pMyNode->vScale.m_fZ = static_cast<float>(pNode->LclScaling.Get()[2]);
	pMyNode->local.m_af[0][0] = pMyNode->vScale.m_fX;
	pMyNode->local.m_af[1][1] = pMyNode->vScale.m_fY;
	pMyNode->local.m_af[2][2] = pMyNode->vScale.m_fZ;
	CKFMtx44 mtxRot;
	CKFMath::MtxRotationYawPitchRoll(mtxRot, pMyNode->vRot);
	pMyNode->local *= mtxRot;
	CKFMtx44 mtxPos;
	CKFMath::MtxTranslation(mtxPos, pMyNode->vTrans);
	pMyNode->local *= mtxPos;

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

			// �{�[���ɂ̏d����� 
			pMyNode->analyzeCluster(pMesh);
		}
		else if (type == FbxNodeAttribute::eSkeleton)
		{//Bone
			//auto pSkeleton = FbxCast<FbxSkeleton>(pNode->GetNodeAttributeByIndex(nCnt));
			//pMyNode->analyzeSkeleton(pSkeleton);
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
//  analyzeAnimation
//--------------------------------------------------------------------------------
CAnimator* CKFUtilityFBX::analyzeAnimation(FbxImporter* lImporter, FbxScene* lScene)
{
	// Anim��
	auto animationNumber = lImporter->GetAnimStackCount();
	if (!animationNumber) { return nullptr; }

	// Cluster
	int clusterNumber = lScene->GetMemberCount<FbxCluster>();
	map<string, FbxCluster*> clusters;
	for (int countBoneFrame = 0; countBoneFrame < clusterNumber; ++countBoneFrame)
	{
		auto pCluster = lScene->GetSrcObject<FbxCluster>(countBoneFrame);
		string name = pCluster->GetLink()->GetName();
		if (clusters.find(name) == clusters.end())
		{
			clusters.emplace(name, pCluster);
		}
	}

	if (clusters.empty())
	{
		MessageBox(NULL, "cluster��������Ȃ��I�I", "analyzeAnimation", MB_OK | MB_ICONWARNING);
		return nullptr; 
	}

	auto pAnimator = new CAnimator;
	pAnimator->Motions.reserve(animationNumber);

	// Animation Name
	FbxArray<FbxString*> animationNames;
	lScene->FillAnimStackNameArray(animationNames);

	// Cluster Matrix
	pAnimator->Clusters.resize(clusters.size());
	int countCluster = 0;
	for (auto pair : clusters)
	{
		pAnimator->Clusters[countCluster].Name = pair.first;
		FbxAMatrix meshGlobalInitPosition;
		pair.second->GetTransformMatrix(meshGlobalInitPosition);
		FbxAMatrix boneGlobalInitPosition;
		pair.second->GetTransformLinkMatrix(boneGlobalInitPosition);
		pAnimator->Clusters[countCluster].RelativeInitPosition = CKFMtx44::FbxToMtx(boneGlobalInitPosition.Inverse() * meshGlobalInitPosition);
		++countCluster;
	}

	//one frame time
	FbxTime oneFrameTime;
	oneFrameTime.SetTime(0, 0, 0, 1, 0, 0, FbxTime::eFrames60);

	for (int nCnt = 0; nCnt < animationNumber; ++nCnt)
	{
		Motion motion;
		motion.Name = animationNames[nCnt]->Buffer();

		//Anime���
		auto pTakeInfo = lScene->GetTakeInfo(animationNames[nCnt]->Buffer());
	
		//�A�j���[�V�����J�n�I������
		auto startTime = pTakeInfo->mLocalTimeSpan.GetStart();
		auto endTime = pTakeInfo->mLocalTimeSpan.GetStop();
		int frameNumber = (int)((endTime - startTime) / (oneFrameTime)).Get();
		motion.Frames.reserve(frameNumber + 1);
		for (auto currentTime = startTime; currentTime < endTime; currentTime += oneFrameTime)
		{
			Frame frame;
			frame.BoneFrames.reserve(clusters.size());
			countCluster = 0;
			for (auto pair : clusters)
			{
				BoneFrame boneFrame;
				boneFrame.Name = pair.first;

				// �A�j���[�V�����s��̎Z�o
				auto skeletonGlobalCurrentTransform = pair.second->GetLink()->EvaluateGlobalTransform(currentTime);
				FbxAMatrix lReferenceGlobalCurrentPosition;
				auto lClusterRelativeCurrentPositionInverse = lReferenceGlobalCurrentPosition.Inverse() * skeletonGlobalCurrentTransform;
				//auto VertexTransformMatrix = lClusterRelativeCurrentPositionInverse * pAnimator->Clusters[countCluster].RelativeInitPosition;
				boneFrame.Matrix = CKFMtx44::FbxToMtx(pair.second->GetLink()->EvaluateLocalTransform(currentTime));
				frame.BoneFrames.push_back(boneFrame);
				++countCluster;
			}
			motion.Frames.push_back(frame);
		}
		motion.Frames.shrink_to_fit();
		motion.EndFrame = motion.Frames.size() - 1;
		pAnimator->Motions.push_back(motion);
	}
	pAnimator->Motions.shrink_to_fit();
	return pAnimator;
}

//--------------------------------------------------------------------------------
//  analyzeAnimation
//--------------------------------------------------------------------------------
void CKFUtilityFBX::analyzeAnimation(FbxImporter* lImporter, FbxScene* lScene, CAnimator* animator)
{
	// Anim��
	auto animationNumber = lImporter->GetAnimStackCount();
	if (!animationNumber) return;

	// Skeleton
	list<FbxNode*> skeletons;
	findSkeletons(lScene->GetRootNode(), skeletons);
	if (skeletons.size() != animator->Clusters.size())
	{
		MessageBox(NULL, "skeleton�̐�������Ȃ��I�I", "analyzeAnimation", MB_OK | MB_ICONWARNING);
		return;
	}

	animator->Motions.reserve(animator->Motions.size() + animationNumber);
	FbxArray<FbxString*> animationNames;
	lScene->FillAnimStackNameArray(animationNames);

	//one frame time
	FbxTime oneFrameTime;
	oneFrameTime.SetTime(0, 0, 0, 1, 0, 0, FbxTime::eFrames60);

	for (int nCnt = 0; nCnt < animationNumber; ++nCnt)
	{
		Motion motion;
		motion.Name = animationNames[nCnt]->Buffer();

		//Anime���
		auto pTakeInfo = lScene->GetTakeInfo(animationNames[nCnt]->Buffer());

		//�A�j���[�V�����J�n�I������
		auto startTime = pTakeInfo->mLocalTimeSpan.GetStart();
		auto endTime = pTakeInfo->mLocalTimeSpan.GetStop();
		int frameNumber = (int)((endTime - startTime) / (oneFrameTime)).Get();
		motion.Frames.reserve(frameNumber + 1);
		for (auto currentTime = startTime; currentTime < endTime; currentTime += oneFrameTime)
		{
			Frame frame;
			frame.BoneFrames.resize(skeletons.size());

			for (auto skeleton : skeletons)
			{
				string name = skeleton->GetName();
				int clusterNo = 0;
				for (int countCluster = 0; countCluster < animator->Clusters.size(); ++countCluster)
				{
					if (name == animator->Clusters[countCluster].Name)
					{
						clusterNo = countCluster;
						break;
					}
				}
				frame.BoneFrames[clusterNo].Name = name;

				// �A�j���[�V�����s��̎Z�o
				//auto skeletonGlobalCurrentTransform = skeleton->EvaluateGlobalTransform(currentTime);
				//FbxAMatrix lReferenceGlobalCurrentPosition;
				//auto lClusterRelativeCurrentPositionInverse = lReferenceGlobalCurrentPosition.Inverse() * skeletonGlobalCurrentTransform;
				//auto VertexTransformMatrix = lClusterRelativeCurrentPositionInverse * animator->Clusters[clusterNo].RelativeInitPosition;
				frame.BoneFrames[clusterNo].Matrix = CKFMtx44::FbxToMtx(skeleton->EvaluateLocalTransform(currentTime));
			}
			motion.Frames.push_back(frame);
		}
		motion.Frames.shrink_to_fit();
		motion.EndFrame = motion.Frames.size() - 1;
		animator->Motions.push_back(motion);
	}
	animator->Motions.shrink_to_fit();
}

//--------------------------------------------------------------------------------
//  analyzePose
//--------------------------------------------------------------------------------
void CKFUtilityFBX::analyzePose(FbxScene* lScene)
{
	int nNumPose = lScene->GetPoseCount();
	for (int nCnt = 0; nCnt < nNumPose; ++nCnt)
	{
		auto pPose = lScene->GetPose(nCnt);
		int nNumCnt = pPose->GetCount();
		list<string> listName;
		list<FbxMatrix> listMtx;
		for (int nCntNode = 0; nCntNode < nNumCnt; ++nCntNode)
		{
			string Name = pPose->GetNode(nCntNode)->GetName();
			auto mtx = pPose->GetMatrix(nCntNode);
			listName.push_back(Name);
			listMtx.push_back(mtx);
		}

		int n = 0;
	}
}

//--------------------------------------------------------------------------------
//  findSkeletons
//--------------------------------------------------------------------------------
void CKFUtilityFBX::findSkeletons(FbxNode* pNode, list<FbxNode*>& listSkeleton)
{
	if (!pNode) return;
	for (int nCnt = 0; nCnt < pNode->GetNodeAttributeCount(); nCnt++)
	{
		auto type = pNode->GetNodeAttributeByIndex(nCnt)->GetAttributeType();
		if (type == FbxNodeAttribute::eSkeleton)
		{
			listSkeleton.push_back(pNode);
		}
	}

	for (int nCnt = 0; nCnt < pNode->GetChildCount(); ++nCnt)
	{
		findSkeletons(pNode->GetChild(nCnt), listSkeleton);
	}
}

//--------------------------------------------------------------------------------
//  findSkeletons
//--------------------------------------------------------------------------------
void CKFUtilityFBX::matchClusterWithSkeleton(vector<Cluster>& clusters, CMyNode* node)
{
	if (!node) return;
	for (auto& cluster : clusters)
	{
		if (!cluster.Node && cluster.Name == node->Name)
		{
			cluster.Node = node;
			break;
		}
	}
	for (auto child : node->listChild)
	{
		matchClusterWithSkeleton(clusters, child);
	}
}

//--------------------------------------------------------------------------------
//  recursiveSaveNode
//--------------------------------------------------------------------------------
void CKFUtilityFBX::recursiveSaveNode(FILE* pFile, CMyNode* pNode, const string& strFileName)
{
	//Node��
	int nSize = (int)pNode->Name.size();
	fwrite(&nSize, sizeof(int), 1, pFile);
	fwrite(&pNode->Name[0], sizeof(char), nSize, pFile);

	//Offset
	fwrite(&pNode->vTrans, sizeof(CKFVec3), 1, pFile);
	fwrite(&pNode->vRot, sizeof(CKFVec3), 1, pFile);
	fwrite(&pNode->vScale, sizeof(CKFVec3), 1, pFile);
	
	//Collider
	int nNumCollider = (int)pNode->listCollider.size();
	fwrite(&nNumCollider, sizeof(int), 1, pFile);
	for (auto& colInfo : pNode->listCollider)
	{
		fwrite(&colInfo.colType, sizeof(int), 1, pFile);
		fwrite(&colInfo.vOffsetPos, sizeof(CKFVec3), 1, pFile);
		fwrite(&colInfo.vOffsetRot, sizeof(CKFVec3), 1, pFile);
		fwrite(&colInfo.vOffsetScale, sizeof(CKFVec3), 1, pFile);
	}

	//Texture
	int nNumTexture = (int)pNode->vecTex.size();
	fwrite(&nNumTexture, sizeof(int), 1, pFile);
	for (auto& texture : pNode->vecTex)
	{
		nSize = (int)texture.Name.size();
		fwrite(&nSize, sizeof(int), 1, pFile);
		fwrite(&texture.Name[0], sizeof(char), nSize, pFile);
	}

	//Mesh
	int nNumMesh = (int)pNode->vecMesh.size();
	fwrite(&nNumMesh, sizeof(int), 1, pFile);
	for (int nCnt = 0; nCnt < nNumMesh; ++nCnt)
	{
		auto& mesh = pNode->vecMesh[nCnt];
		//if (mesh.vecMtx.empty())
		{//���Ȃ�
			//Name
			string strMeshName = strFileName + '_' + pNode->Name + '_' + to_string(nCnt) + ".mesh";
			nSize = (int)strMeshName.size();
			fwrite(&nSize, sizeof(int), 1, pFile);
			fwrite(&strMeshName[0], sizeof(char), nSize, pFile);

			//Mesh
			saveMesh(pNode, mesh, strMeshName);
		}
		//else
		//{//�����X�L�[�����b�V��
		//	//Name
		//	string strMeshName = pNode->Name + '_' + to_string(nCnt) + ".oneSkinMesh";
		//	nSize = (int)strMeshName.size();
		//	fwrite(&nSize, sizeof(int), 1, pFile);
		//	fwrite(&strMeshName[0], sizeof(char), nSize, pFile);

		//	//Mesh
		//	saveOneSkinMesh(pNode, mesh, strMeshName);
		//}
	}
	
	//Child
	int nNumChild = (int)pNode->listChild.size();
	fwrite(&nNumChild, sizeof(int), 1, pFile);
	for (auto& pChild : pNode->listChild)
	{
		recursiveSaveNode(pFile, pChild, strFileName);
	}
}

//--------------------------------------------------------------------------------
//  saveMesh
//--------------------------------------------------------------------------------
void CKFUtilityFBX::saveMesh(const CMyNode* pNode, const Mesh& mesh, const string& strMeshName)
{
	string Name = "data/MESH/" + strMeshName;
	FILE *pFile;
	
	//file open
	fopen_s(&pFile, Name.c_str(), "wb");
	
	//DrawType
	int nDrawType = (int)D3DPT_TRIANGLELIST;
	fwrite(&nDrawType, sizeof(int), 1, pFile);
	
	//NumVtx
	fwrite(&mesh.m_nNumVtx, sizeof(int), 1, pFile);
	
	//NumIdx
	fwrite(&mesh.m_nNumIdx, sizeof(int), 1, pFile);
	
	//NumPolygon
	fwrite(&mesh.m_nNumPolygon, sizeof(int), 1, pFile);
	
	//Vtx
	vector<VERTEX_3D> vecVtx;
	vecVtx.resize(mesh.m_nNumVtx);
	for (int nCnt = 0; nCnt < mesh.m_nNumVtx; ++nCnt)
	{
		vecVtx[nCnt] = mesh.m_vecVtx[nCnt].vtx;
	}
	fwrite(&vecVtx[0], sizeof(VERTEX_3D), mesh.m_nNumVtx, pFile);
	vecVtx.clear();

	//Idx
	WORD *pIdx;
	mesh.m_pIdxBuffer->Lock(0, 0, (void**)&pIdx, 0);
	fwrite(pIdx, sizeof(WORD), mesh.m_nNumIdx, pFile);
	mesh.m_pIdxBuffer->Unlock();

	//Texture
	if (!mesh.strTexName.empty())
	{
		//auto& texture = pNode->vecTex[mesh.nMaterialIndex];
		int nSize = (int)mesh.strTexName.size();
		fwrite(&nSize, sizeof(int), 1, pFile);
		fwrite(&mesh.strTexName[0], sizeof(char), nSize, pFile);
	}
	else
	{
		int nSize = 0;
		fwrite(&nSize, sizeof(int), 1, pFile);
	}

	//Render Priority
	fwrite(&mesh.m_renderPriority, sizeof(RENDER_PRIORITY), 1, pFile);
	
	//Render State
	RENDER_STATE rs;
	if (mesh.m_bEnableLight)
	{
		if (mesh.m_bEnableCullFace) { rs = RS_LIGHTON_CULLFACEON_MUL; }
		else { rs = RS_LIGHTON_CULLFACEOFF_MUL; }
	}
	else
	{
		if (mesh.m_bEnableCullFace) { rs = RS_LIGHTOFF_CULLFACEON_MUL; }
		else { rs = RS_LIGHTOFF_CULLFACEOFF_MUL; }
	}
	fwrite(&rs, sizeof(RENDER_STATE), 1, pFile);
	
	fclose(pFile);
}

//--------------------------------------------------------------------------------
//  saveMesh
//--------------------------------------------------------------------------------
void CKFUtilityFBX::saveOneSkinMesh(const CMyNode* pNode, const Mesh& mesh, const string& strMeshName)
{
	MessageBox(NULL, "���Ή�", "saveOneSkinMesh", MB_OK | MB_ICONWARNING);

	//string Name = "data/MESH/" + strMeshName;
	//FILE *pFile;

	////file open
	//fopen_s(&pFile, Name.c_str(), "wb");

	////DrawType
	//int nDrawType = (int)D3DPT_TRIANGLELIST;
	//fwrite(&nDrawType, sizeof(int), 1, pFile);

	////NumVtx
	//fwrite(&mesh.m_nNumVtx, sizeof(int), 1, pFile);

	////NumIdx
	//fwrite(&mesh.m_nNumIdx, sizeof(int), 1, pFile);

	////NumPolygon
	//fwrite(&mesh.m_nNumPolygon, sizeof(int), 1, pFile);

	////Vtx
	//for (int nCnt = 0; nCnt < mesh.m_nNumVtx; ++nCnt)
	//{
	//	auto& vtxDX = mesh.m_vecVtx[nCnt];

	//	//Vtx
	//	fwrite(&vtxDX.vtx, sizeof(VERTEX_3D), 1, pFile);

	//	//MtxIdx
	//	int nSize = (int)vtxDX.vecBornRefarence.size();
	//	fwrite(&nSize, sizeof(int), 1, pFile);
	//	fwrite(&vtxDX.vecBornRefarence[0], sizeof(BornRefarence), nSize, pFile);
	//	
	//}

	////Idx
	//WORD *pIdx;
	//mesh.m_pIdxBuffer->Lock(0, 0, (void**)&pIdx, 0);
	//fwrite(pIdx, sizeof(WORD), mesh.m_nNumIdx, pFile);
	//mesh.m_pIdxBuffer->Unlock();

	////Texture
	//auto& texture = pNode->vecTex[mesh.nMaterialIndex];
	//int nSize = (int)texture.Name.size();
	//fwrite(&nSize, sizeof(int), 1, pFile);
	//fwrite(&texture.Name[0], sizeof(char), nSize, pFile);

	////Mtx
	//

	//fclose(pFile);
}

//--------------------------------------------------------------------------------
//  getGeometry
//	Get the geometry offset to a node. It is never inherited by the children
//--------------------------------------------------------------------------------
FbxAMatrix CKFUtilityFBX::getGeometry(FbxNode* pNode)
{
	const FbxVector4 lT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 lR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 lS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);
	return FbxAMatrix(lT, lR, lS);
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