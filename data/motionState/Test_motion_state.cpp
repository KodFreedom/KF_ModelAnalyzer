//--------------------------------------------------------------------------------
//  Test_motion_state.cpp
//  this is a motion state class which is auto-created by KF_ModelAnalyzer
//--------------------------------------------------------------------------------
#include "Test_motion_state.h"
#include "animator.h"
#include "motion_data.h"
#include "Walk_motion_state.h"
void TestMotionState::ChangeMotion(Animator& animator)
{
	if(animator.GetMovement() > 5.000000f)
	{
		animator.Change(MY_NEW BlendMotionState(current_motion_name_, MY_NEW WalkMotionState(0), current_frame_counter_, 5));
		return;
	}
}