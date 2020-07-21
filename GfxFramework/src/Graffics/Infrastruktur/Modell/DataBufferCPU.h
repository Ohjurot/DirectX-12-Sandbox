#pragma once

#include <Windows.h>

namespace IF3D12 {
	class CPUDataBuffer {
		public:
			CPUDataBuffer() {};
			CPUDataBuffer(UINT size);
			CPUDataBuffer(void* refPtr, UINT size);
			CPUDataBuffer(const CPUDataBuffer& buffer);
			~CPUDataBuffer();

			void operator=(const CPUDataBuffer &buffer);

			void dropAndResize(UINT size);
			void drop();

			UINT getSize();
			void* getBuffer();

			static HRESULT fromFile(LPCWSTR fileName, IF3D12::CPUDataBuffer* ptrBuffer);
			static void fromMemory(void* pMem, SIZE_T size, IF3D12::CPUDataBuffer* ptrBuffer);
		private:
			BOOL m_bReference = FALSE;
			UINT m_uiSize = 0;
			void* m_ptrBuffer = NULL;
	};
}