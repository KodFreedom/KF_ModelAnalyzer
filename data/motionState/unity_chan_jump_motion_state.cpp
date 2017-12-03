//--------------------------------------------------------------------------------
//  unity_chan_jump_motion_state.cpp
//  this is a motion state class which is auto-created by KF_ModelAnalyzer
//--------------------------------------------------------------------------------
#include "unity_chan_jump_motion_state.h"
#include "animator.h"
#include "unity_chan_land_motion_state.h"
void UnityChanJumpMotionState::ChangeMotion(Animator& animator)
{
	if (current_motion_frame_counter_ >= frameNumber)
	{
		--current_motion_frame_counter_;
	}
	if(animator.GetIsGrounded() == true)
	{
		animator.Change(new BlendMotionState(current_motion_name_, new UnityChanLandMotionState(0), current_motion_frame_counter_, 10);
		return;
	}
}