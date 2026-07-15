#include "SceneParticleSystem.h"
#include "Geometory.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

void SceneParticleSystem::Init()
{	
	// エミッター初期化
	Texture* p_tex = CreateObj<Texture>("ParticleTex");
	p_tex->Create("Assets/Texture/particle.png");


	// シェーダーの読み込み処理
	Shader* shader[] = {
		CreateObj<VertexShader>("VS_Object"),
		CreateObj<PixelShader>("PS_TexColor"),
	};
	const char* file[] = {
		"Assets/Shader/VS_Object.cso",
		"Assets/Shader/PS_TexColor.cso",
	};
	int shaderNum = _countof(shader);
	for (int i = 0; i < shaderNum; ++i)
	{
		if (FAILED(shader[i]->Load(file[i])))
		{
			MessageBox(NULL, file[i], "Shader Error", MB_OK);
		}
	}

	// モデルの読み込み
	CreateObj<Model>("Noob");
	GetObj<Model>("Noob")->Load("Assets/Model/noob/source/noob.obj", 1.0f, true, true);

	// テクスチャ設定
	m_emitter.SetTexture(p_tex);

	// 色設定
	m_emitter.color_start = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
	m_emitter.color_end = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
	m_emitter.colorChangeStartRate = 0.8f;

	// 投射角度設定
	m_emitter.deg_min = 60.0f;
	m_emitter.deg_max = 70.0f;

	// 重力設定
	m_emitter.gravity = 9.8;

	// 生存時間設定
	m_emitter.lifeTime_min = 1.5f;
	m_emitter.lifeTime_max = 2.0f;

	// 投射ベクトル設定(水平速度min/max, 垂直速度min/max)
	m_emitter.SetVector(0.0f, 10.0f, 13.0f, 15.0f);

	// 生成数設定
	m_emitter.SetGeneratNum(1, 30);

	// 生成間隔設定
	m_emitter.SetGeneratTime(0.05f);
}
void SceneParticleSystem::Uninit()
{

}
void SceneParticleSystem::Update(float tick)
{
	m_time += tick;
	
	m_emitter.Update(tick);
}
void SceneParticleSystem::Draw()
{
	m_emitter.Drow();

	Model* p_noob = GetObj<Model>("Noob");
	if (p_noob)
	{
		// Model::Draw() はカメラ/ライト/ワールド行列を外から受け取る構造ではないので、
		// 既存の SceneVisual / SceneAnimation と同じく描画前に各定数バッファを準備します。
		CameraBase* pCamera = GetObj<CameraBase>("Camera");
		LightBase* pLight = GetObj<LightBase>("Light");
		if (pCamera && pLight)
		{
			DirectX::XMFLOAT4X4 mat[3];
			DirectX::XMStoreFloat4x4(&mat[0], DirectX::XMMatrixTranspose(
				DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f) *
				DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f)
			));
			mat[1] = pCamera->GetView();
			mat[2] = pCamera->GetProj();

			DirectX::XMFLOAT3 lightDir = pLight->GetDirection();
			DirectX::XMFLOAT4 light[] = {
				pLight->GetDiffuse(),
				pLight->GetAmbient(),
				{lightDir.x, lightDir.y, lightDir.z, 0.0f}
			};

			Shader* shader[] = {
				GetObj<Shader>("VS_Object"),
				GetObj<Shader>("PS_TexColor"),
			};

			if (shader[0] && shader[1])
			{
				shader[0]->WriteBuffer(0, mat);
				shader[0]->WriteBuffer(1, light);
				shader[1]->WriteBuffer(0, light);
				shader[1]->WriteBuffer(1, &pCamera->GetPos());

				p_noob->SetVertexShader(shader[0]);
				p_noob->SetPixelShader(shader[1]);
				p_noob->Draw();
			}
		}
	}
}
