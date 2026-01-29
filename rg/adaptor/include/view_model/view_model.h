#pragma once

#ifndef _YK_VIEW_MODEL_H_
#define _YK_VIEW_MODEL_H_

#include "global_del.h"
#include <list>
#include "vm_window.h"

namespace rg::adaptor::ViewModel
{
	class ViewModel
	{
	public:
		using WindowHandle = int;

	public:
		ViewModel();
		~ViewModel() = default;

	public:
		WindowHandle AddWindow(ykl::STLUniquePtr<VMWindow>&& newWindow);
		void RemoveWindow(WindowHandle winHdl);

	public:
		ykl::STLUniquePtr<VMWindow> GetWindow(WindowHandle winHdl) const;
		WindowHandle ConsumeNewWindow() const;
		WindowHandle ConsumeRemovedWindow() const;

	private:
		std::unordered_map<WindowHandle, ykl::STLUniquePtr<VMWindow>> m_windows;
		mutable std::list<WindowHandle> m_newWindowHandles;
		mutable std::list<WindowHandle> m_removedWindowHandles;
	};
}

#endif
