//--------------------------------------------------------------------------------
//  Test_motion_state.h
//  this is a motion state class which is auto-created by KF_ModelAnalyzer
//--------------------------------------------------------------------------------
#pragma once
#include "motion_state.h"

class TestMotionState : public NormalMotionState
{
public:
	TestMotionState(const int start_frame) : NormalMotionState("Test", start_frame) {}
	~TestMotionState() {}

private:
	void ChangeMotion(Animator& animator) override;
};