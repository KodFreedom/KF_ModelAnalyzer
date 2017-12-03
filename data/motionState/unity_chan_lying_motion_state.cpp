//--------------------------------------------------------------------------------
//  unity_chan_lying_motion_state.cpp
//  this is a motion state class which is auto-created by KF_ModelAnalyzer
//--------------------------------------------------------------------------------
#include "unity_chan_lying_motion_state.h"
#include "animator.h"
#include "unity_chan_rise_up_motion_state.h"
void UnityChanLyingMotionState::ChangeMotion(Animator& animator)
{
	if(animator.GetIsRiseUp() == true)
	{
		animator.Change(new BlendMotionState(current_motion_name_, new UnityChanRiseUpMotionState(0), current_motion_frame_counter_, 5);
		return;
	}
}