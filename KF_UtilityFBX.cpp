//--------------------------------------------------------------------------------
//
//　gameObject3D.h
//	Author : Xu Wenjie
//	Date   : 2017-04-27
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  インクルードファイル
//--------------------------------------------------------------------------------
#include "main.h"
#include "KF_UtilityFBX.h"
#include "KF_Utility.h"

//--------------------------------------------------------------------------------
//
//  Public
//
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  Load
//--------------------------------------------------------------------------------
MyModel CKFUtilityFBX::Load(const string& strFilePath)
{
	MyModel myModel;

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

	// 三角分割
	FbxGeometryConverter lConverter(lSdkManager);
	lConverter.Triangulate(lScene, true);

	// 縮退ポリゴンの削除
	lConverter.RemoveBadPolygonsFromMeshes(lScene);

	// マテリアルごとにメッシュ分離
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
	////パーツ数の読み込み
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

	////Meshの読み込み
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

	////parts infoの読み込み
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
	//		nodes[countPart]->Translation = offsetPosition;
	//		nodes[countPart]->Rotation = offsetRotation;
	//		nodes[countPart]->Scale = CKFVec3(1.0f);
	//		if (parentID >= 0)
	//		{
	//			nodes[parentID]->Children.push_back(nodes[countPart]);
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

	////motionの読み込み
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

	//	//キーフレーム数取得
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

	//	//キーフレームの読み込み
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

	//	//フレームに転換
	//	int countFrame = 0;
	//	for (int countKey = 0; countKey < keyNumber; ++countKey)
	//	{
	//		motionInfo.MotionFrames.resize(motionInfo.MotionFrames.size() + keyFrameNumbers[countKey]);
	//		for (int countKeyFrame = 0; countKeyFrame < keyFrameNumbers[countKey]; ++countKeyFrame)
	//		{
	//			float rate = (float)countKeyFrame / keyFrameNumbers[countKey];
	//			auto currentIterator = keyFrames[countKey].BoneFrames.begin();

	//			if (!motionInfo.IsLoop && countKey == keyNumber - 1)
	//			{// 最後のキーは補間なし
	//				for (int countBone = 0; countBone < partsNumber; ++countBone)
	//				{
	//					motionInfo.MotionFrames[countFrame].BoneFrames.push_back(BoneFrame(currentIterator->Position, currentIterator->Rotation));
	//					++currentIterator;
	//				}
	//			}
	//			else
	//			{// 補間
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
bool CKFUtilityFBX::Save(const MyModel& model, const string& fileName, const OutType& type)
{
	if (type == Json)
	{
		//Model
		auto& filePath = "data/model/" + fileName + ".json";

		//上書き確認
		ifstream checkFile(filePath);
		if (checkFile.is_open())
		{
			checkFile.close();
			auto id = MessageBox(NULL, "上書きしますか？", "確認", MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2);
			if (id == IDNO) return false;
		}

		//Save
		ofstream file(filePath);
		if (!file.is_open()) return false;
		JSONOutputArchive archive(file);
		model.pNode->RecursiveSave(archive, fileName, model.pAnimator ? true : false);
		if (model.pAnimator)model.pAnimator->SaveAsJson(fileName);
		file.close();
	}
	else if (type == Binary)
	{
		//Model
		auto& filePath = "data/model/" + fileName + ".model";

		//上書き確認
		ifstream checkFile(filePath);
		if (checkFile.is_open())
		{
			checkFile.close();
			auto id = MessageBox(NULL, "上書きしますか？", "確認", MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2);
			if (id == IDNO) return false;
		}

		//Save
		ofstream file(filePath);
		if (!file.is_open()) return false;
		BinaryOutputArchive archive(file);
		model.pNode->RecursiveSave(archive, fileName, model.pAnimator ? true : false);
		if (model.pAnimator)model.pAnimator->SaveAsBinary(fileName);
		file.close();
	}
	//Animator

	return true;
}

#ifdef USING_DIRECTX
//--------------------------------------------------------------------------------
//  FindRepetition
//	重ねたら番号を返す、そうじゃないなら-1を返す
//--------------------------------------------------------------------------------
int CKFUtilityFBX::FindRepetition(const list<VertexDX>& listVtx, const VertexDX& vtx)
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
//
//  Private
//
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  recursiveNode
//--------------------------------------------------------------------------------
CMyNode* CKFUtilityFBX::recursiveNode(FbxManager* pManager, FbxNode* pNode)
{
	if (!pNode) { return NULL; }
	auto pMyNode = new CMyNode;
	pMyNode->Name = pNode->GetName();
	pMyNode->Translation.m_fX = static_cast<float>(pNode->LclTranslation.Get()[0]);
	pMyNode->Translation.m_fY = static_cast<float>(pNode->LclTranslation.Get()[1]);
	pMyNode->Translation.m_fZ = static_cast<float>(pNode->LclTranslation.Get()[2]);
	pMyNode->Rotation.m_fX = static_cast<float>(pNode->LclRotation.Get()[0]);
	pMyNode->Rotation.m_fY = static_cast<float>(pNode->LclRotation.Get()[1]);
	pMyNode->Rotation.m_fZ = static_cast<float>(pNode->LclRotation.Get()[2]);
	pMyNode->Rotation /= 180.0f * KF_PI; //Degree to Radian
	pMyNode->Scale.m_fX = static_cast<float>(pNode->LclScaling.Get()[0]);
	pMyNode->Scale.m_fY = static_cast<float>(pNode->LclScaling.Get()[1]);
	pMyNode->Scale.m_fZ = static_cast<float>(pNode->LclScaling.Get()[2]);
	pMyNode->Local.m_af[0][0] = pMyNode->Scale.m_fX;
	pMyNode->Local.m_af[1][1] = pMyNode->Scale.m_fY;
	pMyNode->Local.m_af[2][2] = pMyNode->Scale.m_fZ;
	CKFMtx44 mtxRot;
	CKFMath::MtxRotationYawPitchRoll(mtxRot, pMyNode->Rotation);
	pMyNode->Local *= mtxRot;
	CKFMtx44 mtxPos;
	CKFMath::MtxTranslation(mtxPos, pMyNode->Translation);
	pMyNode->Local *= mtxPos;

	// マテリアル情報の解析（マテリアルリスト化）                
	pMyNode->analyzeTexture(pNode);

	for (int nCnt = 0; nCnt < pNode->GetNodeAttributeCount(); nCnt++)
	{
		auto type = pNode->GetNodeAttributeByIndex(nCnt)->GetAttributeType();
		pMyNode->AttributeNames.push_back(getAttributeTypeName(type));

		if (type == FbxNodeAttribute::eMesh)
		{//Mesh情報
			pMyNode->Meshes.push_back(Mesh());

			// メッシュ情報の取得                 
			FbxMesh* pMesh = FbxCast<FbxMesh>(pNode->GetNodeAttributeByIndex(nCnt));

			// 頂点座標解析                 
			pMyNode->analyzePoint(pMesh);

			// 法線解析                 
			pMyNode->analyzeNormal(pMesh);

			// UV解析                
			pMyNode->analyzeUV(pMesh);

			// マテリアル解析（参照情報の取得）                 
			pMyNode->analyzeMaterial(pMesh);

			// ボーンにの重さ解析 
			pMyNode->analyzeCluster(pMesh);
		}
		else if (type == FbxNodeAttribute::eSkeleton)
		{//Bone
			//auto pSkeleton = FbxCast<FbxSkeleton>(pNode->GetNodeAttributeByIndex(nCnt));
			//pMyNode->analyzeSkeleton(pSkeleton);
		}
		else
		{
			// メッシュではないアトリビュート   
			MessageBox(NULL, getAttributeTypeName(type).c_str(), "アトリビュート", MB_OK);
		}
	}

	for (int nCnt = 0; nCnt < pNode->GetChildCount(); ++nCnt)
	{
		pMyNode->Children.push_back(recursiveNode(pManager, pNode->GetChild(nCnt)));
	}

	return pMyNode;
}

//--------------------------------------------------------------------------------
//  analyzeAnimation
//--------------------------------------------------------------------------------
CAnimator* CKFUtilityFBX::analyzeAnimation(FbxImporter* lImporter, FbxScene* lScene)
{
	// Anim数
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
		MessageBox(NULL, "clusterが見つからない！！", "analyzeAnimation", MB_OK | MB_ICONWARNING);
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
		pAnimator->Clusters[countCluster].BindPoseInverse = CKFMtx44::FbxToMtx(boneGlobalInitPosition.Inverse() * meshGlobalInitPosition);
		++countCluster;
	}

	//one frame time
	FbxTime oneFrameTime;
	oneFrameTime.SetTime(0, 0, 0, 1, 0, 0, FbxTime::eFrames60);

	for (int nCnt = 0; nCnt < animationNumber; ++nCnt)
	{
		Motion motion;
		motion.Name = animationNames[nCnt]->Buffer();

		//Anime情報
		auto pTakeInfo = lScene->GetTakeInfo(animationNames[nCnt]->Buffer());
	
		//アニメーション開始終了時間
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

				// アニメーション行列の算出
				auto skeletonGlobalCurrentTransform = pair.second->GetLink()->EvaluateGlobalTransform(currentTime);
				FbxAMatrix lReferenceGlobalCurrentPosition;
				auto lClusterRelativeCurrentPositionInverse = lReferenceGlobalCurrentPosition.Inverse() * skeletonGlobalCurrentTransform;
				//auto VertexTransformMatrix = lClusterRelativeCurrentPositionInverse * pAnimator->Clusters[countCluster].BindPoseInverse;
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
	// Anim数
	auto animationNumber = lImporter->GetAnimStackCount();
	if (!animationNumber) return;

	// Skeleton
	list<FbxNode*> skeletons;
	findSkeletons(lScene->GetRootNode(), skeletons);
	if (skeletons.size() != animator->Clusters.size())
	{
		MessageBox(NULL, "skeletonの数が合わない！！", "analyzeAnimation", MB_OK | MB_ICONWARNING);
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

		//Anime情報
		auto pTakeInfo = lScene->GetTakeInfo(animationNames[nCnt]->Buffer());

		//アニメーション開始終了時間
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
				for (int countCluster = 0; countCluster < (int)animator->Clusters.size(); ++countCluster)
				{
					if (name == animator->Clusters[countCluster].Name)
					{
						clusterNo = countCluster;
						break;
					}
				}
				frame.BoneFrames[clusterNo].Name = name;

				// アニメーション行列の算出
				//auto skeletonGlobalCurrentTransform = skeleton->EvaluateGlobalTransform(currentTime);
				//FbxAMatrix lReferenceGlobalCurrentPosition;
				//auto lClusterRelativeCurrentPositionInverse = lReferenceGlobalCurrentPosition.Inverse() * skeletonGlobalCurrentTransform;
				//auto VertexTransformMatrix = lClusterRelativeCurrentPositionInverse * animator->Clusters[clusterNo].BindPoseInverse;
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
	for (auto child : node->Children)
	{
		matchClusterWithSkeleton(clusters, child);
	}
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