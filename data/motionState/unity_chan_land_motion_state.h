//--------------------------------------------------------------------------------
//  unity_chan_land_motion_state.h
//  this is a motion state class which is auto-created by KF_ModelAnalyzer
//--------------------------------------------------------------------------------
#pragma once
#include "motion_state.h"

class UnityChanLandMotionState : public NormalMotionState
{
public:
	UnityChanLandMotionState(const int start_frame) : NormalMotionState("unity_chan_land", start_frame) {}
	~UnityChanLandMotionState() {}

private:
	void ChangeMotion(Animator& animator) override;
	const int frame_to_exit_ = 43;
};