#pragma once
#include <array>

struct CubismShaderSet
{
	unsigned int shaderProgram;
	int attribPosition;
	int attribTexCoord;
	int samplerTexture0;
	int samplerTexture1;
	int uniformMatrix;
	int uniformClipMatrix;
	int unifromChannelFlag;
	int uniformBaseColor;
	int uniformMultiplyColor;
	int uniformScreenColor;

	unsigned int vertexArrayObjectId;
	unsigned int vertexBufferPositionId;
	unsigned int vertexBufferTexcoordId;
	unsigned int vertexBufferElementId;
};

enum CubismShaderNames
{
	CubismShaderNames_SetupMask,
	CubismShaderNames_Normal,
	CubismShaderNames_Masked,
	CubismShaderNames_MaskedInverted,
	CubismShaderNames_PremultipliedAlpha,
	CubismShaderNames_MaskedPremultipliedAlpha,
	CubismShaderNames_MaskedInvertedPremultipliedAlpha,
	CubismShaderNames_CountFlag
};

class CubismShader_Raylib
{
public:
	static CubismShader_Raylib* GetInstance();

	const CubismShaderSet* GetShader(int id);

private:
	static CubismShader_Raylib* m_instance;

	std::array<CubismShaderSet, CubismShaderNames_CountFlag> m_shaders; // I really don't know why there're 19 shader sets in the official OpenGL renderer...

	void LoadShaders();
};
