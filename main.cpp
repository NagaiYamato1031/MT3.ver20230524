#include <Novice.h>
#include <cstdint>
#include <cmath>
#include "imgui.h"

#include "MyConst.h"
#include "Mymath.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"

const char kWindowTitle[] = "LE2A_12_ナガイ_ヤマト_MT3_00_確認課題";
const int kWindowWidth = 1280;
const int kWindowHeight = 720;

namespace NS_Vector3 {
	const int kRowHeight = 20;
	const int kColumnWidth = 60;
}

namespace NS_Matrix4x4 {
	const int kRowHeight = 20;
	const int kColumnWidth = 68;
}


void VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label) {
	Novice::ScreenPrintf(x, y, "%0.2f", vector.x);
	Novice::ScreenPrintf(x + NS_Vector3::kColumnWidth, y, "%0.2f", vector.y);
	Novice::ScreenPrintf(x + NS_Vector3::kColumnWidth * 2, y, "%0.2f", vector.z);
	Novice::ScreenPrintf(x + NS_Vector3::kColumnWidth * 3, y, "%s", label);
}

void Matrix4x4ScreenPrintf(int x, int y, Matrix4x4 matrix, const char* label) {
	Novice::ScreenPrintf(x, y, "%s", label);
	for (int row = 0; row < 4; row++) {
		for (int column = 0; column < 4; column++) {
			Novice::ScreenPrintf(
				x + column * NS_Matrix4x4::kColumnWidth, y + (row + 1) * NS_Matrix4x4::kRowHeight, "%6.02f",
				matrix.m[row][column]);
		}
	}
}

void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	// Grid 半分の幅
	const float kGridHalfWidth = 2.0f;
	// 分割数
	const uint32_t kSubdivision = 10;
	// 1 つ分の長さ
	const float kGridEvery = (kGridHalfWidth * 2) / float(kSubdivision);
	// 奥から手前への線を引いていく
	// 今回はローカル座標で宣言
	Vector3 xOrigin{ 0.0f,0.0f,0.0f };
	Vector3 xVector{ 0.0f,0.0f,kGridHalfWidth * 2.0f };
	for (uint32_t xIndex = 0; xIndex <= kSubdivision; ++xIndex) {
		// 始点と終点
		//Vector3 xStart{ (kSubdivision / 2.0f - xIndex) * kGridEvery,0.0f,kSubdivision * kGridEvery };
		//Vector3 xEnd{ (kSubdivision / 2.0f - xIndex) * kGridEvery,0.0f,-float(kSubdivision) * kGridEvery };
		// スクリーン座標まで変換を掛ける
		//Matrix4x4 startWorldMatrix = Mymath::MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, xOrigin);
		//Matrix4x4 endWorldMatrix = Mymath::MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, xVector);
		Matrix4x4 worldMatrix = Mymath::MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, { (kSubdivision / 2.0f - xIndex) * kGridEvery,0.0f,-kGridHalfWidth });
		Matrix4x4 worldViewProjectionMatrix = Mymath::Multiply(worldMatrix, viewProjectionMatrix);

		Vector3 kLocalVertices[2]{ xOrigin,xVector };
		Vector3 screenVertices[2];
		for (uint32_t i = 0; i < 2; ++i) {
			Vector3 ndcVertex = Mymath::Transform(kLocalVertices[i], worldViewProjectionMatrix);
			screenVertices[i] = Mymath::Transform(ndcVertex, viewportMatrix);
		}
		if (xIndex != kSubdivision / 2) {
			Novice::DrawLine(int(screenVertices[0].x), int(screenVertices[0].y), int(screenVertices[1].x), int(screenVertices[1].y), 0xAAAAAAFF);
		}
		else {
			Novice::DrawLine(int(screenVertices[0].x), int(screenVertices[0].y), int(screenVertices[1].x), int(screenVertices[1].y), 0x222222FF);
		}
	}
	// 左から右への線を引いていく
	// 今回はローカル座標で宣言
	Vector3 zOrigin{ 0.0f,0.0f,0.0f };
	Vector3 zVector{ kGridHalfWidth * 2.0f,0.0f,0.0f };
	for (uint32_t zIndex = 0; zIndex <= kSubdivision; ++zIndex) {
		// スクリーン座標まで変換を掛ける
		Matrix4x4 worldMatrix = Mymath::MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, { -kGridHalfWidth,0.0f,(kSubdivision / 2.0f - zIndex) * kGridEvery });
		Matrix4x4 worldViewProjectionMatrix = Mymath::Multiply(worldMatrix, viewProjectionMatrix);

		Vector3 kLocalVertices[2]{ zOrigin,zVector };
		Vector3 screenVertices[2];
		for (uint32_t i = 0; i < 2; ++i) {
			Vector3 ndcVertex = Mymath::Transform(kLocalVertices[i], worldViewProjectionMatrix);
			screenVertices[i] = Mymath::Transform(ndcVertex, viewportMatrix);
		}
		if (zIndex != kSubdivision / 2) {
			Novice::DrawLine(int(screenVertices[0].x), int(screenVertices[0].y), int(screenVertices[1].x), int(screenVertices[1].y), 0xAAAAAAFF);
		}
		else {
			Novice::DrawLine(int(screenVertices[0].x), int(screenVertices[0].y), int(screenVertices[1].x), int(screenVertices[1].y), 0x222222FF);
		}
	}

}

