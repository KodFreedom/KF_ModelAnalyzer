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
		Clusters[count].Node->Translation = current.BoneFrames[count].Translation;
		Clusters[count].Node->Rotation = current.BoneFrames[count].Rotation;
		Clusters[count].Node->Scale = current.BoneFrames[count].Scale;
	}
}

//--------------------------------------------------------------------------------
//  UpdateClusterWorld
//--------------------------------------------------------------------------------
void CAnimator::UpdateClusterWorld(void)
{
	for (auto& cluster : Clusters)
	{
		if (cluster.Name == "sword_1")
		{
			cluster.World = cluster.Node->World;
			continue;
		}
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
		newFrames.reserve(current.EndFrame + 1 - current.StartFrame);
		for (int count = current.StartFrame; count <= current.EndFrame; ++count)
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

	// Cluster
	archive(make_nvp("Clusters", Clusters));

	// IKController
	static const char* IKName[eIKMax] =
	{
		"Hips",
		"Spine",
		"ShoulderLeft",
		"UpperArmLeft",
		"LowerArmLeft",
		"HandLeft",
		"ShoulderRight",
		"UpperArmRight",
		"LowerArmRight",
		"HandRight",
		"UpperLegLeft",
		"LowerLegLeft",
		"FootLeft",
		"ToesLeft",
		"UpperLegRight",
		"LowerLegRight",
		"FootRight",
		"ToesRight",
	};
	for (auto& controller : IKControllers)
	{
		archive(make_nvp(IKName[controller.index], controller.index));
	}

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
	ofstream file(filePath, ios::binary);
	if (!file.is_open()) return;
	BinaryOutputArchive archive(file);

	// Cluster
	int clusterNumber = (int)Clusters.size();
	archive.saveBinary(&clusterNumber, sizeof(clusterNumber));
	for (auto& cluster : Clusters)
	{
		int size = (int)cluster.Name.size();
		archive.saveBinary(&size, sizeof(size));
		archive.saveBinary(&cluster.Name[0], size);
		archive.saveBinary(&cluster.BindPoseInverse, sizeof(CKFMtx44));
	}

	// IKController
	for (auto& controller : IKControllers)
	{
		archive.saveBinary(&controller.index, sizeof(controller.index));
	}

	// Motion
	int motionNumber = (int)Motions.size();
	archive.saveBinary(&motionNumber, sizeof(motionNumber));
	for (int count = 0; count < motionNumber; ++count)
	{
		int size = (int)Motions[count].Name.size();
		archive.saveBinary(&size, sizeof(size));
		archive.saveBinary(&Motions[count].Name[0], size);
		DeleteOutOfRangeFrames(count);
		saveAsBinary(Motions[count]);
	}
	file.close();
}

//--------------------------------------------------------------------------------
//  SaveMotionTransitions
//--------------------------------------------------------------------------------
void CAnimator::SaveMotionTransitions(const int motionNo)
{
	saveMotionTransitionsHead(motionNo);
	saveMotionTransitionsCpp(motionNo);
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
	ofstream file(filePath, ios::binary);
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
			archive.saveBinary(&boneFrame.Translation, sizeof(CKFVec3));
			archive.saveBinary(&boneFrame.Rotation, sizeof(CKFQuaternion));
			archive.saveBinary(&boneFrame.Scale, sizeof(CKFVec3));
		}
	}
	file.close();
}

//--------------------------------------------------------------------------------
//  saveMotionTransitionsHead
//--------------------------------------------------------------------------------
void CAnimator::saveMotionTransitionsHead(const int motionNo)
{
	auto& motion = Motions[motionNo];
	auto& filePath = "data/motionState/" + motion.Name + "_motion_state.h";
	ifstream checkFile(filePath);
	if (checkFile.is_open())
	{
		checkFile.close();
		auto nID = MessageBox(NULL, "上書きしますかしますか？", filePath.c_str(), MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2);
		if (nID == IDNO)
		{
			return;
		}
	}
	ofstream file(filePath);

	auto& className = CKFUtility::ParameterNameToMethodName(motion.Name) + "MotionState";
	file << "//--------------------------------------------------------------------------------\n";
	file << "//  " + motion.Name + "_motion_state.h\n";
	file << "//  this is a motion state class which is auto-created by KF_ModelAnalyzer\n";
	file << "//--------------------------------------------------------------------------------\n";
	file << "#pragma once\n";
	file << "#include \"motion_state.h\"\n\n";
	file << "class " + className + " : public NormalMotionState\n";
	file << "{\n";
	file << "public:\n";
	file << "\t" + className + "(const int start_frame) : NormalMotionState(\"" + motion.Name + "\", start_frame) {}\n";
	file << "\t~" + className + "() {}\n\n";
	file << "private:\n";
	file << "\tvoid ChangeMotion(Animator& animator) override;\n";
	if (!motion.IsLoop && motion.ChangeWhenOver)
	{
		file << "\tconst int frame_to_exit_ = " + std::to_string(motion.ChangeWhenOverExitFrame) + ";\n";
	}
	file << "};";
	file.close();
	MessageBox(NULL, "セーブしました。", filePath.c_str(), MB_OK);
}

