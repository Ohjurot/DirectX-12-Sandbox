#include "ShaderRegistry.h"

IF3D12::ShaderRegistry IF3D12::ShaderRegistry::s_registry;

INT IF3D12::ShaderRegistry::getShader(LPCWSTR name){
	for (auto it = m_vecShaders.begin(); it != m_vecShaders.end(); it++) {
		if (wcscmp((*it).name, name) == 0) {
			return (*it).id;
		}
	}
	
	return -1;
}

BOOL IF3D12::ShaderRegistry::getShader(INT id, IF3D12::Shader** ppShader){
	// Check bounds
	if (id < 0 || id >= m_vecShaders.size()) {
		return FALSE;
	}
	
	// Set shader and return true
	*ppShader = &m_vecShaders.at(id);
	return TRUE;
}

IF3D12::ShaderRegistry* IF3D12::ShaderRegistry::getRegistry(){
	// Return sigelton
	return &s_registry;
}

IF3D12::ShaderRegistry::ShaderRegistry() {
	WIN32_FIND_DATA fd;
	INT id = 0;
	HANDLE find = FindFirstFile(L".\\shaders\\*", &fd);

	// Check for error
	if (find == INVALID_HANDLE_VALUE) {
		return;
	}

	// Check all files
	do {
		// Check if is file
		if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			// Read size
			LARGE_INTEGER fileSize;
			fileSize.LowPart = fd.nFileSizeLow;
			fileSize.HighPart = fd.nFileSizeHigh;

			// Create entry
			IF3D12::Shader shader = {};

			// Copy name
			shader.name = (LPWSTR)malloc(sizeof(WCHAR) * (wcslen(fd.cFileName) + 1));
			memcpy(shader.name, fd.cFileName, sizeof(WCHAR) * (wcslen(fd.cFileName) + 1));

			// Copy data		
			WCHAR buffer[MAX_PATH];
			buffer[0] = 0x00;

			wcscat_s(&buffer[0], MAX_PATH, L".\\shaders\\");
			wcscat_s(&buffer[9], MAX_PATH - 9, fd.cFileName);
			HANDLE file = CreateFile(buffer, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			
			if (file != INVALID_HANDLE_VALUE) {
				shader.ptrData = malloc(fileSize.QuadPart);
				ReadFile(file, shader.ptrData, fileSize.LowPart, NULL, NULL);
				CloseHandle(file);
			}

			// Size
			shader.size = fileSize.QuadPart;
			shader.id = id;

			// Push Back
			m_vecShaders.push_back(shader);
			id++;
		}
		
		
	} while (FindNextFile(find, &fd));

	FindClose(find);
}

IF3D12::ShaderRegistry::~ShaderRegistry(){
	for (auto it = m_vecShaders.begin(); it != m_vecShaders.end(); it++) {
		free((*it).ptrData);
		free((*it).name);
	}
}

