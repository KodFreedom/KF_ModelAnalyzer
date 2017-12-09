//--------------------------------------------------------------------------------
//  unity_chan_damaged_motion_state.h
//  this is a motion state class which is auto-created by KF_ModelAnalyzer
//--------------------------------------------------------------------------------
#pragma once
#include "motion_state.h"

class UnityChanDamagedMotionState : public NormalMotionState
{
public:
	UnityChanDamagedMotionState(const int start_frame) : NormalMotionState("unity_chan_damaged", start_frame) {}
	~UnityChanDamagedMotionState() {}

private:
	void ChangeMotion(Animator& animator) override;
	const int frame_to_exit_ = 62;
};