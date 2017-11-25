//--------------------------------------------------------------------------------
//  neutral_motion_state.cpp
//  this is a motion state class which is auto-created by KF_ModelAnalyzer
//--------------------------------------------------------------------------------
#include "neutral_motion_state.h"
#include "animator.h"
#include "run_motion_state.h"
void NeutralMotionState::ChangeMotion(Animator& animator)
{
	if(animator.GetMove() > 0.000000f
	|| animator.GetIsMe() == true)
	{
		animator.Change(new BlendMotionState(current_motion_name_, new RunMotionState(0), current_motion_frame_counter_, 10);
		return;
	}
}