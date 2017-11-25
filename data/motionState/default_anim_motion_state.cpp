//--------------------------------------------------------------------------------
//  default_anim_motion_state.cpp
//  this is a motion state class which is auto-created by KF_ModelAnalyzer
//--------------------------------------------------------------------------------
#pragma once
#include "default_anim_motion_state.h"
#include "animator.h"

void DefaultAnimMotionState::ChangeMotion(Animator& animator)
{
	if (current_motion_frame_counter_ >= frame_to_exit_)
	{
		current_motion_frame_counter_ = frame_to_exit_ - 1;
		animator.Change(new BlendMotionState(current_motion_name_, new AutoChangeAnimMotionState("auto_change_anim", 0), current_motion_frame_counter_, 10);
		return;
	}
	if(animator.is_me == false
		|| animator.is_you == true	)
	{
		animator.Change(new BlendMotionState(current_motion_name_, new ChangeAnimMotionState("change_anim", 0), current_motion_frame_counter_, 7);
		return;
	}
	if(animator.my_power == 0.000000f
		|| animator.your_power < 55.555557f	)
	{
		animator.Change(new BlendMotionState(current_motion_name_, new ChangAnim2MotionState("chang_anim2", 0), current_motion_frame_counter_, 12);
		return;
	}
}