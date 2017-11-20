//--------------------------------------------------------------------------------
//
//　animator.h
//	Author : Xu Wenjie
//	Date   : 2017-09-15
//--------------------------------------------------------------------------------
#pragma once

//--------------------------------------------------------------------------------
//  インクルードファイル
//--------------------------------------------------------------------------------
#include "main.h"

//--------------------------------------------------------------------------------
//  前方宣言
//--------------------------------------------------------------------------------
class CMyNode;

//--------------------------------------------------------------------------------
//  構造体定義
//--------------------------------------------------------------------------------
struct BoneFrame
{
	string		Name;
	CKFMtx44	Matrix;

	template <class Archive>
	void serialize(Archive & ar)
	{
		ar(make_nvp("Matrix", Matrix));
	}
};

struct Frame
{
	Frame() {}
	~Frame() { BoneFrames.clear(); BoneFrames.shrink_to_fit(); }
	vector<BoneFrame> BoneFrames;

	template <class Archive>
	void serialize(Archive & ar)
	{
		ar(make_nvp("BoneFrames", BoneFrames));
	}
};

struct Motion
{
	Motion() : StartFrame(0), EndFrame(0), IsLoop(true)
	{
		Name.clear();
		Frames.clear();
	}
	string			Name;
	vector<Frame>	Frames;
	int				StartFrame;
	int				EndFrame;
	bool			IsLoop;

	template <class Archive>
	void serialize(Archive & ar)
	{
		ar(make_nvp("IsLoop", IsLoop), make_nvp("Frames", Frames));
	}
};

struct Cluster
{
	Cluster() : Node(nullptr) {}
	string		Name;
	CMyNode*	Node;
	CKFMtx44	BindPoseInverse;
	CKFMtx44	World;

	template <class Archive>
	void serialize(Archive & ar)
	{
		ar(make_nvp("Name", Name), make_nvp("BindPoseInverse", BindPoseInverse));
	}
};

//--------------------------------------------------------------------------------
//  クラス定義
//--------------------------------------------------------------------------------
class CAnimator
{
public:
	CAnimator() {}
	~CAnimator() {}

	void Release(void)
	{
		for (auto& motion : Motions)
		{
			for (auto& frame : motion.Frames)
			{
				for (auto& boneFrame : frame.BoneFrames) { boneFrame.Name.clear(); }
				frame.BoneFrames.clear();
			}
			motion.Frames.clear();
		}
		Motions.clear();
	}

	vector<Motion> Motions;
	vector<Cluster> Clusters;

	void UpdateBones(const Frame& current);
	void UpdateClusterWorld(void);
	void DeleteOutOfRangeFrames(const int motionNo);
	void SaveAsJson(const string& fileName);
	void SaveAsBinary(const string& fileName);

private:
	void saveAsJson(const Motion& motion);
	void saveAsBinary(const Motion& motion);
};
