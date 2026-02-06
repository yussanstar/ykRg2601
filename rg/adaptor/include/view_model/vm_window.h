#pragma once

#ifndef _YK_VIEW_MODEL_WINDOW_H_
#define _YK_VIEW_MODEL_WINDOW_H_

#include "global_def.h"

namespace rg::adaptor::ViewModel
{
	class VMWindow
	{
	public:
		VMWindow(int x, int y, int w, int h);
		~VMWindow() = default;

	public:
		ykl::STLUniquePtr<VMWindow> clone(const IAllocatorPtr& allocator) const;

	private:
		int x, y, w, h;
	};
}

#endif
