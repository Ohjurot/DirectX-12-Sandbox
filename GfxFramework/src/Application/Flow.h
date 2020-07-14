#pragma once

#include <Windows.h>
#include <Application/App.h>

#include <WindowFramework/Window.h>

namespace Application {
	struct Meta {
		LPCWSTR name;
		UINT clientWidth;
		UINT clientHeight;
		GfxApp* entryPoint;
	};

	class Flow {
		public:
			static HRESULT Begin(Meta* ptrAppMeta);
	};
}