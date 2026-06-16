#include "Shader.h"
#include "MiscUtility.h"
#include "KamataEngine.h"
#include <cassert>       // assert
#include <d3dcompiler.h> // D3DCompileFromFile
#include <dxcapi.h>
#pragma comment(lib, "dxcompiler.lib")

// シェーダーファイルを読み込み、コンパイルする
void Shader::Load(const std::wstring& filePath, const std::wstring& shaderModel) {
	ID3DBlob* shaderBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	// 1. wstring => string 文字列変換を行う
	std::string mbShaderModel = ConvertString(shaderModel);

	HRESULT hr = D3DCompileFromFile(
	    filePath.c_str(), // シェーダーファイル名
	    nullptr,
	    D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
	    "main",mbShaderModel.c_str(),                           // 2. 変換した string の .c_str() を渡す
	    D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
	    0, &shaderBlob, &errorBlob);

	// エラーが発生した場合、止める
	if (FAILED(hr)) {
		if (errorBlob) {
			OutputDebugStringA(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
			errorBlob->Release();
		}

		assert(false);
	}

	// 生成したshaderBlobをとっておく
	blob_ = shaderBlob;
}

// シェーダーファイルを読み込み、コンパイルする
// ※外部コンパイラ版 シェーダーモデル6.0以上で利用する
void Shader::LoadDxc(const std::wstring& filePath, const std::wstring& shaderModel) {
	// DXC(DirectX Shader Compiler)を初期化
	static IDxcUtils* dxcUtils = nullptr;
	static IDxcCompiler3* dxcCompiler = nullptr;
	static IDxcIncludeHandler* includeHandler = nullptr;

	HRESULT hr;

	if (dxcUtils == nullptr) {
		hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
		assert(SUCCEEDED(hr));
	}

	if (dxcCompiler == nullptr) {
		hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
		assert(SUCCEEDED(hr));
	}

	if (includeHandler == nullptr) {
		hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
		assert(SUCCEEDED(hr));
	}

	//==================================================
	// 1. hlslファイルを読む
	//==================================================

	IDxcBlobEncoding* shaderSource = nullptr;

	hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);

	assert(SUCCEEDED(hr));

	// 読み込んだファイルの内容をDxcBufferに設定する
	DxcBuffer shaderSourceBuffer{};

	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();

	shaderSourceBuffer.Size = shaderSource->GetBufferSize();

	shaderSourceBuffer.Encoding = DXC_CP_UTF8;

	//==================================================
	// 2. Compileする
	//==================================================

	LPCWSTR arguments[] = {

	    filePath.c_str(), // コンパイル対象のhlslファイル名

	    L"-E",
	    L"main", // エントリーポイント

	    L"-T",
	    shaderModel.c_str(), // ShaderProfile

	    L"-Zi",
	    L"-Qembed_debug", // デバッグ情報埋め込み

	    L"-Od", // 最適化OFF

	    L"-Zpr", // 行優先レイアウト
	};

	IDxcResult* shaderResult = nullptr;

	hr = dxcCompiler->Compile(
	    &shaderSourceBuffer,        // 読み込んだファイル
	    arguments,                  // コンパイルオプション
	    _countof(arguments),        // オプション数
	    includeHandler,             // include用
	    IID_PPV_ARGS(&shaderResult) // コンパイル結果
	);

	// dxc自体が起動できないなど致命的エラー
	assert(SUCCEEDED(hr));

	//==================================================
	// 3. 警告・エラー確認
	//==================================================

	IDxcBlobUtf8* shaderError = nullptr;
	IDxcBlobWide* nameBlob = nullptr;

	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), &nameBlob);

	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		OutputDebugStringA(shaderError->GetStringPointer());

		assert(false);
	}

	//==================================================
	// 4. Compile結果取得
	//==================================================

	IDxcBlob* shaderBlob = nullptr;

	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), &nameBlob);

	assert(SUCCEEDED(hr));

	//==================================================
	// 不要なリソース解放
	//==================================================

	shaderSource->Release();
	shaderResult->Release();

	//==================================================
	// 実行用バイナリ保存
	//==================================================

	dxcBlob_ = shaderBlob;
}

// コンパイル済みのシェーダーデータを返す
// ※未コンパイルの場合は nullptr となる
ID3DBlob* Shader::GetBlob() { return blob_; }
IDxcBlob* Shader::GetDxcBlob() { return dxcBlob_; }

// コンストラクタ
Shader::Shader() {}

// デストラクタ
Shader::~Shader() {
	if (blob_ != nullptr) {
		blob_->Release();
		blob_ = nullptr;
	}
	if (dxcBlob_ != nullptr) {
		dxcBlob_->Release();
		dxcBlob_ = nullptr;
	}
}
