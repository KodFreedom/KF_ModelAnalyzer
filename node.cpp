//--------------------------------------------------------------------------------
//
//�@node.cpp
//	Author : Xu Wenjie
//	Date   : 2017-09-15
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  �C���N���[�h�t�@�C��
//--------------------------------------------------------------------------------
#include "KF_UtilityFBX.h"
#include "node.h"
#include "manager.h"
#include "textureManager.h"
#include "rendererDX.h"
#include "materialManager.h"


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
//  VertexDX
//--------------------------------------------------------------------------------
bool VertexDX::operator==(const VertexDX& value) const
{
	if (Vertex.vPos == value.Vertex.vPos
		&& Vertex.vNormal == value.Vertex.vNormal
		&& Vertex.vUV == value.Vertex.vUV
		&& Vertex.ulColor == value.Vertex.ulColor
		&& BoneReferences.size() == value.BoneReferences.size())
	{
		for (int count = 0; count < (int)BoneReferences.size(); ++count)
		{
			if (BoneReferences[count].Weight != value.BoneReferences[count].Weight
				|| BoneReferences[count].Index != value.BoneReferences[count].Index)
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
//
//  MyNode
//
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//
//  Public
//
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  Release
//--------------------------------------------------------------------------------
void CMyNode::Release(void)
{
	//Mesh
	for (auto& mesh : Meshes)
	{
		//mesh.vecMtx.clear();
		mesh.Normals.clear();
		mesh.NormalIndeces.clear();
		for (auto& point : mesh.Points)
		{
			point.BoneReferences.clear();
		}
		mesh.Points.clear();
		mesh.PointIndeces.clear();
		for (auto& UVSet : mesh.UVSets)
		{
			UVSet.Name.clear();
			UVSet.UVs.clear();
			UVSet.UVIndeces.clear();
		}
		mesh.UVSets.clear();

#ifdef USING_DIRECTX
		for (auto& vertex : mesh.Verteces)
		{
			vertex.BoneReferences.clear();
		}
		SAFE_RELEASE(mesh.VertexBuffer);
		SAFE_RELEASE(mesh.IndexBuffer);
		SAFE_RELEASE(s_pMeshCube);
		SAFE_RELEASE(s_pMeshSphere);
#endif
	}
	Meshes.clear();

	//Collider
	Colliders.clear();

	//Child
	for (auto pChild : Children)
	{
		pChild->Release();
	}
	Children.clear();

	//Other
	Name.clear();
	AttributeNames.clear();
	delete this;
}

//--------------------------------------------------------------------------------
//  RecursiveUpdateMatrix
//--------------------------------------------------------------------------------
void CMyNode::RecursiveUpdateMatrix(const CKFMtx44& parent)
{
	CKFMath::MtxIdentity(World);
	World.m_af[0][0] = Scale.m_fX;
	World.m_af[1][1] = Scale.m_fY;
	World.m_af[2][2] = Scale.m_fZ;
	auto& rotation = Rotation * CKFMath::EulerToQuaternion(RotationOffset);
	World *= CKFMath::QuaternionToMtx(rotation);
	CKFMtx44 mtxPos;
	CKFMath::MtxTranslation(mtxPos, Translation);
	World *= mtxPos;
	World *= parent;
	for (auto pChild : Children)
	{
		pChild->RecursiveUpdateMatrix(World);
	}
}

//--------------------------------------------------------------------------------
//  RecursiveUpdateSkin
//--------------------------------------------------------------------------------
void CMyNode::RecursiveUpdateSkin(const vector<Cluster>& clusters)
{
	for (auto& mesh : Meshes)
	{
#ifdef USING_DIRECTX
		// ������i�܂胏���X�L���Ȃǁj
		// ���_�̍��W�ϊ� 
		VERTEX_3D* pVtx;
		mesh.VertexBuffer->Lock(0, 0, (void**)&pVtx, 0);

//#pragma omp parallel for 
		for (int count = 0; count < (int)mesh.Verteces.size(); ++count)
		{
			CKFMtx44 mtx;
			auto& vertexDX = mesh.Verteces[count];
			if (!vertexDX.BoneReferences.empty())
			{
				mesh.IsSkin = true;
				ZeroMemory(&mtx, sizeof(CKFMtx44));
				for (auto& boneReference : vertexDX.BoneReferences)
				{
					mtx += clusters[boneReference.Index].World * boneReference.Weight;
				}
			}
			pVtx[count].vPos = CKFMath::Vec3TransformCoord(vertexDX.Vertex.vPos, mtx);
			pVtx[count].vNormal = CKFMath::Vec3TransformNormal(vertexDX.Vertex.vNormal, mtx);
		}
		mesh.VertexBuffer->Unlock();
#endif
	}

	//Child
	for (auto pChild : Children)
	{
		pChild->RecursiveUpdateSkin(clusters);
	}
}

//--------------------------------------------------------------------------------
//  RecursiveMatchClusterID
//--------------------------------------------------------------------------------
void CMyNode::RecursiveMatchClusterID(const vector<Cluster>& avatar)
{
	for (auto& mesh : Meshes)
	{
#pragma omp parallel for 
		for (int count = 0; count < (int)mesh.Verteces.size(); ++count)
		{
			auto& vertexDX = mesh.Verteces[count];
			for (auto& boneReference : vertexDX.BoneReferences)
			{
				for (int countBone = 0; countBone < (int)avatar.size(); ++countBone)
				{
					if (boneReference.Name == avatar[countBone].Name)
					{
						boneReference.Index = countBone;
						break;
					}
				}
			}
		}
	}

	//Child
	for (auto pChild : Children)
	{
		pChild->RecursiveMatchClusterID(avatar);
	}
}

//--------------------------------------------------------------------------------
//  RecursiveDraw
//--------------------------------------------------------------------------------
void CMyNode::RecursiveDraw(const unordered_map<string, Material>& mapMaterial, const bool& drawSkeleton, const bool& drawMesh, const bool& drawCollider)
{
#ifdef USING_DIRECTX
	auto pDevice = CMain::GetManager()->GetRenderer()->GetDevice();

	//Mesh
	if (drawMesh)
	{
		for (auto& mesh : Meshes)
		{
			if (mesh.MyRenderPriority == RP_AlphaTest)
			{
				pDevice->SetRenderState(D3DRS_ALPHAREF, (DWORD)0x00000001);
				pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
				pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
			}

			CRenderState* renderState = nullptr;
			if (mesh.EnableLight)
			{
				if (mesh.EnableCullFace) { renderState = &CDrawComponent::s_lightOnCullOn; }
				else { renderState = &CDrawComponent::s_lightOnCullOff; }
			}
			else
			{
				if (mesh.EnableCullFace) { renderState = &CDrawComponent::s_lightOffCullOn; }
				else { renderState = &CDrawComponent::s_lightOffCullOff; }
			}

			renderState->SetRenderState();

			// Material
			if (mapMaterial.find(mesh.MaterialName) != mapMaterial.end())
			{
				auto& material = mapMaterial.at(mesh.MaterialName);
				auto pTexture = CMain::GetManager()->GetTextureManager()->GetTexture(material.DiffuseTextureName);
				pDevice->SetTexture(0, pTexture);

				// �}�e���A���̐ݒ�
				D3DMATERIAL9 mat;
				mat.Diffuse = material.Diffuse;
				mat.Ambient = material.Ambient;
				mat.Specular = material.Specular;
				mat.Emissive = material.Emissive;
				mat.Power = material.Power;
				pDevice->SetMaterial(&mat);
			}

			D3DXMATRIX mtx;
			if (mesh.IsSkin)
			{
				D3DXMatrixIdentity(&mtx);
			}
			else
			{
				mtx = World;
			}
			pDevice->SetTransform(D3DTS_WORLD, &mtx);

			// ���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
			pDevice->SetStreamSource(
				0,						//�X�g���[���ԍ�
				mesh.VertexBuffer,		//���_�o�b�t�@
				0,						//�I�t�Z�b�g�i�J�n�ʒu�j
				sizeof(VERTEX_3D));		//�X�g���C�h��

										// ���_�C���f�b�N�X�̐ݒ�
			pDevice->SetIndices(mesh.IndexBuffer);

			// ���_�t�H�[�}�b�g�̐ݒ�
			pDevice->SetFVF(FVF_VERTEX_3D);

			//�v���~�e�B�u�`��
			pDevice->DrawIndexedPrimitive(
				D3DPT_TRIANGLELIST,
				0,
				0,
				mesh.VertexNumber,
				0,
				mesh.PolygonNumber);

			renderState->ResetRenderState();

			if (mesh.MyRenderPriority == RP_AlphaTest)
			{
				pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
			}
		}
	}

	if (!s_pMeshSphere)
	{
		CMain::GetManager()->GetTextureManager()->UseTexture("polygon.png");
		D3DXCreateSphere(pDevice, 1.0f, 10, 10, &s_pMeshSphere, nullptr);
	}
	if (!s_pMeshCube) { D3DXCreateBox(pDevice, 1.0f, 1.0f, 1.0f, &s_pMeshCube, nullptr); }
	auto pTexture = CMain::GetManager()->GetTextureManager()->GetTexture("polygon.png");
	D3DMATERIAL9 material = CMain::GetManager()->GetMaterialManager()->GetMaterial(ColliderMaterialID);
	D3DMATERIAL9 matDef;
	pDevice->GetMaterial(&matDef);
	pDevice->SetTexture(0, pTexture);
	pDevice->SetMaterial(&material);

	// Skeleton
	if (drawSkeleton)
	{
		if (!AttributeNames.empty() && AttributeNames[0] == "skeleton")
		{
			D3DXMATRIX mtx = World;
			pDevice->SetTransform(D3DTS_WORLD, &mtx);
			s_pMeshSphere->DrawSubset(0);
		}
	}

	//Draw Collider
	if (drawCollider)
	{
		for (auto& col : Colliders)
		{
			CKFMtx44 mtxCol, mtxRot, mtxPos;

			//�g�k
			mtxCol.m_af[0][0] = col.Scale.m_fX;
			mtxCol.m_af[1][1] = col.Scale.m_fY;
			mtxCol.m_af[2][2] = col.Scale.m_fZ;

			//��]
			CKFMath::MtxRotationYawPitchRoll(mtxRot, col.Rotation);
			mtxCol *= mtxRot;

			//���s�ړ�
			CKFMath::MtxTranslation(mtxPos, col.Position);
			mtxCol *= mtxPos;
			mtxCol *= World;

			if (col.Type == CS::COL_SPHERE)
			{
				D3DXMATRIX mtx = mtxCol;
				pDevice->SetTransform(D3DTS_WORLD, &mtx);
				s_pMeshSphere->DrawSubset(0);
			}
			else if (col.Type == CS::COL_AABB)
			{
				mtxCol.m_af[0][0] = CKFMath::VecMagnitude(CKFVec3(mtxCol.m_af[0][0], mtxCol.m_af[0][1], mtxCol.m_af[0][2]));
				mtxCol.m_af[1][1] = CKFMath::VecMagnitude(CKFVec3(mtxCol.m_af[1][0], mtxCol.m_af[1][1], mtxCol.m_af[1][2]));
				mtxCol.m_af[2][2] = CKFMath::VecMagnitude(CKFVec3(mtxCol.m_af[2][0], mtxCol.m_af[2][1], mtxCol.m_af[2][2]));
				mtxCol.m_af[0][1] = mtxCol.m_af[0][2]
					= mtxCol.m_af[1][0] = mtxCol.m_af[1][2]
					= mtxCol.m_af[2][0] = mtxCol.m_af[2][1] = 0.0f;
				D3DXMATRIX mtx = mtxCol;
				pDevice->SetTransform(D3DTS_WORLD, &mtx);
				s_pMeshCube->DrawSubset(0);
			}
			else if (col.Type == CS::COL_OBB)
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
	for (auto pNode : Children)
	{
		pNode->RecursiveDraw(mapMaterial, drawSkeleton, drawMesh, drawCollider);
	}
#else
	glPushMatrix();
	glScalef(vScale.m_fX, vScale.m_fY, vScale.m_fZ);
	glRotatef(vRot.m_fZ, 0.0f, 0.0f, 1.0f);
	glRotatef(vRot.m_fX, 1.0f, 0.0f, 0.0f);
	glRotatef(vRot.m_fY, 0.0f, 1.0f, 0.0f);
	glTranslatef(vTrans.m_fX, vTrans.m_fY, vTrans.m_fZ);

	//Mesh
	for (auto& mesh : Meshes)
	{
		//Texture
		if (!Textures.empty())
		{
			glBindTexture(GL_TEXTURE_2D, Textures[mesh.nMaterialIndex].nID);
		}

		// �|���S���`��         
		glBegin(GL_TRIANGLES);

		if (mesh.vecMtx.empty())
		{// ���Ȃ��i�܂荄�̂̉�j             
			for (int count = 0; count < (int)mesh.PointIndeces.size(); ++count)
			{
				glColor4f(255.0f, 255.0f, 255.0f, 255.0f);
				glNormal3f(
					mesh.Normals[mesh.NormalIndeces[count]].m_fX,
					mesh.Normals[mesh.NormalIndeces[count]].m_fY,
					mesh.Normals[mesh.NormalIndeces[count]].m_fZ);
				glTexCoord2f(
					mesh.UVSets[0].UVs[mesh.UVSets[0].UVIndeces[count]].m_fX,
					mesh.UVSets[0].UVs[mesh.UVSets[0].UVIndeces[count]].m_fY);
				glVertex3f(
					mesh.Points[mesh.PointIndeces[count]].vPos.m_fX,
					mesh.Points[mesh.PointIndeces[count]].vPos.m_fY,
					mesh.Points[mesh.PointIndeces[count]].vPos.m_fZ);
			}
		}
		else
		{// ������i�܂胏���X�L���Ȃǁj 
		 // ���_�̍��W�ϊ� 
			vector<CKFVec3> vecPos;
			vecPos.reserve(mesh.Points.size());
			CKFMtx44 mtx;
			for (auto& point : mesh.Points)
			{
				ZeroMemory(&mtx, sizeof(CKFMtx44));
				for (auto& boneReference : point.BoneReferences)
				{
					mtx += mesh.vecMtx[boneReference.Index] * boneReference.Weight;
				}
				CKFVec3 vPos = point.vPos;
				CKFMath::Vec3TransformCoord(&vPos, mtx);
				vecPos.push_back(vPos);
			}

			for (int count = 0; count < (int)mesh.PointIndeces.size(); ++count)
			{
				glColor4f(255.0f, 255.0f, 255.0f, 255.0f);
				glNormal3f(
					mesh.Normals[mesh.NormalIndeces[count]].m_fX,
					mesh.Normals[mesh.NormalIndeces[count]].m_fY,
					mesh.Normals[mesh.NormalIndeces[count]].m_fZ);
				glTexCoord2f(
					mesh.UVSets[0].UVs[mesh.UVSets[0].UVIndeces[count]].m_fX,
					mesh.UVSets[0].UVs[mesh.UVSets[0].UVIndeces[count]].m_fY);
				glVertex3f(
					vecPos[mesh.PointIndeces[count]].m_fX,
					vecPos[mesh.PointIndeces[count]].m_fY,
					vecPos[mesh.PointIndeces[count]].m_fZ);
			}
		}

		glEnd();
	}

	//Child
	for (auto pNode : Children)
	{
		pNode->RecursiveDraw(bDrawNormal);
	}
	glPopMatrix();
#endif
}

//--------------------------------------------------------------------------------
//  RecursiveRecombineMeshes
//--------------------------------------------------------------------------------
void CMyNode::RecursiveRecombineMeshes(void)
{
#ifdef USING_DIRECTX
	for (auto& mesh : Meshes)
	{
		list<VertexDX> listVtx;
		list<int> listIdx;

		for (int count = 0; count < (int)mesh.PointIndeces.size(); ++count)
		{
			VertexDX vertexDX;
			auto& point = mesh.Points[mesh.PointIndeces[count]];
			vertexDX.Vertex.ulColor = CKFMath::sc_cWhite;
			vertexDX.Vertex.vNormal = mesh.Normals[mesh.NormalIndeces[count]];
			vertexDX.Vertex.vUV = mesh.UVSets[0].UVs[mesh.UVSets[0].UVIndeces[count]];
			vertexDX.Vertex.vPos = point.Position;
			vertexDX.BoneReferences.assign(point.BoneReferences.begin(), point.BoneReferences.end());

			int nIdx = CKFUtilityFBX::FindRepetition(listVtx, vertexDX);
			if (nIdx >= 0) { listIdx.push_back(nIdx); }
			else
			{
				listIdx.push_back((int)listVtx.size());
				listVtx.push_back(vertexDX);
			}
		}

		mesh.VertexNumber = (int)listVtx.size();
		mesh.IndexNumber = (int)listIdx.size();
		mesh.PolygonNumber = mesh.IndexNumber / 3;

		auto pDevice = CMain::GetManager()->GetRenderer()->GetDevice();
		HRESULT hr;

		//���_�o�b�t�@
		hr = pDevice->CreateVertexBuffer(
			sizeof(VERTEX_3D) * mesh.VertexNumber,	//�쐬���������_�o�b�t�@�̃T�C�Y
			D3DUSAGE_WRITEONLY,					//���_�o�b�t�@�̎g�p���@
			FVF_VERTEX_3D,						//�����Ȃ��Ă����v
			D3DPOOL_MANAGED,					//�������Ǘ����@(managed�F�f�o�C�X�ɂ��C��)
			&mesh.VertexBuffer,					//���_�o�b�t�@�̃|�C���^
			NULL);

		if (FAILED(hr))
		{
			MessageBox(NULL, "CMeshManager : CreateVertexBuffer ERROR!!", "�G���[", MB_OK | MB_ICONWARNING);
			continue;
		}

		VERTEX_3D *pVtx;
		mesh.VertexBuffer->Lock(0, 0, (void**)&pVtx, 0);
		mesh.Verteces.reserve(mesh.VertexNumber);
		auto itrVtx = listVtx.begin();
		for (int count = 0; count < mesh.VertexNumber; ++count)
		{
			mesh.Verteces.push_back(*itrVtx);
			pVtx[count] = itrVtx->Vertex;
			++itrVtx;
		}
		mesh.VertexBuffer->Unlock();

		//�C���f�b�N�X�o�b�t�@�̍쐬
		hr = pDevice->CreateIndexBuffer(
			sizeof(WORD) * mesh.IndexNumber,
			D3DUSAGE_WRITEONLY,
			D3DFMT_INDEX16,
			D3DPOOL_MANAGED,
			&mesh.IndexBuffer,
			NULL);

		if (FAILED(hr))
		{
			MessageBox(NULL, "CMeshManager : CreateIndexBuffer ERROR!!", "�G���[", MB_OK | MB_ICONWARNING);
			continue;
		}

		WORD *pIdx;
		mesh.IndexBuffer->Lock(0, 0, (void**)&pIdx, 0);
		auto itrIdx = listIdx.begin();
		for (int count = 0; count < mesh.IndexNumber; ++count)
		{
			pIdx[count] = *itrIdx;
			++itrIdx;
		}
		mesh.IndexBuffer->Unlock();
	}
#endif

	//Child
	for (auto pNode : Children)
	{
		pNode->RecursiveRecombineMeshes();
	}
}

//--------------------------------------------------------------------------------
//  RecursiveReverseTexV
//--------------------------------------------------------------------------------
void CMyNode::RecursiveReverseTexV(void)
{
#ifdef USING_DIRECTX
	for (auto& mesh : Meshes)
	{
		VERTEX_3D *pVtx;
		mesh.VertexBuffer->Lock(0, 0, (void**)&pVtx, 0);
		for (int count = 0; count < mesh.VertexNumber; ++count)
		{
			mesh.Verteces[count].Vertex.vUV.m_fY = 1.0f - mesh.Verteces[count].Vertex.vUV.m_fY;
			pVtx[count].vUV.m_fY = mesh.Verteces[count].Vertex.vUV.m_fY;
		}
		mesh.VertexBuffer->Unlock();
	}
#endif

	//Child
	for (auto pNode : Children)
	{
		pNode->RecursiveReverseTexV();
	}
}

//--------------------------------------------------------------------------------
//  RecursiveReverseTexV
//--------------------------------------------------------------------------------
void CMyNode::RecalculateMeshesBy(const CKFMtx44& matrix)
{
#ifdef USING_DIRECTX
	for (auto& mesh : Meshes)
	{
		VERTEX_3D *pVtx;
		mesh.VertexBuffer->Lock(0, 0, (void**)&pVtx, 0);
		for (int count = 0; count < mesh.VertexNumber; ++count)
		{
			auto& vtx = mesh.Verteces[count].Vertex;
			vtx.vPos = CKFMath::Vec3TransformCoord(vtx.vPos, matrix);
			vtx.vNormal = CKFMath::Vec3TransformNormal(vtx.vNormal, matrix);
			CKFMath::VecNormalize(vtx.vNormal);
			pVtx[count].vPos = vtx.vPos;
			pVtx[count].vNormal = vtx.vNormal;
		}
		mesh.VertexBuffer->Unlock();
	}
#endif

	for (auto pChild : Children)
	{
		pChild->Translation = CKFMath::Vec3TransformCoord(pChild->Translation, matrix);
		pChild->RecalculateMeshesBy(matrix);
	}
}

//--------------------------------------------------------------------------------
//  RecursiveSave(Json)
//--------------------------------------------------------------------------------
void CMyNode::RecursiveSave(JSONOutputArchive& archive, const string& fileName, const bool& haveAnimator)
{
	//Node��
	archive(make_nvp("Name", Name));

	//Offset
	auto& rotation = Rotation * CKFMath::EulerToQuaternion(RotationOffset);
	archive(make_nvp("Translation", Translation)
		, make_nvp("Rotation", rotation)
		, make_nvp("Scale", Scale));

	//Collider
	int colliderNumber = (int)Colliders.size();
	archive(make_nvp("ColliderNumber", colliderNumber));
	for (auto& collider : Colliders)
	{
		archive(make_nvp("Collider", collider));
	}

	//Mesh
	int meshNumber = (int)Meshes.size();
	archive(make_nvp("MeshNumber", meshNumber));
	for (int count = 0; count < meshNumber; ++count)
	{
		auto& mesh = Meshes[count];
		auto& meshName = Name + '_' + to_string(count);
		archive(make_nvp("MeshName", meshName));

		//Material
		archive(make_nvp("MaterialName", mesh.MaterialName));

		//Render Priority
		archive(make_nvp("RenderPriority", mesh.MyRenderPriority));

		//Shader Type
		ShaderType shaderType = ShaderType::kDefaultShader;
		if (!Meshes[count].EnableLight && !Meshes[count].EnableFog && Meshes[count].EnableCullFace) shaderType = ShaderType::kNoLightNoFog;
		else if (Meshes[count].EnableLight && Meshes[count].EnableFog && !Meshes[count].EnableCullFace) shaderType = ShaderType::kCullNone;
		archive(make_nvp("ShaderType", shaderType));

		//Type
		MeshType type = haveAnimator ? k3dSkin : k3dMesh;
		archive(make_nvp("MeshType", type));

		if (haveAnimator) saveSkinMeshJson(mesh, meshName);
		else saveMeshJson(mesh, meshName);
	}

	//Child
	int childNumber = (int)Children.size();
	archive(make_nvp("ChildNumber", childNumber));
	for (auto& pChild : Children)
	{
		pChild->RecursiveSave(archive, fileName, haveAnimator);
	}
}

//--------------------------------------------------------------------------------
//  RecursiveSave(Binary
//--------------------------------------------------------------------------------
void CMyNode::RecursiveSave(BinaryOutputArchive& archive, const string& fileName, const bool& haveAnimator)
{
	//Node��
	int size = (int)Name.size();
	archive.saveBinary(&size, sizeof(int));
	archive.saveBinary(&Name[0], size);

	//Offset
	auto& rotation = Rotation * CKFMath::EulerToQuaternion(RotationOffset);
	archive.saveBinary(&Translation, sizeof(CKFVec3));
	archive.saveBinary(&rotation, sizeof(CKFQuaternion));
	archive.saveBinary(&Scale, sizeof(CKFVec3));

	//Collider
	int colliderNumber = (int)Colliders.size();
	archive.saveBinary(&colliderNumber, sizeof(int));
	for (auto& collider : Colliders)
	{
		archive.saveBinary(&collider.Type, sizeof(collider.Type));
		archive.saveBinary(&collider.Position, sizeof(collider.Position));
		archive.saveBinary(&collider.Rotation, sizeof(collider.Rotation));
		archive.saveBinary(&collider.Scale, sizeof(collider.Scale));
	}

	//Mesh
	int meshNumber = (int)Meshes.size();
	archive.saveBinary(&meshNumber, sizeof(int));
	for (int count = 0; count < meshNumber; ++count)
	{
		// Name
		string& meshName = Name + '_' + to_string(count);
		size = (int)meshName.size();
		archive.saveBinary(&size, sizeof(int));
		archive.saveBinary(&meshName[0], size);

		//Material
		int nameSize = (int)Meshes[count].MaterialName.size();
		archive.saveBinary(&nameSize, sizeof(int));
		archive.saveBinary(&Meshes[count].MaterialName[0], nameSize);

		//Render Priority
		archive.saveBinary(&Meshes[count].MyRenderPriority, sizeof(Meshes[count].MyRenderPriority));

		//Shader Type
		ShaderType shaderType = ShaderType::kDefaultShader;
		if (!Meshes[count].EnableLight && !Meshes[count].EnableFog && Meshes[count].EnableCullFace) shaderType = ShaderType::kNoLightNoFog;
		else if (Meshes[count].EnableLight && Meshes[count].EnableFog && !Meshes[count].EnableCullFace) shaderType = ShaderType::kCullNone;
		archive.saveBinary(&shaderType, sizeof(shaderType));

		//Type
		MeshType type = Meshes[count].IsSkin ? k3dSkin : k3dMesh;
		archive.saveBinary(&type, sizeof(type));

		// MeshInfo
		if (Meshes[count].IsSkin) saveSkinMeshBinary(Meshes[count], meshName);
		else saveMeshBinary(Meshes[count], meshName);
	}

	//Child
	int childNumber = (int)Children.size();
	archive.saveBinary(&childNumber, sizeof(int));
	for (auto& pChild : Children)
	{
		pChild->RecursiveSave(archive, fileName, haveAnimator);
	}
}

//--------------------------------------------------------------------------------
//
//  Private
//
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  analyzePos
//--------------------------------------------------------------------------------
void CMyNode::analyzePoint(FbxMesh* pMesh)
{
	//���_���̎擾
	auto nNumPoint = pMesh->GetControlPointsCount();

	//���_�o�b�t�@�̍쐬
	auto& currentMesh = Meshes.back();
	currentMesh.Points.resize(nNumPoint);

	//���_���擾
	auto pFbxV4 = pMesh->GetControlPoints();
	for (int count = 0; count < nNumPoint; ++count)
	{
		currentMesh.Points[count].Position.m_fX = static_cast<float>(pFbxV4[count][0]);
		currentMesh.Points[count].Position.m_fY = static_cast<float>(pFbxV4[count][1]);
		currentMesh.Points[count].Position.m_fZ = static_cast<float>(pFbxV4[count][2]);
	}

	//�C���f�b�N�X�̎擾
	auto nNumIdx = pMesh->GetPolygonVertexCount();
	currentMesh.PointIndeces.resize(nNumIdx);
	auto pIdx = pMesh->GetPolygonVertices();
	for (int count = 0; count < nNumIdx; ++count)
	{
		currentMesh.PointIndeces[count] = static_cast<unsigned int>(pIdx[count]);
	}
}

//--------------------------------------------------------------------------------
//  analyzeNormal
//--------------------------------------------------------------------------------
void CMyNode::analyzeNormal(FbxMesh* pMesh)
{
	int nLayerCnt = pMesh->GetLayerCount();

	for (int count = 0; count < nLayerCnt; ++count)
	{
		auto pElementNormal = pMesh->GetElementNormal(count);

		if (!pElementNormal) { continue; }

		//�@���f�[�^�̎擾
		auto& currentMesh = Meshes.back();
		auto nNumNormal = pElementNormal->GetDirectArray().GetCount();
		currentMesh.Normals.resize(nNumNormal);
		for (int count = 0; count < nNumNormal; ++count)
		{
			currentMesh.Normals[count].m_fX = (float)pElementNormal->GetDirectArray()[count][0];
			currentMesh.Normals[count].m_fY = (float)pElementNormal->GetDirectArray()[count][1];
			currentMesh.Normals[count].m_fZ = (float)pElementNormal->GetDirectArray()[count][2];
		}

		//�}�b�s���O���[�h�A���t�@�����X���[�h�擾
		auto mappingMode = pElementNormal->GetMappingMode();
		auto referenceMode = pElementNormal->GetReferenceMode();
		if (mappingMode == FbxLayerElement::eByPolygonVertex)
		{//�@���Ǝ��̃C���f�b�N�X���g�p
			if (referenceMode == FbxLayerElement::eDirect)
			{// �C���f�b�N�X�Q�Ƃ̕K�v�Ȃ�
				currentMesh.NormalIndeces.resize(nNumNormal);
				for (int count = 0; count < nNumNormal; ++count)
				{
					currentMesh.NormalIndeces[count] = count;
				}
			}
			else if (referenceMode == FbxLayerElement::eIndexToDirect
				|| referenceMode == FbxLayerElement::eIndex)
			{// �Ǝ��C���f�b�N�X������ 
			 // �C���f�b�N�X���̎擾                 
				int nNumNormalIdx = pElementNormal->GetIndexArray().GetCount();

				// �@���C���f�b�N�X�i�[�p�R���e�i�̗̈�\��
				currentMesh.NormalIndeces.resize(nNumNormalIdx);

				// �@���C���f�b�N�X�̎擾  
				for (int count = 0; count < nNumNormalIdx; ++count)
				{
					currentMesh.NormalIndeces[count] = pElementNormal->GetIndexArray()[count];
				}
			}
		}

		else if (mappingMode == FbxLayerElement::eByControlPoint)
		{//���_�o�b�t�@�Ɠ����C���f�b�N�X
			if (referenceMode == FbxLayerElement::eDirect)
			{
				currentMesh.NormalIndeces.resize(currentMesh.PointIndeces.size());
				currentMesh.NormalIndeces.assign(currentMesh.PointIndeces.begin(), currentMesh.PointIndeces.end());
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
	auto& currentMesh = Meshes.back();
	currentMesh.UVSets.resize(nUVLayerCnt);

	for (int count = 0; count < nUVLayerCnt; ++count)
	{
		auto& UVSet = currentMesh.UVSets[count];

		auto pElementUV = pMesh->GetLayer(count)->GetUVs();

		if (!pElementUV) { continue; }

		//UVSetName
		UVSet.Name = pElementUV->GetName();

		//�}�b�s���O���[�h�A���t�@�����X���[�h�擾
		auto mappingMode = pElementUV->GetMappingMode();
		auto referenceMode = pElementUV->GetReferenceMode();

		if (mappingMode == FbxLayerElement::eByPolygonVertex)
		{
			if (referenceMode == FbxLayerElement::eDirect)
			{
				int nNumUV = pElementUV->GetDirectArray().GetCount();
				UVSet.UVs.resize(nNumUV);
				UVSet.UVIndeces.resize(nNumUV);

				for (int count = 0; count < nNumUV; ++count)
				{
					UVSet.UVs[count].m_fX = static_cast<float>(pElementUV->GetDirectArray()[count][0]);
					UVSet.UVs[count].m_fY = static_cast<float>(pElementUV->GetDirectArray()[count][1]);
					UVSet.UVIndeces[count] = count;
				}
			}

			else if (referenceMode == FbxLayerElement::eIndexToDirect
				|| referenceMode == FbxLayerElement::eIndex)
			{
				//Idx
				int nNumUVIdx = pElementUV->GetIndexArray().GetCount();
				UVSet.UVIndeces.resize(nNumUVIdx);
				for (int count = 0; count < nNumUVIdx; ++count)
				{
					UVSet.UVIndeces[count] = pElementUV->GetIndexArray()[count];
				}

				//UV
				int nNumUV = pElementUV->GetDirectArray().GetCount();
				UVSet.UVs.resize(nNumUV);
				for (int count = 0; count < nNumUV; ++count)
				{
					UVSet.UVs[count].m_fX = static_cast<float>(pElementUV->GetDirectArray()[count][0]);
					UVSet.UVs[count].m_fY = static_cast<float>(pElementUV->GetDirectArray()[count][1]);
				}
			}
		}
		else
		{
			MessageBox(NULL, "�}�b�s���O���[�h��Ή�", "analyzeUV", MB_OK);
		}
	}
}

//--------------------------------------------------------------------------------
//  analyzeMaterial
//--------------------------------------------------------------------------------
void CMyNode::analyzeMaterial(FbxMesh* pMesh)
{
	auto& currentMesh = Meshes.back();
	int nNumLayer = pMesh->GetLayerCount();

	for (int count = 0; count < nNumLayer; ++count)
	{
		auto pElementMaterial = pMesh->GetLayer(count)->GetMaterials();
		if (!pElementMaterial) { continue; }

		int nNumMaterialIdx = pElementMaterial->GetIndexArray().GetCount();
		if (nNumMaterialIdx == 0)
		{
			continue;
		}

		auto mappingMode = pElementMaterial->GetMappingMode();
		auto referenceMode = pElementMaterial->GetReferenceMode();

		if (mappingMode == FbxLayerElement::eAllSame)
		{
			if (referenceMode == FbxLayerElement::eIndexToDirect)
			{// ���b�V���S�������̃}�e���A���C���f�b�N�X 
				auto index = pElementMaterial->GetIndexArray()[0];
				currentMesh.MaterialName = pMesh->GetNode()->GetMaterial(index)->GetName();
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
			auto index = pElementMaterial->GetIndexArray()[0];
			currentMesh.MaterialName = pMesh->GetNode()->GetMaterial(index)->GetName();
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
	auto& currentMesh = Meshes.back();

	// �X�L���̐����擾 
	int nNumSkin = pMesh->GetDeformerCount(FbxDeformer::eSkin);

	for (int countSkin = 0; countSkin < nNumSkin; ++countSkin)
	{
		// �X�L�����擾 
		auto pSkin = (FbxSkin*)pMesh->GetDeformer(countSkin, FbxDeformer::eSkin);

		// �N���X�^�[�̐����擾 
		int nNumCluster = pSkin->GetClusterCount();

		for (int countCluster = 0; countCluster < nNumCluster; ++countCluster)
		{
			// �N���X�^���擾
			auto pCluster = pSkin->GetCluster(countCluster);
			string strClusterName = pCluster->GetLink()->GetName();

			// ���̃N���X�^���e�����y�ڂ����_�C���f�b�N�X�̌����擾 
			int nNumPointIdx = pCluster->GetControlPointIndicesCount();
			auto pPointIndexArray = pCluster->GetControlPointIndices();
			auto pWeightArray = pCluster->GetControlPointWeights();
			for (int count = 0; count < nNumPointIdx; ++count)
			{
				currentMesh.Points[pPointIndexArray[count]].BoneReferences.push_back(
					BoneReference(countCluster, static_cast<float>(pWeightArray[count]), strClusterName));
			}
		}
	}
}

//--------------------------------------------------------------------------------
//  saveMeshJson
//--------------------------------------------------------------------------------
void CMyNode::saveMeshJson(const Mesh& mesh, const string& meshName)
{
	auto& filePath = "data/mesh/" + meshName + ".json";
	ofstream file(filePath);
	if (!file.is_open()) return;
	JSONOutputArchive archive(file);

	//DrawType
	int drawType = (int)TriangleList;
	archive(make_nvp("DrawType", drawType));

	//NumVtx,Idx,Polygon
	archive(make_nvp("VertexNumber", mesh.VertexNumber));
	archive(make_nvp("IndexNumber", mesh.IndexNumber));
	archive(make_nvp("PolygonNumber", mesh.PolygonNumber));

	//Vtx
	vector<VertexOutNoSkin> verteces;
	verteces.reserve(mesh.VertexNumber);
	for (int count = 0; count < mesh.VertexNumber; ++count)
	{
		verteces.push_back((VertexOutNoSkin)mesh.Verteces[count]);
	}
	archive(make_nvp("Verteces", verteces));
	verteces.clear();

	//Idx
	vector<WORD> indeces;
	indeces.reserve(mesh.IndexNumber);
	WORD *pIdx;
	mesh.IndexBuffer->Lock(0, 0, (void**)&pIdx, 0);
	for (int count = 0; count < mesh.IndexNumber; ++count)
	{
		indeces.push_back(pIdx[count]);
	}
	mesh.IndexBuffer->Unlock();
	archive(make_nvp("Indeces", indeces));
	indeces.clear();
	file.close();
}

//--------------------------------------------------------------------------------
//  saveSkinMeshJson
//--------------------------------------------------------------------------------
void CMyNode::saveSkinMeshJson(const Mesh& mesh, const string& meshName)
{
	auto& filePath = "data/skin/" + meshName + ".json";
	ofstream file(filePath);
	if (!file.is_open()) return;
	JSONOutputArchive archive(file);

	//DrawType
	int drawType = (int)TriangleList;
	archive(make_nvp("DrawType", drawType));

	//NumVtx,Idx,Polygon
	archive(make_nvp("VertexNumber", mesh.VertexNumber));
	archive(make_nvp("IndexNumber", mesh.IndexNumber));
	archive(make_nvp("PolygonNumber", mesh.PolygonNumber));

	//Vtx
	vector<VertexOutSkin> verteces;
	verteces.reserve(mesh.VertexNumber);
	for (int count = 0; count < mesh.VertexNumber; ++count)
	{
		verteces.push_back((VertexOutSkin)mesh.Verteces[count]);
	}
	archive(make_nvp("SkinVerteces", verteces));
	verteces.clear();

	//Idx
	vector<WORD> indeces;
	indeces.reserve(mesh.IndexNumber);
	WORD *pIdx;
	mesh.IndexBuffer->Lock(0, 0, (void**)&pIdx, 0);
	for (int count = 0; count < mesh.IndexNumber; ++count)
	{
		indeces.push_back(pIdx[count]);
	}
	mesh.IndexBuffer->Unlock();
	archive(make_nvp("Indeces", indeces));
	indeces.clear();
	file.close();
}

//--------------------------------------------------------------------------------
//  saveMeshBinary
//--------------------------------------------------------------------------------
void CMyNode::saveMeshBinary(const Mesh& mesh, const string& meshName)
{
	auto& filePath = "data/mesh/" + meshName + ".mesh";
	ofstream file(filePath, ios::binary);
	if (!file.is_open()) return;
	BinaryOutputArchive archive(file);

	//DrawType
	auto drawType = TriangleList;
	archive.saveBinary(&drawType, sizeof(drawType));

	//NumVtx,Idx,Polygon
	archive.saveBinary(&mesh.VertexNumber, sizeof(int));
	archive.saveBinary(&mesh.IndexNumber, sizeof(int));
	archive.saveBinary(&mesh.PolygonNumber, sizeof(int));

	//Vtx
	vector<VertexOutNoSkin> verteces;
	verteces.reserve(mesh.VertexNumber);
	for (int count = 0; count < mesh.VertexNumber; ++count)
	{
		verteces.push_back((VertexOutNoSkin)mesh.Verteces[count]);
	}
	archive.saveBinary(&verteces[0], sizeof(VertexOutNoSkin) * mesh.VertexNumber);
	verteces.clear();

	//Idx
	WORD *pIdx;
	mesh.IndexBuffer->Lock(0, 0, (void**)&pIdx, 0);
	archive.saveBinary(&pIdx[0], sizeof(WORD) * mesh.IndexNumber);
	mesh.IndexBuffer->Unlock();
	file.close();
}

//--------------------------------------------------------------------------------
//  saveSkinMeshBinary
//--------------------------------------------------------------------------------
void CMyNode::saveSkinMeshBinary(const Mesh& mesh, const string& meshName)
{
	auto& filePath = "data/skin/" + meshName + ".skin";
	ofstream file(filePath, ios::binary);
	if (!file.is_open()) return;
	BinaryOutputArchive archive(file);

	//DrawType
	auto drawType = TriangleList;
	archive.saveBinary(&drawType, sizeof(DrawType));

	//NumVtx,Idx,Polygon
	archive.saveBinary(&mesh.VertexNumber, sizeof(int));
	archive.saveBinary(&mesh.IndexNumber, sizeof(int));
	archive.saveBinary(&mesh.PolygonNumber, sizeof(int));

	//Vtx
	vector<VertexOutSkin> verteces;
	verteces.reserve(mesh.VertexNumber);
	for (int count = 0; count < mesh.VertexNumber; ++count)
	{
		verteces.push_back((VertexOutSkin)mesh.Verteces[count]);
	}
	archive.saveBinary(&verteces[0], sizeof(VertexOutSkin) * mesh.VertexNumber);
	verteces.clear();

	//Idx
	WORD *pIdx;
	mesh.IndexBuffer->Lock(0, 0, (void**)&pIdx, 0);
	archive.saveBinary(&pIdx[0], sizeof(WORD) * mesh.IndexNumber);
	mesh.IndexBuffer->Unlock();
	file.close();
}