
#include "view_model/view_model.h"
#include <atomic>

namespace rg::adaptor::ViewModel
{

	static std::atomic<int> s_nextWindowHandle{1};

	ViewModel::ViewModel()
	{
	}

	ViewModel::WindowHandle ViewModel::AddWindow(ykl::STLUniquePtr<VMWindow>&& newWindow)
	{
		if(!newWindow) return 0;

		const WindowHandle handle = s_nextWindowHandle.fetch_add(1);
		m_windows.emplace(handle, std::move(newWindow));
		m_newWindowHandles.push_back(handle);
		return handle;
	}

	void ViewModel::RemoveWindow(WindowHandle winHdl)
	{
		auto it = m_windows.find(winHdl);
		if(it == m_windows.end()) return;

		m_windows.erase(it);
		m_removedWindowHandles.push_back(winHdl);
	}

	ykl::STLUniquePtr<VMWindow> ViewModel::GetWindow(WindowHandle winHdl) const
	{
		auto it = m_windows.find(winHdl);
		if(it == m_windows.end()) return nullptr;

		return it->second->clone(IAllocatorPtr());
	}

	ViewModel::WindowHandle ViewModel::ConsumeNewWindow() const
	{
		if(m_newWindowHandles.empty()) return -1;
		auto h = m_newWindowHandles.front();
		m_newWindowHandles.pop_front();
		return h;
	}

	ViewModel::WindowHandle ViewModel::ConsumeRemovedWindow() const
	{
		if(m_removedWindowHandles.empty()) return -1;
		auto h = m_removedWindowHandles.front();
		m_removedWindowHandles.pop_front();
		return h;
	}

}
