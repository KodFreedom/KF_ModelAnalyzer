//--------------------------------------------------------------------------------
//  jugg_neutral_look_around_motion_state.cpp
//  this is a motion state class which is auto-created by KF_ModelAnalyzer
//--------------------------------------------------------------------------------
#include "jugg_neutral_look_around_motion_state.h"
#include "animator.h"
#include "motion_data.h"
#include "jugg_neutral_motion_state.h"
#include "jugg_walk_motion_state.h"
#include "jugg_jump_motion_state.h"
#include "jugg_attack_left_motion_state.h"
#include "jump_flail_motion_state.h"
#include "jugg_spain_motion_state.h"
#include "jugg_death_motion_state.h"
#include "jugg_stun_motion_state.h"
#include "jugg_hand_up_motion_state.h"
void JuggNeutralLookAroundMotionState::ChangeMotion(Animator& animator)
{
	if (current_frame_counter_ >= frame_to_exit_)
	{
		current_frame_counter_ = frame_to_exit_ - 1;
		animator.Change(MY_NEW BlendMotionState(current_motion_name_, MY_NEW JuggNeutralMotionState(0), current_frame_counter_, 10));
		return;
	}
	if(animator.GetMovement() > 0.000000f)
	{
		animator.Change(MY_NEW BlendMotionState(current_motion_name_, MY_NEW JuggWalkMotionState(0), current_frame_counter_, 10));
		return;
	}
	if(animator.GetIsJump() == true)
	{
		animator.Change(MY_NEW BlendMotionState(current_motion_name_, MY_NEW JuggJumpMotionState(0), current_frame_counter_, 10));
		return;
	}
	if(animator.GetIsAttack() == true)
	{
		animator.Change(MY_NEW BlendMotionState(current_motion_name_, MY_NEW JuggAttackLeftMotionState(0), current_frame_counter_, 10));
		return;
	}
	if(animator.GetIsGrounded() == false)
	{
		animator.Change(MY_NEW BlendMotionState(current_motion_name_, MY_NEW JumpFlailMotionState(0), current_frame_counter_, 5));
		return;
	}
	if(animator.GetIsSkill() == true)
	{
		animator.Change(MY_NEW BlendMotionState(current_motion_name_, MY_NEW JuggSpainMotionState(0), current_frame_counter_, 10));
		return;
	}
	if(animator.GetIsDeath() == true)
	{
		animator.Change(MY_NEW BlendMotionState(current_motion_name_, MY_NEW JuggDeathMotionState(0), current_frame_counter_, 5));
		return;
	}
	if(animator.GetIsStun() == true)
	{
		animator.Change(MY_NEW BlendMotionState(current_motion_name_, MY_NEW JuggStunMotionState(0), current_frame_counter_, 10));
		return;
	}
	if(animator.GetIsUltra() == true)
	{
		animator.Change(MY_NEW BlendMotionState(current_motion_name_, MY_NEW JuggHandUpMotionState(0), current_frame_counter_, 10));
		return;
	}
}