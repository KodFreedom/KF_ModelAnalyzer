//--------------------------------------------------------------------------------
//	レンダーステートクラス
//　renderState.h
//	Author : Xu Wenjie
//	Date   : 2017-05-22
//--------------------------------------------------------------------------------
#pragma once

//--------------------------------------------------------------------------------
//  インクルードファイル
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//  クラス宣言
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  スーパークラス
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
//  LightOffクラス
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
//  LightOffCullOffクラス
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
//  LightOffCullOnクラス
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
//  LightOnCullOffクラス
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
//  LightOnCullOnクラス
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
//  ヌルクラス
//--------------------------------------------------------------------------------
class CNullRenderState : public CRenderState
{
public:
	CNullRenderState() : CRenderState() {}
	~CNullRenderState() {}

	void	SetRenderState(void) override {}
	void	ResetRenderState(void) override {}
};