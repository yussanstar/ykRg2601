
#include "view_model/vm_window.h"

namespace rg::adaptor::ViewModel
{
	VMWindow::VMWindow(int x, int y, int w, int h)
		: x(x), y(y), w(w), h(h)
	{
	}

	ykl::STLUniquePtr<VMWindow> VMWindow::clone(const IAllocatorPtr& allocator) const
	{
		return ykl::STLMakeUnique<VMWindow>(allocator, x, y, w, h);
	}
}
