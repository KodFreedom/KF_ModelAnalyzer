//--------------------------------------------------------------------------------
//  default_anim_motion_state.h
//  this is a motion state class which is auto-created by KF_ModelAnalyzer
//--------------------------------------------------------------------------------
#pragma once
#include "motion_state.h"

class DefaultAnimMotionState : public NormalMotionState
{
public:
	DefaultAnimMotionState(const int start_frame) : NormalMotionState("default_anim", start_frame) {}
		~DefaultAnimMotionState() {}

private:
	void ChangeMotion(Animator& animator) override
	const int frame_to_exit_ = 20;
};