//--------------------------------------------------------------------------------
//
//　animator.cpp
//	Author : Xu Wenjie
//	Date   : 2017-09-15
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  インクルードファイル
//--------------------------------------------------------------------------------
#include "animator.h"
#include "node.h"

//--------------------------------------------------------------------------------
//
//  Public
//
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  UpdateBones
//--------------------------------------------------------------------------------
void CAnimator::UpdateBones(const Frame& current)
{
	for (int count = 0; count < (int)Clusters.size(); ++count)
	{
		Clusters[count].Node->Local = current.BoneFrames[count].Matrix;
	}
}

//--------------------------------------------------------------------------------
//  UpdateClusterWorld
//--------------------------------------------------------------------------------
void CAnimator::UpdateClusterWorld(void)
{
	for (auto& cluster : Clusters)
	{
		cluster.World = cluster.BindPoseInverse * cluster.Node->World;
	}
}

//--------------------------------------------------------------------------------
//  DeleteOutOfRangeFrames
//--------------------------------------------------------------------------------
void CAnimator::DeleteOutOfRangeFrames(const int motionNo)
{
	auto& current = Motions[motionNo];
	if (current.StartFrame > 0 || current.EndFrame < (int)current.Frames.size() - 1)
	{
		vector<Frame> newFrames;
		newFrames.reserve(current.EndFrame - current.StartFrame);
		for (int count = current.StartFrame; count < current.EndFrame; ++count)
		{
			newFrames.push_back(current.Frames[count]);
		}
		current.Frames.clear();
		for (auto& frame : newFrames)
		{
			current.Frames.push_back(frame);
		}
		current.Frames.shrink_to_fit();
		current.StartFrame = 0;
		current.EndFrame = current.Frames.size() - 1;
	}
}

//--------------------------------------------------------------------------------
//  SaveAsJson
//--------------------------------------------------------------------------------
void CAnimator::SaveAsJson(const string& fileName)
{
	auto& filePath = "data/avatar/" + fileName + ".json";
	ofstream file(filePath);
	if (!file.is_open()) return;
	JSONOutputArchive archive(file);
	archive(make_nvp("Clusters", Clusters));

	// Motion
	int motionNumber = (int)Motions.size();
	archive(make_nvp("MotionNumber", motionNumber));
	for (int count = 0; count < motionNumber; ++count)
	{
		archive(make_nvp("MotionName", Motions[count].Name));
		DeleteOutOfRangeFrames(count);
		saveAsJson(Motions[count]);
	}
	file.close();
}

//--------------------------------------------------------------------------------
//  SaveAsBinary
//--------------------------------------------------------------------------------
void CAnimator::SaveAsBinary(const string& fileName)
{
	auto& filePath = "data/avatar/" + fileName + ".avatar";
	ofstream file(filePath);
	if (!file.is_open()) return;
	BinaryOutputArchive archive(file);
	int clusterNumber = (int)Clusters.size();
	archive.saveBinary(&clusterNumber, sizeof(int));
	for (auto& cluster : Clusters)
	{
		int size = (int)cluster.Name.size();
		archive.saveBinary(&size, sizeof(int));
		archive.saveBinary(&cluster.Name[0], size);
		archive.saveBinary(&cluster.BindPoseInverse, sizeof(CKFMtx44));
	}

	// Motion
	int motionNumber = (int)Motions.size();
	archive.saveBinary(&motionNumber, sizeof(int));
	for (int count = 0; count < motionNumber; ++count)
	{
		int size = (int)Motions[count].Name.size();
		archive.saveBinary(&size, sizeof(int));
		archive.saveBinary(&Motions[count].Name[0], size);
		DeleteOutOfRangeFrames(count);
		saveAsBinary(Motions[count]);
	}
	file.close();
}

//--------------------------------------------------------------------------------
//
//  Private
//
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  SaveAsJson
//--------------------------------------------------------------------------------
void CAnimator::saveAsJson(const Motion& motion)
{
	auto& filePath = "data/motion/" + motion.Name + ".json";
	ofstream file(filePath);
	if (!file.is_open()) return;
	JSONOutputArchive archive(file);
	archive(make_nvp("MotionInfo", motion));
	file.close();
}

//--------------------------------------------------------------------------------
//  SaveAsJson
//--------------------------------------------------------------------------------
void CAnimator::saveAsBinary(const Motion& motion)
{
	auto& filePath = "data/motion/" + motion.Name + ".motion";
	ofstream file(filePath);
	if (!file.is_open()) return;
	BinaryOutputArchive archive(file);
	archive.saveBinary(&motion.IsLoop, sizeof(bool));
	int frameNumber = (int)motion.Frames.size();
	archive.saveBinary(&frameNumber, sizeof(int));
	for (auto& frame : motion.Frames)
	{
		int boneFrameNumber = (int)frame.BoneFrames.size();
		archive.saveBinary(&boneFrameNumber, sizeof(int));
		for (auto& boneFrame : frame.BoneFrames)
		{
			archive.saveBinary(&boneFrame.Matrix, sizeof(CKFMtx44));
		}
	}
	file.close();
}