// 球の表示
void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	const float pi = 22.0f / 7.0f;
	// 分割数
	const uint32_t kSubdivision = 16;
	// 経度分割 1 つ分の角度
	const float kLonEvery = 2 * pi / float(kSubdivision);
	// 緯度分割 1 つ分の角度
	const float kLatEvery = 2 * pi / float(kSubdivision);
	// 緯度の方向に分割 -π / 2 ～ π / 2
	for (uint32_t latIndex = 0; latIndex < kSubdivision; latIndex++) {
		float dTheta = pi / kSubdivision;
		// 現在の緯度
		float lat = -pi / 2.0f + kLatEvery * latIndex;
		// 経度の方向に分割 0 ～ 2 π
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; lonIndex++) {
			float dPhi = 2 * pi / kSubdivision;
			// 現在の経度
			float lon = lonIndex * kLonEvery;
			// world 座標系での a, b, c を求める
			Vector3 a, b, c;
			// a, b, c を Screen 座標系まで変換
			a = { cosf(lat) * cosf(lon),sinf(lat),cosf(lat) * sinf(lon) };
			b = { cosf(lat + dTheta) * cosf(lon),sinf(lat + dTheta),cosf(lat + dTheta) * sinf(lon) };
			c = { cosf(lat) * cosf(lon + dPhi),sinf(lat),cosf(lat) * sinf(lon + dPhi) };
			a *= sphere.radius;
			b *= sphere.radius;
			c *= sphere.radius;

			Matrix4x4 worldMatrix = Mymath::MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, sphere.center);
			Matrix4x4 worldViewProjectionMatrix = Mymath::Multiply(worldMatrix, viewProjectionMatrix);
			Vector3 scrA, scrB, scrC;
			Vector3 ndcVertex = Mymath::Transform(a, worldViewProjectionMatrix);
			scrA = Mymath::Transform(ndcVertex, viewportMatrix);
			ndcVertex = Mymath::Transform(b, worldViewProjectionMatrix);
			scrB = Mymath::Transform(ndcVertex, viewportMatrix);
			ndcVertex = Mymath::Transform(c, worldViewProjectionMatrix);
			scrC = Mymath::Transform(ndcVertex, viewportMatrix);
			// ab, bc で線を引く
			Novice::DrawLine(int(scrA.x), int(scrA.y), int(scrB.x), int(scrB.y), color);
			Novice::DrawLine(int(scrA.x), int(scrA.y), int(scrC.x), int(scrC.y), color);
			//Novice::DrawLine(int(scrB.x), int(scrB.y), int(scrC.x), int(scrC.y), color);
		}
	}
}


// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, kWindowWidth, kWindowHeight);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	// カメラ

	Vector3 kCameraStartPosition{ 0.0f,1.9f,-6.49f };
	Vector3 kCameraStartRotate{ 0.26f,0.0f,0.0f };

	Vector3 cameraPosition = kCameraStartPosition;
	Vector3 cameraRotate = kCameraStartRotate;

	// Y 軸への移動を反映させるか
	bool isEnableYMovement = false;

	// マウス座標
	Vector2 mousePosition{ 0 };
	Vector2 preMousePosition{ 0 };

	float cameraSence = 0.003f;

	// 球
	Sphere sphere1;
	sphere1.center = { 0.0f,0.0f,0.0f };
	sphere1.radius = 0.5f;
	Sphere sphere2;
	sphere2.center = { 0.5f,0.3f,0.7f };
	sphere2.radius = 0.3f;

	int color = WHITE;

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		// ImGui

