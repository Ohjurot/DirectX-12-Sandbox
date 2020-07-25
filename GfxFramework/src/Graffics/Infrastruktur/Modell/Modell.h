#pragma once

#include <Windows.h>
#include <d3d12.h>
#include <Graffics/DirectXContext/d3d_Device.h>
#include <Graffics/Infrastruktur/Modell/DataBufferGPU.h>

namespace IF3D12 {
	class IInstance {
		public:
			virtual void* getMemoryPointer() = 0;
			virtual UINT getSize() = 0;
	};

	template<class T>
	class Instance : public IF3D12::IInstance{
		public:
			Instance() {};
			Instance(T x) : m_data(x){};
			Instance(void* d) {
				memcpy(&m_data, d, sizeof(T));
			}

			T& get() {
				return m_data;
			}

			void* getMemoryPointer() {
				return (void*)&m_data;
			}

			INT getSize() {
				return sizeof(T);
			}
		private:
			T m_data;
	};
	
	class IModell {
		public:
			virtual IInstance createInstance(void* ptrArgs) = 0;
			virtual void getVertexData(D3D12_VERTEX_BUFFER_VIEW* ptrVertexView, UINT* ptrVertexCount) = 0;
			virtual void getIndexData(D3D12_INDEX_BUFFER_VIEW* ptrIndexView, UINT* ptrIndexCount) = 0;

			void bind(D3D::Device* ptrDevice);
			void draw(D3D::Device* ptrDevice, UINT numInstance, IInstance* ptrInstance, IF3D12::GPUDataBuffer* ptrGpuBuffer);
	};
}