//--------------------------------------------------------------------------------
//  saveMotionTransitionsCpp
//--------------------------------------------------------------------------------
void CAnimator::saveMotionTransitionsCpp(const int motionNo)
{
	auto& motion = Motions[motionNo];
	auto& filePath = "data/motionState/" + motion.Name + "_motion_state.cpp";
	ifstream checkFile(filePath);
	if (checkFile.is_open())
	{
		checkFile.close();
		auto nID = MessageBox(NULL, "上書きしますかしますか？", filePath.c_str(), MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2);
		if (nID == IDNO)
		{
			return;
		}
	}
	ofstream file(filePath);

	file << "//--------------------------------------------------------------------------------\n";
	file << "//  " + motion.Name + "_motion_state.cpp\n";
	file << "//  this is a motion state class which is auto-created by KF_ModelAnalyzer\n";
	file << "//--------------------------------------------------------------------------------\n";

	// Include
	file << "#include \"" + motion.Name + "_motion_state.h" + "\"\n";
	file << "#include \"animator.h\"\n";
	file << "#include \"motion_data.h\"\n";
	if (!motion.IsLoop && motion.ChangeWhenOver)
	{
		file << "#include \"" + motion.ChangeWhenOverNextMotion + "_motion_state.h" + "\"\n";
	}
	for (auto& transition : motion.Transitions)
	{
		if (transition.Conditions.empty()) continue;
		file << "#include \"" + transition.NextMotion + "_motion_state.h" + "\"\n";
	}

	auto& className = CKFUtility::ParameterNameToMethodName(motion.Name) + "MotionState";
	file << "void " + className + "::ChangeMotion(Animator& animator)\n";
	file << "{\n";

	// Change when over
	if (!motion.IsLoop)
	{
		if (motion.ChangeWhenOver)
		{
			auto& nextClassName = CKFUtility::ParameterNameToMethodName(motion.ChangeWhenOverNextMotion) + "MotionState";
			file << "\tif (current_frame_counter_ >= frame_to_exit_)\n";
			file << "\t{\n";
			file << "\t\tcurrent_frame_counter_ = frame_to_exit_ - 1;\n";
			file << "\t\tanimator.Change(MY_NEW BlendMotionState(current_motion_name_, "
				"MY_NEW " + nextClassName + "(0), "
				+ "current_frame_counter_, "
				+ std::to_string(motion.ChangeWhenOverBlendFrame) + "));\n";
			file << "\t\treturn;\n";
			file << "\t}\n";
		}
		else
		{
			file << "\tif (current_frame_counter_ >= static_cast<int>(current_motion_data_->frames_.size()))\n";
			file << "\t{\n";
			file << "\t\t--current_frame_counter_;\n";
			file << "\t}\n";
		}
	}
	
	// Transition
	for (auto& transition : motion.Transitions)
	{
		if (transition.Conditions.empty()) continue;

		file << "\tif(";
		// Conditions
		for (auto iterator = transition.Conditions.begin();;)
		{
			file << "animator.Get" + CKFUtility::ParameterNameToMethodName(iterator->ParameterName) + "()";
			switch (iterator->ParameterType)
			{
			case eParameterType::eBool:
			{
				file << " == ";
				file << (iterator->BoolValue ? "true" : "false");
				break;
			}
			case eParameterType::eFloat:
			{
				file << OperatorToString(iterator->FloatOperator);
				file << std::to_string(iterator->FloatValue) + "f";
				break;
			}
			}

			if (++iterator == transition.Conditions.end()) break;
			file << "\n\t|| ";
		}
		file << ")\n";
		file << "\t{\n";

		auto& nextClassName = CKFUtility::ParameterNameToMethodName(transition.NextMotion) + "MotionState";
		file << "\t\tanimator.Change(MY_NEW BlendMotionState(current_motion_name_, "
			"MY_NEW " + nextClassName + "(0), "
			+ "current_frame_counter_, "
			+ std::to_string(transition.BlendFrame) + "));\n";
		file << "\t\treturn;\n";
		file << "\t}\n";
	}
	file << "}";
	file.close();
	MessageBox(NULL, "セーブしました。", filePath.c_str(), MB_OK);
}