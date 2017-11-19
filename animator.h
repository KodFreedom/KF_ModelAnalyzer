#pragma once
//--------------------------------------------------------------------------------
//  �C���N���[�h�t�@�C��
//--------------------------------------------------------------------------------
#include "main.h"

//--------------------------------------------------------------------------------
//  �O���錾
//--------------------------------------------------------------------------------
class CMyNode;

//--------------------------------------------------------------------------------
//  �\���̒�`
//--------------------------------------------------------------------------------
struct BoneFrame
{
	string		Name;
	CKFMtx44	Matrix;
};

struct Frame
{
	Frame() {}
	~Frame() { BoneFrames.clear(); BoneFrames.shrink_to_fit(); }
	vector<BoneFrame> BoneFrames;
};

struct Motion
{
	Motion() : StartFrame(0), EndFrame(0)
	{
		Name.clear();
		Frames.clear();
	}
	string			Name;
	vector<Frame>	Frames;
	int				StartFrame;
	int				EndFrame;
};

struct Cluster
{
	Cluster() : Node(nullptr) {}
	string		Name;
	CMyNode*	Node;
	CKFMtx44	BindPoseInverse;
	CKFMtx44	World;
};

//--------------------------------------------------------------------------------
//  �N���X��`
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
};
