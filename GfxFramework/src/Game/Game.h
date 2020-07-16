#pragma once

#include <Windows.h>
#include <d3d12.h>
#include <dxgi.h>
#include <d3dcompiler.h>

#include <Application/App.h>
#include <Graffics/DirectXContext/d3d_Device.h>
#include <Graffics/DirectXContext/d3d_SwapChain.h>
#include <Graffics/DirectXContext/d3d_View.h>

namespace MY {
	class Game : public Application::GfxApp {
		public:
			struct Vertex {
				float posX, posY, posZ;
			};
			
			HRESULT Init(WF::Window* ptrAppWindow, UINT width, UINT height);
			HRESULT Loop();
			HRESULT Shutdown();

		private:
			// Test
			ID3D12PipelineState* m_ptrPso = NULL;

			// Shader
			ID3DBlob* m_ptrBlbVertex = NULL;
			ID3DBlob* m_ptrBlbPixel = NULL;
			ID3D12RootSignature* m_ptrRootSig = NULL;

			// Vertex buffer
			const Vertex c_cpuBuffer[3] = {
				{0.0f, 0.5f, 0.0f},
				{-0.5f, -0.5f, 0.0f},
				{0.5f, -0.5f, 0.0f},
			};
			ID3D12Resource* m_ptrVertexBuffer = NULL;
			D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
			const D3D12_INPUT_ELEMENT_DESC m_inputDescVertex[2] = {
				// SemanticName,	Semantic Index,	Format,							InputSlot,	ByteOffset,	InputSlotClass,								InstanceDataStepRate
				{"POSITION",		0,				DXGI_FORMAT_R32G32B32_FLOAT,	0,			0,			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
			};

			// D3D Objects
			WF::Window* m_ptrWindow;

			D3D::Device* m_ptrDevice;
			D3D::SwapChain* m_ptrSwapChain;
			D3D::View* m_ptrView;
	};
}