#pragma region ImGui



		ImGui::Begin("Window");
		if (ImGui::Button("CameraReset")) {
			cameraPosition = kCameraStartPosition;
			cameraRotate = kCameraStartRotate;
		}
		if (ImGui::TreeNode("CameraControl")) {
			ImGui::Checkbox("isEnableYMovement", &isEnableYMovement);
			ImGui::SliderFloat("CameraSence", &cameraSence, 0.001f, 0.01f);
			ImGui::DragFloat3("CameraTraslate", &cameraPosition.x, 0.01f);
			ImGui::DragFloat3("CameraRotate", &cameraRotate.x, 0.01f);
			ImGui::TreePop();
		}
		ImGui::DragFloat3("sphere1.center", &sphere1.center.x, 0.01f);
		ImGui::DragFloat("sphere1.radius", &sphere1.radius, 0.01f);
		ImGui::DragFloat3("sphere2.center", &sphere2.center.x, 0.01f);
		ImGui::DragFloat("sphere2.radius", &sphere2.radius, 0.01f);
		ImGui::End();

#pragma endregion


#pragma region 実際の処理

		if (Mymath::IsCollision(sphere1, sphere2))
			color = RED;
		else
			color = WHITE;

#pragma endregion

		// カメラの移動

		Matrix4x4 cameraMatrix = Mymath::MakeAffineMatrix({ 1.0f,1.0f,1.0f }, cameraRotate, cameraPosition);

#pragma region カメラ操作

		const float kCameraSpeed = 0.05f;
		if (Novice::CheckHitKey(DIK_SPACE)) {
			cameraPosition.y += kCameraSpeed / 2.0f;
		}
		if (Novice::CheckHitKey(DIK_LCONTROL) || Novice::CheckHitKey(DIK_RCONTROL)) {
			cameraPosition.y -= kCameraSpeed / 2.0f;
		}
		if (Novice::CheckHitKey(DIK_W)) {
			Vector3 vertical{ cameraRotate.z,0.0f,cameraRotate.x };
			Vector3 velocity = Mymath::TransformNormal(vertical, cameraMatrix) * kCameraSpeed;
			if (!isEnableYMovement) {
				velocity.y = 0.0f;
			}
			cameraPosition = cameraPosition + velocity;
		}
		if (Novice::CheckHitKey(DIK_S)) {
			Vector3 vertical{ cameraRotate.z,0.0f,cameraRotate.x };
			Vector3 velocity = Mymath::TransformNormal(vertical, cameraMatrix) * kCameraSpeed;
			if (!isEnableYMovement) {
				velocity.y = 0.0f;
			}
			cameraPosition = cameraPosition - velocity;
		}
		if (Novice::CheckHitKey(DIK_D)) {
			Vector3 vertical{ cameraRotate.x,0.0f,cameraRotate.z };
			cameraPosition = cameraPosition + (Mymath::TransformNormal(vertical, cameraMatrix) * kCameraSpeed);
		}
		if (Novice::CheckHitKey(DIK_A)) {
			Vector3 vertical{ cameraRotate.x,0.0f,cameraRotate.z };
			cameraPosition = cameraPosition - (Mymath::TransformNormal(vertical, cameraMatrix) * kCameraSpeed);
		}
		if (Novice::IsPressMouse(2)) {
			int x, y;
			Novice::GetMousePosition(&x, &y);
			mousePosition = { (float)x,(float)y };
			Vector2 velocity = (mousePosition - preMousePosition) * cameraSence;
			cameraRotate += {velocity.y, velocity.x, 0.0f};
		}
		// ホイール入力
		{
			float wheel = Novice::GetWheel() / 5.0f;
			Vector3 vertical{ cameraRotate.z,0.0f,cameraRotate.x };
			Vector3 velocity = Mymath::TransformNormal(vertical, cameraMatrix) * kCameraSpeed;
			cameraPosition = cameraPosition + velocity * wheel;
		}

#pragma endregion


		cameraMatrix = Mymath::MakeAffineMatrix({ 1.0f,1.0f,1.0f }, cameraRotate, cameraPosition);
		Matrix4x4 viewMatrix = Mymath::Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = Mymath::MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
		// viewProjectionMatrix
		Matrix4x4 viewProjectionMatrix = Mymath::Multiply(viewMatrix, projectionMatrix);
		Matrix4x4 viewportMatrix = Mymath::MakeViewportMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);

		int x, y;
		Novice::GetMousePosition(&x, &y);
		preMousePosition = { (float)x,(float)y };

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		DrawGrid(viewProjectionMatrix, viewportMatrix);

		DrawSphere(sphere1, viewProjectionMatrix, viewportMatrix, color);
		DrawSphere(sphere2, viewProjectionMatrix, viewportMatrix, WHITE);


		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}

