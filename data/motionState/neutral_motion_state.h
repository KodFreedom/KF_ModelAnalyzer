//--------------------------------------------------------------------------------
//  neutral_motion_state.h
//  this is a motion state class which is auto-created by KF_ModelAnalyzer
//--------------------------------------------------------------------------------
#pragma once
#include "motion_state.h"

class NeutralMotionState : public NormalMotionState
{
public:
	NeutralMotionState(const int start_frame) : NormalMotionState("neutral", start_frame) {}
	~NeutralMotionState() {}

private:
	void ChangeMotion(Animator& animator) override;
};