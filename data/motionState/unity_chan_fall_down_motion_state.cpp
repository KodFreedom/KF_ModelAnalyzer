//--------------------------------------------------------------------------------
//  unity_chan_fall_down_motion_state.cpp
//  this is a motion state class which is auto-created by KF_ModelAnalyzer
//--------------------------------------------------------------------------------
#include "unity_chan_fall_down_motion_state.h"
#include "animator.h"
#include "unity_chan_lying_motion_state.h"
void UnityChanFallDownMotionState::ChangeMotion(Animator& animator)
{
	if (current_motion_frame_counter_ >= frame_to_exit_)
	{
		current_motion_frame_counter_ = frame_to_exit_ - 1;
		animator.Change(new BlendMotionState(current_motion_name_, new UnityChanLyingMotionState(0), current_motion_frame_counter_, 5);
		return;
	}
}