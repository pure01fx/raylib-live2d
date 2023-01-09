#pragma once
#include <array>
#include <vector>
#include <deque>

struct CubismRenderBufferSet
{
	unsigned int vertexArrayObjectId;
	unsigned int vertexBufferPositionId;
	unsigned int vertexBufferTexcoordId;
	unsigned int vertexBufferElementId;

	int indexCount;
};

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

class RenderBufferManager
{
public:
	RenderBufferManager();

	void ClearBatch();

	void CreateBuffer(int shaderId, const CubismShaderSet*& shaderSet, CubismRenderBufferSet*& bufferSet);

	void BeginApplyBatch();

	void ManuallyApplyBuffer();

	void EndApplyBatch();

	~RenderBufferManager();

private:

	void ApplyBuffer(const CubismRenderBufferSet& buffer);

	typedef std::deque<CubismRenderBufferSet> BufferSetList;
	typedef std::vector<CubismRenderBufferSet*> Batch;

	Batch m_batch;
	Batch::iterator m_applyingBatch;

	std::array<BufferSetList, CubismShaderNames_CountFlag> m_pool;
	std::array<BufferSetList::iterator, CubismShaderNames_CountFlag> m_poolTail;
};
