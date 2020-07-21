#include "DataBufferCPU.h"

IF3D12::CPUDataBuffer::CPUDataBuffer(UINT size){
	m_uiSize = size;
	m_ptrBuffer = malloc(size);
}

IF3D12::CPUDataBuffer::CPUDataBuffer(void* refPtr, UINT size){
	m_uiSize = size;
	m_ptrBuffer = refPtr;
	m_bReference = TRUE;
}

IF3D12::CPUDataBuffer::CPUDataBuffer(const CPUDataBuffer& buffer){
	// Check if ref copy
	if (buffer.m_bReference) {
		m_bReference = TRUE;
		m_ptrBuffer = buffer.m_ptrBuffer;
		m_uiSize = buffer.m_uiSize;
	}
	// Else do hard copy
	else {
		m_bReference = FALSE;
		m_uiSize = buffer.m_uiSize;
		m_ptrBuffer = malloc(m_uiSize);
		memcpy(m_ptrBuffer, buffer.m_ptrBuffer, m_uiSize);
	}
}

IF3D12::CPUDataBuffer::~CPUDataBuffer(){
	// Free buffer if exists
	drop();
}

void IF3D12::CPUDataBuffer::operator=(const CPUDataBuffer& buffer){
	// Drop self content
	drop();
	
	// Check if ref copy
	if (buffer.m_bReference) {
		m_bReference = TRUE;
		m_ptrBuffer = buffer.m_ptrBuffer;
		m_uiSize = buffer.m_uiSize;
	}
	// Else do hard copy
	else {
		m_bReference = FALSE;
		m_uiSize = buffer.m_uiSize;
		m_ptrBuffer = malloc(m_uiSize);
		memcpy(m_ptrBuffer, buffer.m_ptrBuffer, m_uiSize);
	}
}

void IF3D12::CPUDataBuffer::dropAndResize(UINT size){
	drop();
	m_uiSize = size;
	m_ptrBuffer = malloc(size);
}

void IF3D12::CPUDataBuffer::drop(){
	// Clear buffer
	if (m_ptrBuffer && !m_bReference) {
		free(m_ptrBuffer);
		m_ptrBuffer = NULL;

		// zero size
		m_uiSize = 0;
	}
}

UINT IF3D12::CPUDataBuffer::getSize(){
	return m_uiSize;
}

void* IF3D12::CPUDataBuffer::getBuffer(){
	return m_ptrBuffer;
}

HRESULT IF3D12::CPUDataBuffer::fromFile(LPCWSTR fileName, IF3D12::CPUDataBuffer* ptrBuffer){
	// Create file handle
	HANDLE file = CreateFile(fileName, GENERIC_READ, NULL, NULL, OPEN_EXISTING, NULL, NULL);
	if (file == INVALID_HANDLE_VALUE) {
		return GetLastError();
	}

	// Get file size
	LARGE_INTEGER fileSize;
	fileSize.LowPart = GetFileSize(file, (LPDWORD)&fileSize.HighPart);

	// Resize buffer
	ptrBuffer->dropAndResize(fileSize.QuadPart);

	// Read to buufer
	SIZE_T offset = 0;
	while (fileSize.QuadPart > 0) {
		// Read from file and store readed size
		DWORD read = 0;
		ReadFile(file, (void*)((SIZE_T)ptrBuffer->getBuffer() + offset), fileSize.LowPart, &read, NULL);
		
		// Sustract from total size
		fileSize.QuadPart -= read;
	}

	// Close file
	CloseHandle(file);
}

void IF3D12::CPUDataBuffer::fromMemory(void* pMem, SIZE_T size, IF3D12::CPUDataBuffer* ptrBuffer){
	// Resize buffer
	ptrBuffer->dropAndResize(size);

	// Copy memory
	memcpy(ptrBuffer->getBuffer(), pMem, size);
}
