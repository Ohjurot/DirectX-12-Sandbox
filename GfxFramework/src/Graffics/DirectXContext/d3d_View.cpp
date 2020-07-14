#include "d3d_View.h"

D3D::View::View(UINT width, UINT height){
	// Set view port
	m_viewPort.TopLeftX = 0.0f;
	m_viewPort.TopLeftY = 0.0f;
	m_viewPort.Width = (FLOAT)width;
	m_viewPort.Height = (FLOAT)height;
	m_viewPort.MinDepth = 0.0f;
	m_viewPort.MaxDepth = 1.0f;

	// Set rect
	m_sicRect = { 0, 0, (LONG)width, (LONG)height };
}

VOID D3D::View::bind(D3D::Device* ptrD3DDevice){
	// Bind viewport and rect
	ptrD3DDevice->getCommandList()->RSSetViewports(1, &m_viewPort);
	ptrD3DDevice->getCommandList()->RSSetScissorRects(1, &m_sicRect);
}
