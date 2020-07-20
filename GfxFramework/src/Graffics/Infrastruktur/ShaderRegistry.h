#pragma once

#include <Windows.h>

#include <vector>

namespace IF3D12 {
	// Shader description
	struct Shader {
		LPWSTR name;
		SIZE_T size;
		INT id;
		void* ptrData;
	};

	class ShaderRegistry {
		public:
			INT getShader(LPCWSTR name);
			BOOL getShader(INT id, IF3D12::Shader** ppShader);
			static ShaderRegistry* getRegistry();

		private:
			ShaderRegistry();
			~ShaderRegistry();
			std::vector<Shader> m_vecShaders;

			static ShaderRegistry s_registry;
	};
}