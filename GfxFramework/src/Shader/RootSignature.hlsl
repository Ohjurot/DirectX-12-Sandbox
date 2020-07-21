#define MyRS "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), " \
			 "CBV(b0, visibility = SHADER_VISIBILITY_VERTEX), " \
			 "DescriptorTable(SRV(t1, numDescriptors = 32), visibility = SHADER_VISIBILITY_PIXEL), " \
			 "DescriptorTable(CBV(b1, numDescriptors = 32), visibility = SHADER_VISIBILITY_VERTEX), " \
			 "StaticSampler(s0, visibility = SHADER_VISIBILITY_PIXEL)"