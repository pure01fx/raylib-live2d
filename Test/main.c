#include "raylib.h"
#include "raylib-live2d.h"

int main(void)
{
	InitWindow(1024, 768, "Live2D Test");
	SetTargetFPS(60);
	l2dInit();

	void* model = l2dLoadModel("Resources/Mao/", "Mao.model3.json");
	l2dSetModelHeight(model, 600);
	l2dSetModelX(model, 512);
	l2dSetModelY(model, 384);

	void* paramAngleX = l2dGetParameterId("ParamAngleX");
	void* paramAngleY = l2dGetParameterId("ParamAngleY");
	void* paramAngleZ = l2dGetParameterId("ParamAngleZ");
	void* paramBodyAngleX = l2dGetParameterId("ParamBodyAngleX");
	void* paramEyeBallX = l2dGetParameterId("ParamEyeBallX");
	void* paramEyeBallY = l2dGetParameterId("ParamEyeBallY");

	while (!WindowShouldClose()) {
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && l2dHitTest(model, "Head", GetMouseX(), GetMouseY()))
		{
			l2dSetMotion(model, "TapBody", 0, 2);
		}

		l2dUpdate();
		l2dPreUpdateModel(model);

		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
		{
			// There's a drag manager in the official demo. So you may feel strange here.

			float ndcX = ((float)GetMouseX()) / GetScreenWidth() * 2 - 1;
			float ndcY = 1 - ((float)GetMouseY()) / GetScreenHeight() * 2;

			// angle [-30, 30]
			l2dSetParameter(model, paramAngleX, SetParameterType_Add, ndcX * 30, 1);
			l2dSetParameter(model, paramAngleY, SetParameterType_Add, ndcY * 30, 1);
			l2dSetParameter(model, paramAngleY, SetParameterType_Add, ndcX * ndcY * -30, 1);

			// bodyAngle [-10, 10]
			l2dSetParameter(model, paramBodyAngleX, SetParameterType_Add, ndcX * 10, 1);

			// eyeball [-1, 1]
			l2dSetParameter(model, paramEyeBallX, SetParameterType_Add, ndcX, 1);
			l2dSetParameter(model, paramEyeBallY, SetParameterType_Add, ndcY, 1);
		}

		l2dUpdateModel(model);

		BeginDrawing();
			ClearBackground(RAYWHITE);

			l2dDrawModel(model);

			DrawFPS(100, 100);
		EndDrawing();
	}
}
