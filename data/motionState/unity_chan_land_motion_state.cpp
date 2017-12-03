//--------------------------------------------------------------------------------
//  unity_chan_land_motion_state.cpp
//  this is a motion state class which is auto-created by KF_ModelAnalyzer
//--------------------------------------------------------------------------------
#include "unity_chan_land_motion_state.h"
#include "animator.h"
#include "unity_chan_wait_motion_state.h"
#include "unity_chan_fall_down_motion_state.h"
#include "unity_chan_walk_motion_state.h"
#include "unity_chan_damaged_motion_state.h"
void UnityChanLandMotionState::ChangeMotion(Animator& animator)
{
	if (current_motion_frame_counter_ >= frame_to_exit_)
	{
		current_motion_frame_counter_ = frame_to_exit_ - 1;
		animator.Change(new BlendMotionState(current_motion_name_, new UnityChanWaitMotionState(0), current_motion_frame_counter_, 5);
		return;
	}
	if(animator.GetIsDead() == true)
	{
		animator.Change(new BlendMotionState(current_motion_name_, new UnityChanFallDownMotionState(0), current_motion_frame_counter_, 5);
		return;
	}
	if(animator.GetMovement() > 0.000000f)
	{
		animator.Change(new BlendMotionState(current_motion_name_, new UnityChanWalkMotionState(0), current_motion_frame_counter_, 5);
		return;
	}
	if(animator.GetIsDamaged() == true)
	{
		animator.Change(new BlendMotionState(current_motion_name_, new UnityChanDamagedMotionState(0), current_motion_frame_counter_, 5);
		return;
	}
}