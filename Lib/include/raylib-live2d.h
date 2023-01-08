#pragma once

#ifdef __cplusplus
extern "C" {
#endif
	typedef enum SetParameterType_t {
		SetParameterType_Set,
		SetParameterType_Add,
		SetParameterType_Multiply
	} SetParameterType;

	__declspec(dllexport) void l2dInit();

	/// <summary>
	/// 加载Live2D模型
	/// </summary>
	/// <param name="dir">模型文件夹路径，以目录分隔符结尾</param>
	/// <param name="file">.model3.json文件名，不用包含目录</param>
	/// <returns>模型数据的指针</returns>
	__declspec(dllexport) void* l2dLoadModel(const char* dir, const char* file);

	__declspec(dllexport) void l2dSetModelWidth(void* model1, float v);

	__declspec(dllexport) void l2dSetModelHeight(void* model1, float v);

	__declspec(dllexport) void l2dSetModelX(void* model1, float v);

	__declspec(dllexport) void l2dSetModelY(void* model1, float v);

	__declspec(dllexport) void l2dUpdate();

	__declspec(dllexport) void l2dPreUpdateModel(void* model);

	__declspec(dllexport) void l2dUpdateModel(void* model);

	__declspec(dllexport) void l2dDrawModel(void* model);

	__declspec(dllexport) int l2dHitTest(void* data, const char* name, float x, float y);

	__declspec(dllexport) void l2dSetExpression(void* data, const char* expid);

	__declspec(dllexport) void l2dSetMotion(void* data, const char* group, int no, int priority);

	__declspec(dllexport) const void* l2dGetParameterId(const char* name);

	__declspec(dllexport) void l2dSetParameter(void* data, const void* id, SetParameterType type, float value, float weight);
#ifdef __cplusplus
}
#endif