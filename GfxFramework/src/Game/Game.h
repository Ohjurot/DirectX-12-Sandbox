#pragma once

#include <Windows.h>
#include <d3d12.h>
#include <dxgi.h>
#include <d3dcompiler.h>

#include <DirectXMath.h>

#include <Application/App.h>
#include <Graffics/DirectXContext/d3d_Device.h>
#include <Graffics/DirectXContext/d3d_SwapChain.h>
#include <Graffics/DirectXContext/d3d_View.h>
#include <Graffics/DirectXContext/Buffers/d3d_FixedUploadBuffer.h>
#include <Graffics/DirectXContext/Buffers/d3d_VariableUploadBuffer.h>
#include <Graffics/DirectXContext/Texture/d3d_TextureUploader.h>

#include <Loading/Blobs/d3dBlogLoader.h>
#include <Loading/Images/WIC_ImageLoader.h>

#include <Graffics/Infrastruktur/ShaderRegistry.h>
#include <Graffics/Infrastruktur/DrawDescriptor.h>
#include <Graffics/Infrastruktur/Modell/DataBufferCPU.h>
#include <Graffics/Infrastruktur/Modell/DataBufferGPU.h>
#include <Graffics/Infrastruktur/Modell/VertexLayout.h>
#include <Graffics/Infrastruktur/Modell/DataBuffer.h>
#include <Graffics/Infrastruktur/Modell/Texture.h>

namespace MY {
	class Game : public Application::GfxApp {
		public:
			// === VIDEO 4 ===
			
			// Vertex deffinition
			struct Vertex {
				float posX, posY;
				float u, v;
			};

			// CBuffer deffiniton
			struct CBuffer {
				DirectX::XMFLOAT4X4 matModell;
				DirectX::XMFLOAT4X4 matView;
				DirectX::XMFLOAT4X4 matProjection;
			};

			// ================
			
			HRESULT Init(WF::Window* ptrAppWindow, UINT width, UINT height);
			HRESULT Loop();
			HRESULT Shutdown();

		private:
			// === VIDEO 4 ===

			// Upload buffer
			D3D::FixedUploadBuffer<128> m_uploadBuffer;

			// Pipline state Object for drawing
			ID3D12PipelineState* m_ptrPso = NULL;
			
			// Root signatures
			ID3D12RootSignature* m_ptrRootSig = NULL;

			// Vertex buffer resource
			IF3D12::FDataBuffer<Vertex, 3> m_vertexBuffer;
			D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView = {};

			// Constant buffer
			IF3D12::FDataBuffer<CBuffer> m_constBuffer;
			D3D12_CONSTANT_BUFFER_VIEW_DESC m_constBufferView = {};

			// Texure
			IF3D12::Texture* m_ptrTexture = NULL;
			ID3D12DescriptorHeap* m_ptrHeapRootTable;
			
			// Vertex buffer for triangle
			const Vertex c_cpuBuffer[3] = {
				{0.0f, 0.5f,		0.5163f, 0.0716f},
				{-0.5f, -0.5f,		0.0650f, 0.8971f},
				{0.5f, -0.5f,		0.9051f, 0.8971f},
			};
			
			// Vertex input layout descriptor
			IF3D12::VertexLayout m_inputLayout;

			// =============================

			// D3D Objects
			WF::Window* m_ptrWindow;

			D3D::Device* m_ptrDevice;
			D3D::SwapChain* m_ptrSwapChain;
			D3D::View* m_ptrView;
	};
}