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
//  列挙型定義
//--------------------------------------------------------------------------------
enum eParameterType
{
	eBool,
	eFloat
};

enum eFloatOperator
{
	fEqual,
	fNotEqual,
	fGreater,
	fLess,
};

//--------------------------------------------------------------------------------
//  構造体定義
//--------------------------------------------------------------------------------
struct BoneFrame
{
	string			Name;
	CKFVec3			Translation;
	CKFQuaternion	Rotation;
	CKFVec3			Scale;

	template <class Archive>
	void serialize(Archive & ar)
	{
		ar(make_nvp("Translation", Translation)
			, make_nvp("Rotation", Rotation)
			, make_nvp("Scale", Scale));
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

struct Condition
{
	Condition() : ParameterType(eBool), BoolValue(true), FloatOperator(fEqual), FloatValue(0.0f) {}
	eParameterType	ParameterType;
	string			ParameterName;
	bool			BoolValue;
	eFloatOperator	FloatOperator;
	float			FloatValue;
};

struct StateTransition
{
	StateTransition() : BlendFrame(10) {}
	string				NextMotion;
	int					BlendFrame;
	list<Condition>		Conditions;
};

struct Motion
{
	Motion() : StartFrame(0), EndFrame(0)
		, ChangeWhenOverExitFrame(0), ChangeWhenOverBlendFrame(10)
		, IsLoop(true), ChangeWhenOver(true)
	{
		Name.clear();
		Frames.clear();
	}
	string					Name;
	vector<Frame>			Frames;
	int						StartFrame;
	int						EndFrame;
	int						ChangeWhenOverExitFrame;
	int						ChangeWhenOverBlendFrame;
	string					ChangeWhenOverNextMotion;
	bool					IsLoop;
	bool					ChangeWhenOver;
	list<StateTransition>	Transitions;

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
	void SaveMotionTransitions(const int motionNo);

private:
	void saveAsJson(const Motion& motion);
	void saveAsBinary(const Motion& motion);
	void saveMotionTransitionsHead(const int motionNo);
	void saveMotionTransitionsCpp(const int motionNo);
	string OperatorToString(const eFloatOperator& value)
	{
		switch (value)
		{
		case eFloatOperator::fEqual:
			return " == ";
		case eFloatOperator::fNotEqual:
			return " != ";
		case eFloatOperator::fGreater:
			return " > ";
		case eFloatOperator::fLess:
			return " < ";
		default:
			return " == ";
		}
	}
};
