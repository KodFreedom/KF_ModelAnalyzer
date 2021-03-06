//--------------------------------------------------------------------------------
//  jugg_ultra_motion_state.h
//  this is a motion state class which is auto-created by KF_ModelAnalyzer
//--------------------------------------------------------------------------------
#pragma once
#include "motion_state.h"

class JuggUltraMotionState : public NormalMotionState
{
public:
	JuggUltraMotionState(const int start_frame) : NormalMotionState("jugg_ultra", start_frame) {}
	~JuggUltraMotionState() {}

private:
	void ChangeMotion(Animator& animator) override;
	const int frame_to_exit_ = 41;
};