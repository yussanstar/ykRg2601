#pragma once

#ifndef _YKT_UI2D_TEST_H_
#define _YKT_UI2D_TEST_H_

#include "global_def.h"

namespace ykl::test
{
	class UI2DTest
	{
	public:
		virtual ~UI2DTest() {}

	public:
		static STLUniquePtr<UI2DTest> Create(IAllocatorPtr _allocator);
		virtual bool Launch() = 0;
		virtual bool IsRequiredExitLoop() = 0;
		virtual void Loop() = 0;
		virtual void Terminate() = 0;
	};
}

#endif
