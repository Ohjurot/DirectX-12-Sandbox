#define MyRS "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), " \
			 "CBV(b0, visibility = SHADER_VISIBILITY_VERTEX), " \
			 "DescriptorTable(SRV(t0), visibility = SHADER_VISIBILITY_PIXEL), " \
			 "StaticSampler(s0, visibility = SHADER_VISIBILITY_PIXEL)"