/*****************************************************************//**
 * \file   RendererManagerBase.h
 * \brief  レンダラーマネージャーのベースクラス
 * 
 * \author USAMI KOSHI
 * \date   2021/06/14
 *********************************************************************/
#pragma once

class RendererManagerBase
{
public:
	RendererManagerBase() = default;
	virtual ~RendererManagerBase() = default;


	RendererManagerBase(const RendererManagerBase&) = delete;
	RendererManagerBase(RendererManagerBase&&) = delete;

protected:

};

