/*****************************************************************//**
 * \file   RendererManagerBase.h
 * \brief  レンダラーマネージャーのベースクラス
 * 
 * \author USAMI KOSHI
 * \date   2021/06/14
 *********************************************************************/
#pragma once

/// @brief レンダラーのベースクラス
/// @class RendererManagerBase
class RendererManagerBase
{
public:
	RendererManagerBase() = default;
	virtual ~RendererManagerBase() = default;

	virtual void finalize() = 0;
	virtual void clear() = 0;
	virtual void present() = 0;


	RendererManagerBase(const RendererManagerBase&) = delete;
	RendererManagerBase(RendererManagerBase&&) = delete;

protected:

};

