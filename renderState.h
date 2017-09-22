//--------------------------------------------------------------------------------
//	�����_�[�X�e�[�g�N���X
//�@renderState.h
//	Author : Xu Wenjie
//	Date   : 2017-05-22
//--------------------------------------------------------------------------------
#pragma once

//--------------------------------------------------------------------------------
//  �C���N���[�h�t�@�C��
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//  �N���X�錾
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  �X�[�p�[�N���X
//--------------------------------------------------------------------------------
class CRenderState
{
public:
	CRenderState() {}
	~CRenderState() {}

	virtual void	SetRenderState(void) = 0;
	virtual void	ResetRenderState(void) = 0;
};

//--------------------------------------------------------------------------------
//  LightOff�N���X
//--------------------------------------------------------------------------------
class CLightOffRenderState : public CRenderState
{
public:
	CLightOffRenderState() : CRenderState() {}
	~CLightOffRenderState() {}

	void	SetRenderState(void) override;
	void	ResetRenderState(void) override;
};

//--------------------------------------------------------------------------------
//  LightOffCullOff�N���X
//--------------------------------------------------------------------------------
class CLightOffCullOffRenderState : public CRenderState
{
public:
	CLightOffCullOffRenderState() : CRenderState() {}
	~CLightOffCullOffRenderState() {}

	void	SetRenderState(void) override;
	void	ResetRenderState(void) override;
};

//--------------------------------------------------------------------------------
//  LightOffCullOn�N���X
//--------------------------------------------------------------------------------
class CLightOffCullOnRenderState : public CRenderState
{
public:
	CLightOffCullOnRenderState() : CRenderState() {}
	~CLightOffCullOnRenderState() {}

	void	SetRenderState(void) override;
	void	ResetRenderState(void) override;
};

//--------------------------------------------------------------------------------
//  LightOnCullOff�N���X
//--------------------------------------------------------------------------------
class CLightOnCullOffRenderState : public CRenderState
{
public:
	CLightOnCullOffRenderState() : CRenderState() {}
	~CLightOnCullOffRenderState() {}

	void	SetRenderState(void) override;
	void	ResetRenderState(void) override;
};

//--------------------------------------------------------------------------------
//  LightOnCullOn�N���X
//--------------------------------------------------------------------------------
class CLightOnCullOnRenderState : public CRenderState
{
public:
	CLightOnCullOnRenderState() : CRenderState() {}
	~CLightOnCullOnRenderState() {}

	void	SetRenderState(void) override;
	void	ResetRenderState(void) override;
};

//--------------------------------------------------------------------------------
//  �k���N���X
//--------------------------------------------------------------------------------
class CNullRenderState : public CRenderState
{
public:
	CNullRenderState() : CRenderState() {}
	~CNullRenderState() {}

	void	SetRenderState(void) override {}
	void	ResetRenderState(void) override {}
};