#include "raylib-live2d.h"
#include "LAppAllocator.hpp"
#include "LAppDefine.hpp"
#include "LAppModel.hpp"
#include "LAppPal.hpp"
#include "raylib.h"
#include "rlgl.h"
#include <Math/CubismMatrix44.hpp>
#include <Math/CubismModelMatrix.hpp>
#include <Math/CubismViewMatrix.hpp>
#include <Id/CubismId.hpp>
#include <Id/CubismIdManager.hpp>
#include <CubismFramework.hpp>

using namespace Csm;
using namespace LAppDefine;

Csm::CubismMatrix44 projection;

extern "C" {
	void l2dInit() {
		auto cubismAllocator = new LAppAllocator();
		auto cubismOption = new CubismFramework::Option();

		cubismOption->LogFunction = LAppPal::PrintMessage;
		cubismOption->LoggingLevel = LAppDefine::CubismLoggingLevel;
		CubismFramework::StartUp(cubismAllocator, cubismOption);
		CubismFramework::Initialize();
		LAppPal::UpdateTime();
	}

	void* l2dLoadModel(const char* dir, const char* filename) {
		auto model = new LAppModel();
		model->Initialize(dir, filename);
		return model;
	}

	void l2dSetModelWidth(void* model1, float v)
	{
		LAppModel* model = reinterpret_cast<LAppModel*>(model1);
		model->GetModelMatrix()->SetWidth(v);
	}

	void l2dSetModelHeight(void* model1, float v)
	{
		LAppModel* model = reinterpret_cast<LAppModel*>(model1);
		model->GetModelMatrix()->SetHeight(v);
	}

	void l2dSetModelX(void* model1, float v)
	{
		LAppModel* model = reinterpret_cast<LAppModel*>(model1);
		model->GetModelMatrix()->SetX(v);
	}

	void l2dSetModelY(void* model1, float v)
	{
		LAppModel* model = reinterpret_cast<LAppModel*>(model1);
		model->GetModelMatrix()->SetY(GetScreenHeight() - v);
	}

	void l2dUpdate(void) {
		rlDrawRenderBatchActive(); // end previous draw
		LAppPal::UpdateTime();
		projection.LoadIdentity();
		projection.TranslateRelative(-1, -1);
		projection.ScaleRelative(2.0f / GetScreenWidth(), 2.0f / GetScreenHeight());
	}

	void l2dPreUpdateModel(void* model1) {
		LAppModel* model = reinterpret_cast<LAppModel*>(model1);
		model->PreUpdate();
	}

	void l2dUpdateModel(void* model1) {
		LAppModel* model = reinterpret_cast<LAppModel*>(model1);
		model->Update();
	}

	void l2dDrawModel(void* model1) {
		LAppModel* model = reinterpret_cast<LAppModel*>(model1);
		model->Draw(projection);
		EndBlendMode();
	}

	int l2dHitTest(void* model1, const char* name, float x, float y) {
		LAppModel* model = reinterpret_cast<LAppModel*>(model1);
		return model->HitTest(name, x, GetScreenHeight() - y);
	}

	void l2dSetExpression(void* model1, const char* expid) {
		LAppModel* model = reinterpret_cast<LAppModel*>(model1);
		model->SetExpression(expid);
	}

	void l2dSetMotion(void* model1, const char* group, int no, int priority) {
		LAppModel* model = reinterpret_cast<LAppModel*>(model1);
		model->StartMotion(group, no, priority);
	}

	const void* l2dGetParameterId(const char* name) {
		return CubismFramework::GetIdManager()->GetId(name);
	}

	void l2dSetParameter(void* model1, const void* id, SetParameterType type, float value, float weight) {
		LAppModel* model = reinterpret_cast<LAppModel*>(model1);
		switch (type) {
		case SetParameterType_Set:
			model->GetModel()->SetParameterValue(static_cast<const CubismId*>(id), value, weight);
			break;
		case SetParameterType_Add:
			model->GetModel()->AddParameterValue(static_cast<const CubismId*>(id), value, weight);
			break;
		case SetParameterType_Multiply:
			model->GetModel()->MultiplyParameterValue(static_cast<const CubismId*>(id), value, weight);
			break;
		}
	}
}
