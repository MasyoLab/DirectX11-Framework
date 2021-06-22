//==========================================================================
// メイン [main.cpp]
// author : MasyoLab
//==========================================================================
#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#define _GLIBCXX_DEBUG
#include "Source/Common/DirectX11.h"
#include "Source/Common/Function.h"
#include "Source/Common/System/mdx_imgui.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(WindowResizeFunctions, UINT);

constexpr float g_AspectX = 16.0f;
constexpr float g_AspectY = 9.0f;
constexpr DWORD g_WindowStyle = (WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX));
constexpr long long g_Fps = 60;
constexpr long long g_TimePerFrame = (long long)1000 / g_Fps;
WindowResizeFunctions g_WindowResizeFunctions;

void CheckSize(WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (mdx_imgui::WndProcHandler(hWnd, message, wParam, lParam))
		return true;

	switch (message)
	{
	case WM_SIZE:
		if (wParam != SIZE_MINIMIZED)
			g_WindowResizeFunctions.Broadcast(lParam);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SIZING:
		CheckSize(wParam, lParam);
		return TRUE;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			if (MessageBox(hWnd, "終了しますか？", "終了メッセージ", MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON1) == IDYES)
				PostQuitMessage(0);
		break;
	case WM_LBUTTONDOWN:
		SetFocus(hWnd);
		break;
	case WM_CLOSE:
		if (MessageBox(hWnd, "終了しますか？", "終了メッセージ", MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON1) == IDYES)
			PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	int width = 1270;
	int height = 714;
	MSG msg = { 0 };
	WNDCLASSEX wcex = {
		sizeof(WNDCLASSEX),				// 構造体のサイズ
		CS_VREDRAW | CS_HREDRAW,		// ウインドウスタイル
		WndProc,						// そのウインドウのメッセージを処理するコールバック関数へのポインタ
		0L,								// ウインドウクラス構造体の後ろに割り当てる補足バイト数．普通0．
		0L,								// ウインドウインスタンスの後ろに割り当てる補足バイト数．普通0．
		hInstance, 						// このクラスのためのウインドウプロシージャがあるインスタンスハン
		LoadIcon(hInstance, nullptr), 	// アイコンのハンドル
		LoadCursor(nullptr, IDC_ARROW), // マウスカーソルのハンドル．LoadCursor(nullptr, IDC_ARROW )とか
		(HBRUSH)(COLOR_WINDOW + 1), 	// ウインドウ背景色(背景描画用ブラシのハンドル)．
		nullptr,						// デフォルトメニュー名(MAKEINTRESOURCE(メニューID))
		"DirectX11",					// このウインドウクラスにつける名前
		LoadIcon(hInstance, nullptr)	// 16×16の小さいサイズのアイコン
	};

#if defined(_DEBUG)
	// コンソールデバッグ
	mdx::system::d3dx11debug::DebugConsoleBegin();
#endif

	// マネージャー
	mdx::system::DirectX11Manager manager;
	g_WindowResizeFunctions.AddDynamic1(&mdx::system::DirectX11Manager::ResizeBuffers, &manager);

	mdx_imgui::Setting();
	mdx_imgui::SetFont("Assets/Font/Meiryo/meiryo.ttc");
	manager.m_ImGuiInitializer.__Internal_AddDynamic(&mdx_imgui::Init);

	if (FAILED(manager.Init("Window", width, height, g_WindowStyle, wcex, hInstance, nCmdShow)))
		return -1;

	for (;;)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			continue;
		}

		if (!mdx::system::clock::FpsFixed())
			continue;

		mdx_imgui::NewFrame();

		{
			mdx_imgui::Begin("Window Info");
			mdx_imgui::DisplayElapsedTime();
			mdx_imgui::DisplayFramerate();
			mdx_imgui::DisplayWindowsSize();
			mdx_imgui::End();
		}

		{
			mdx_imgui::Begin(u8"ウィンドウの基本操作");
			mdx_imgui::TextWrapped(u8"ImGuiで作成されるウィンドウの基本的な操作は以下の通りです。");
			mdx_imgui::Bullet();
			mdx_imgui::TextWrapped(u8"タイトルバーをダブルクリックするとタイトルバーより下のウィンドウエリアが閉じてタイトルバーだけになります。");
			mdx_imgui::Bullet();
			mdx_imgui::TextWrapped(u8"ウィンドウの右下または左下の三角コーナーをドラッグするとウィンドウサイズを変更できます。 ダブルクリックするとウィンドウ内の内容にフィットするサイズにリサイズされます。");
			mdx_imgui::Bullet();
			mdx_imgui::TextWrapped(u8"ウィンドウ内の空白のエリアをドラッグすることでウィンドウ自体を自由に動かすことができます。");
			mdx_imgui::Bullet();
			mdx_imgui::TextWrapped(u8"マウスホイールでウィンドウ内をスクロールすることができます。");
			mdx_imgui::Bullet();
			mdx_imgui::TextWrapped(u8"ウィンドウ同士のドッキングができます。");
			mdx_imgui::End();
		}

		{
			mdx_imgui::Begin(u8"ウィンドウの作成");
			mdx_imgui::Text(u8"通常の文章");
			mdx_imgui::Text(u8"2行目", { 1.0f, 0.0f, 1.0f, 1.0f }); // 色付き文字
			mdx_imgui::BulletText(u8"白い丸が先頭に付いた文章\n\"改行\"した文章"); // \n で改行します。 " を使いたいときは \" のようにします。
			mdx_imgui::Indent();
			mdx_imgui::BulletText(u8"インデントされた文章");
			mdx_imgui::TextDisabled(u8"無効な文字"); // グレーで表示されます。
			mdx_imgui::Unindent();
			mdx_imgui::Separator();
			mdx_imgui::TextWrapped(u8"とても長い文章でもウィンドウの横幅に応じて自動的に折り返して次の行に表示してくれます");
			mdx_imgui::NewLine(); // 空白行を追加します。
			mdx_imgui::PushTextWrapPos(200.0f); // 折り返しさせる幅を指定します。
			mdx_imgui::Text(u8"PushTextWrapPosからPopTextWrapPosまでの間のTextは折り返して表示されます");
			// ... 
			// ここに追加する文章はすべて PushTextWrapPos(200.0f); の影響を受けます。
			// ...
			mdx_imgui::PopTextWrapPos();
			mdx_imgui::End();
		}

		{
			/*
			このようにしてボタンやチェックボックスを配置します。
			ボタンを押すか、もしくは、チェックボックスを押すと別のウィンドウが開くようにしたいのですが、
			実はこのままではチェックボックスをクリックしても別のウィンドウが開きません。
			ボタンのほうはクリックをするとちゃんと別のウィンドウが開きます。もしくは閉じます。

			このようにして "##1" や "##2" のような文字列を入れることで互いを区別できるようになり、
			チェックボックスをクリックしても別のウィンドウの開閉ができるようになります。
			*/
			static bool is_open = false;

			mdx_imgui::Begin(u8"ボタンとチェックボックス");
			if (mdx_imgui::Button("Open/Close##1"))
			{
				// ボタンがクリックされるとここにきます。
				is_open ^= 1;
			}

			mdx_imgui::SmallButton(u8"小さいボタン");

			mdx_imgui::SameLine(); // 次に書くUIパーツを現在と同じ行に配置します。

			// チェックボックスがクリックされるとis_openが反転し、trueならチェックマークが表示されます。
			mdx_imgui::Checkbox("Open/Close##2", is_open);
			if (is_open)
			{
				// is_openがtrueなら"別のウィンドウ"が表示されます。
				mdx_imgui::Begin(u8"別のウィンドウ", &is_open);
				mdx_imgui::Text("Hello");
				mdx_imgui::End();
			}

			/*
			ループ内で大量に同じ名前のUIパーツを作成したい場合
			*/
			for (int i = 0; i < 100; i++)
			{
				mdx_imgui::PushID(i);
				mdx_imgui::Button("Click");
				mdx_imgui::PopID();
			}
			mdx_imgui::End();
		}

		{
			mdx_imgui::Begin(u8"ラジオボタン");

			static int radio = 0;
			// ラジオボタンがクリックされると第3引数の整数が第2引数のradioに格納されます。
			mdx_imgui::RadioButton(u8"ラジオボタン 0", &radio, 0); mdx_imgui::SameLine();
			mdx_imgui::RadioButton(u8"ラジオボタン 1", &radio, 1); mdx_imgui::SameLine();
			mdx_imgui::RadioButton(u8"ラジオボタン 2", &radio, 2);

			mdx_imgui::Text(ANSI_TO_UTF8_ENCODE(mdx_imgui::MakeString("ラジオボタンは%dを選択しています", radio)));

			mdx_imgui::End();
		}

		{
			mdx_imgui::Begin(u8"長押しで急増/急減する三角矢印ボタン");

			static int counter = 0;

			mdx_imgui::PushButtonRepeat(true);

			if (mdx_imgui::ArrowButton("##left", mdx_imgui::ImGuiDir_::ImGuiDir_Left))
				counter--;
			mdx_imgui::SameLine();
			if (mdx_imgui::ArrowButton("##right", mdx_imgui::ImGuiDir_::ImGuiDir_Right))
				counter++;

			mdx_imgui::PopButtonRepeat();

			mdx_imgui::SameLine();
			mdx_imgui::Text(mdx_imgui::MakeString("%d", counter));

			mdx_imgui::End();
		}

		{
			mdx_imgui::Begin(u8"ドロップダウンリストと文章入力欄");

			{
				mdx_imgui::LabelText(u8"ラベル", u8"値");

				const char* items[] = { "AAA", "BBB", "CCC" };
				static int item_current = 0; // 0なら"AAA", 1なら"BBB", 2なら"CCC"

				mdx_imgui::Combo(u8"ドロップダウンリスト##1", &item_current, items, ((int)(sizeof(items) / sizeof(*items))));
				// 別の書き方として \0 で項目を区切って書く方法があります。
				mdx_imgui::Combo(u8"ドロップダウンリスト##2", &item_current, "AAA\0BBB\0CCC\0");

				mdx_imgui::Text(ANSI_TO_UTF8_ENCODE(mdx_imgui::MakeString("現在選択されているのは %d で、%s です", item_current, items[item_current])));

				static std::string str1, str2;
				mdx_imgui::InputText(u8"文章入力欄##1", &str1);
				mdx_imgui::InputTextWithHint(u8"文章入力欄##2", u8"空欄時に表示される文章を指定できます", &str2);

				/*
				ImGui::Comboを使うことでドロップダウンリストを作成することができます。
				文章入力欄が複数ある時は、TABキーまたはSHIFT+TABキーで入力欄のフォーカスを変更できます。
				*/
			}

			{
				static std::string text;

				auto flags = mdx_imgui::ImGuiInputTextFlags_::ImGuiInputTextFlags_AllowTabInput; // Tabキーを押すことでタブが入力されるようになります。
				// flags |= ImGuiInputTextFlags_ReadOnly; // 編集できないようにするにはこのようにします。

				// 高さが3行の文章入力欄を作成します。
				// スクロールすることで下の行も見ることができます。
				// "##"をつけていることでラベル名は表示されません。
				mdx_imgui::InputTextMultiline("##text", &text, { -FLT_MIN, mdx_imgui::GetTextLineHeight() * 3 }, flags);
			}

			{
				static char buf1[64] = "";
				mdx_imgui::InputText(u8"入力欄", buf1, 64);
				static char buf2[64] = "";
				// ImGuiInputTextFlags_CharsDecimal をつけることで 0123456789.+-*/ の文字しか入力できない入力欄になります。
				// InputTextWithHintを使うことで入力欄が空白の時にグレーで表示される文章を指定できます。
				mdx_imgui::InputTextWithHint(u8"数字入力欄", "only 0123456789.+-*/", buf2, 64, mdx_imgui::ImGuiInputTextFlags_::ImGuiInputTextFlags_CharsDecimal);

				static char bufpass[64] = "password123"; // 最初に入力しておく文字
				// ***表示のパスワード入力欄にするには ImGuiInputTextFlags_Password を使います。
				// 空白を許可しない入力欄にするには ImGuiInputTextFlags_CharsNoBlank を使います。
				mdx_imgui::InputText("Password", bufpass, 64, mdx_imgui::ImGuiInputTextFlags_::ImGuiInputTextFlags_Password | mdx_imgui::ImGuiInputTextFlags_::ImGuiInputTextFlags_CharsNoBlank);
			}

			mdx_imgui::End();
		}

		{
			mdx_imgui::Begin(u8"数字入力欄");

			static int i0 = 123;
			mdx_imgui::InputInt(u8"整数入力欄", &i0);

			static float f0 = 0.001f;
			mdx_imgui::InputFloat(u8"小数入力欄", &f0, 0.01f, 1.0f, "%.3f");

			static float vec3[3] = { 0.10f, 0.20f, 0.30f };
			mdx_imgui::InputFloat3(u8"3つの値", vec3, "%.2f");
			mdx_imgui::End();
		}

		{
			mdx_imgui::Begin(u8"ドラッグして値を増減させる(スライダー)");

			static int i1 = 50, i2 = 42;
			mdx_imgui::DragInt(u8"ドラッグして整数変更", &i1, 1);
			mdx_imgui::DragInt(u8"％表示", &i2, 1, 0, 100, "%d%%"); // 最後の引数で値の表示の仕方を指定できます。

			mdx_imgui::SetNextItemWidth(100); // 次のUIパーツの幅を100にします。

			static float f1 = 1.00f;
			mdx_imgui::DragFloat(u8"ドラッグして小数変更", &f1, 0.005f);

			// 次のUIパーツの幅をウィンドウ幅の半分にします。
			mdx_imgui::SetNextItemWidth(mdx_imgui::GetWindowWidth() * 0.5f);

			static int i3 = 0;
			// -1から3までドラッグして変化させることができます。
			mdx_imgui::SliderInt(u8"スライダーで整数変更", &i3, -1, 3);

			// 次のUIパーツの幅をウィンドウの左端から、右端から100の位置までにします。
			mdx_imgui::SetNextItemWidth(-100);

			static float f2 = 0.123f, f3 = 0.0f;
			// 0から1まで変化します。"ratio = %.3f"の部分で値の表示の仕方を指定できます。
			mdx_imgui::SliderFloat(u8"スライダーで小数変更##1", &f2, 0.0f, 1.0f, "ratio = %.3f");

			// 最後の引数を5.0fとしていることでスライダーを左右の端に近づくほど値が大きく増減するようになります。
			// 逆にスライダーが中央付近にある場合は小さな値でしか増減しないようになります。
			// -10 から 10 までドラッグして変化させることができます。
			mdx_imgui::SliderFloat(u8"スライダーで小数変更##2", &f3, -10.0f, 10.0f, "%.4f", 5.0f);

			static float angle = 0.0f;
			mdx_imgui::SliderAngle(u8"角度", &angle); // -360から360までドラッグして変化させることができます。

			static float begin = 10, end = 90;

			// 最小値0,最大値100のスライダーを2つ作成します。
			// "Min:"のスライダーは、"Max:"のスライダーの値を超えないようにスライダーで動かすことができます。
			// 0.2fはドラッグする時に値が変化する量です。2.0fにした場合は2ずつ値が変化するようになります。
			mdx_imgui::DragFloatRange2("range", &begin, &end, 0.2f, 0.0f, 100.0f, "Min: %.1f %%", "Max: %.1f %%");

			static float vec3f[4] = { 0.10f, 0.20f, 0.30f };
			// 各項目を 0 から 1 までドラッグして変化させることができます。
			mdx_imgui::SliderFloat3(u8"3つの値をスライダー操作", vec3f, 0.0f, 1.0f);

			static float x = 1.0f, y = 2.0f, z = 3.0f;
			mdx_imgui::PushItemWidth(70); // これから先のUIパーツの幅を70で固定します。

			const char* items[] = { "AAA", "BBB", "CCC", "DDD" };
			static int item = -1;
			mdx_imgui::Combo("##XYZ", &item, items, ((int)(sizeof(items) / sizeof(*items))));

			mdx_imgui::SameLine(0, 10); // 次のUIパーツを同じ行に配置し、その際、右に10だけスペースを空けます。

			mdx_imgui::SliderFloat("X", &x, 0.0f, 5.0f); mdx_imgui::SameLine();
			mdx_imgui::SliderFloat("Y", &y, 0.0f, 5.0f); mdx_imgui::SameLine();
			mdx_imgui::SliderFloat("Z", &z, 0.0f, 5.0f);

			mdx_imgui::PopItemWidth();

			mdx_imgui::End();
		}

		{
			mdx_imgui::Begin(u8"垂直スライダー");

			static float values[7] = { 0.0f, 0.60f, 0.35f, 0.9f, 0.70f, 0.20f, 0.0f };
			for (int i = 0; i < (int)(sizeof(values) / sizeof(float)); i++)
			{
				if (i > 0) mdx_imgui::SameLine();
				mdx_imgui::PushID(i);

				// 垂直スライダーを幅18,高さ160,最小値0,最大値1で作成します。
				mdx_imgui::VSliderFloat("##v", { 18, 160 }, &values[i], 0.0f, 1.0f, "");

				if (mdx_imgui::IsItemActive() || mdx_imgui::IsItemHovered())
					// スライダー上をマウスオーバーした時に現在の値がポップアップで表示されます。
					mdx_imgui::SetTooltip(mdx_imgui::MakeString("%.3f", values[i]));

				mdx_imgui::PopID();
			}
			mdx_imgui::End();
		}

		{

			/*
			色のついた四角形をクリックするとカラーピッカーで色を選択できるようになります。
			RGBのそれぞれの入力欄をドラッグすることで、RGBの値をそれぞれ変化させることができます。
			その入力欄を右クリックすることで、RGB表示ではなくHSV表示にすることもできます。
			入力欄をCTRL+クリックすることで、値をキーボードで入力する状態になります。
			*/

			mdx_imgui::Begin(u8"enum選択UIとカラー選択UI");

			enum Element { Element_Fire, Element_Earth, Element_Air, Element_Water, Element_COUNT };

			static int current_element = 0;
			auto strEnumName = NAMEOF_ENUM((Element)current_element);
			mdx_imgui::SliderInt(u8"enumの選択", &current_element, 0, Element_COUNT - 1, strEnumName.data());

			static float col1[3] = { 1.0f,0.0f,0.2f };
			static float col2[4] = { 0.4f,0.7f,0.0f,0.5f };

			mdx_imgui::ColorEdit3(u8"カラー 1", col1); // RGB
			mdx_imgui::ColorEdit4(u8"カラー 2", col2); // RGBAのアルファ付き

			mdx_imgui::ImGuiColorEditFlags_ flag = mdx_imgui::ImGuiColorEditFlags_::ImGuiColorEditFlags_Float; // 0 ~ 255表記ではなく、0.0 ~ 1.0表記にします。
			flag |= mdx_imgui::ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs; // 数字入力欄を非表示にします。
			flag |= mdx_imgui::ImGuiColorEditFlags_::ImGuiColorEditFlags_NoLabel;  // カラーボックスの右隣に配置されるラベルをなくします。
			mdx_imgui::ColorEdit3(u8"カラーID", col1, flag);
			mdx_imgui::End();
		}

		{
			mdx_imgui::Begin(u8"リストボックス");

			const char* listbox_items[] = { "Apple", "Banana", "Cherry", "Kiwi", "Mango", "Orange", "Pineapple", "Strawberry", "Watermelon" };
			static int listbox_item_current = 1;

			mdx_imgui::ListBox(u8"リストボックス", &listbox_item_current, listbox_items, ((int)sizeof(listbox_items) / sizeof(int)), 4);
			mdx_imgui::End();
		}

		{
			mdx_imgui::Begin(u8"表・テーブル");

			// 第3引数をtrueにすることで各項目の間にグレーの縦棒をつけることができます。デフォルトでtrue。
			// Columnsで作成した縦棒はドラッグすることで、各コラムの横幅を変更させることができます。
			// 縦列が3個の表を作成します。
			mdx_imgui::Columns(3, "columnID", true);
			static bool selected[12] = {};
			for (int i = 0; i < 12; i++)
			{
				// Windowsではsprintfで警告が出るのでプロジェクトのプロパティから、C/C++ -> プリプロセッサで 「_CRT_SECURE_NO_WARNINGS」 を追加します。
				char label[32]; sprintf(label, "Item %d", i);
				if (mdx_imgui::Selectable(label, &selected[i]))
				{
					// 選択された時にしたい処理をここに書きます。
				}
				mdx_imgui::NextColumn();
			}
			mdx_imgui::Columns(1); // これがないとこれ以降のUIパーツがすべてColumns項目の横幅に制限されてしまいます。
			mdx_imgui::Separator();
			// 縦列が4個の表を作成します。
			mdx_imgui::Columns(4, "columnListID");
			mdx_imgui::Separator();
			mdx_imgui::Text("ID");      mdx_imgui::NextColumn();
			mdx_imgui::Text("Name");    mdx_imgui::NextColumn();
			mdx_imgui::Text("Path");    mdx_imgui::NextColumn();
			mdx_imgui::Text("Hovered"); mdx_imgui::NextColumn();
			mdx_imgui::Separator();
			const char* names[3] = { "One", "Two", "Three" };
			const char* paths[3] = { "/path/one", "/path/two", "/path/three" };
			static int select = -1;
			for (int i = 0; i < 3; i++)
			{
				char label[32];
				sprintf(label, "%04d", i);
				// ImGuiSelectableFlags_SpanAllColumnsをつけることでコラム内の項目を選択した時に
				// その行全体を選択したような見た目になります。
				// これがない場合は、"ID"がある縦列の項目しかハイライトされません。
				if (mdx_imgui::Selectable(label, select == i, mdx_imgui::ImGuiSelectableFlags_::ImGuiSelectableFlags_SpanAllColumns))
					select = i;
				bool hovered = mdx_imgui::IsItemHovered();
				mdx_imgui::NextColumn();
				mdx_imgui::Text(mdx_imgui::MakeString("%s", names[i])); mdx_imgui::NextColumn();
				mdx_imgui::Text(mdx_imgui::MakeString("%s", paths[i])); mdx_imgui::NextColumn();
				mdx_imgui::Text(mdx_imgui::MakeString("%d", hovered));  mdx_imgui::NextColumn();
			}
			mdx_imgui::Columns(1); // 忘れずにこれをつけます。
			mdx_imgui::End();
		}

		{
			/*

			TreeNodeのif節の中の最後に必ず TreePop(); を書く必要があります。
			TreeNodeのif節の中と外ではUIパーツにつける文字列のIDは同じであっても区別されるようになっています。

			*/

			static bool bOpen2 = true;
			mdx_imgui::Begin(u8"開閉可能なフィールド");

			if (mdx_imgui::CollapsingHeader(u8"開閉可能なフィールド1"))
			{
				// このフィールドを開いている場合にしたい処理をここに書きます。
				if (mdx_imgui::TreeNode(u8"ツリーノード A"))
				{
					mdx_imgui::Text(u8"Aキーを押す");
					// "ツリーノード A"が開いている場合にしたい処理をここに書きます。
					if (mdx_imgui::IsKeyDown(mdx_imgui::GetKeyIndex(mdx_imgui::ImGuiKey_::ImGuiKey_A)))
						mdx_imgui::Text(u8"Aキーが押されている場合は表示されます");
					mdx_imgui::TreePop();
				}
				// SetNextItemOpenを使うことで次のツリーノードは最初から開いている状態になります。
				mdx_imgui::SetNextItemOpen(true, mdx_imgui::ImGuiCond_::ImGuiCond_Once);
				if (mdx_imgui::TreeNode(u8"ツリーノード B"))
				{
					mdx_imgui::Text(u8"ツリーノード Bは最初から開いた状態です");
					mdx_imgui::TreePop();
				}
			}

			if (mdx_imgui::NewTreeNode(u8"開閉可能なフィールド2", bOpen2))
			{
				mdx_imgui::Text(u8"関数二つでツリーノードを作成できます");
				mdx_imgui::EndTreeNode();
			}

			mdx_imgui::End();
		}

		{

			/*

			(?)という文字をグレーで表示し、マウスオーバー時にポップアップで文章を表示させます。
			Begin~をコールしたら、End~をコールし、Push~をコールしたらPop~をコールします。

			*/

			mdx_imgui::Begin(u8"ツールチップ");

			mdx_imgui::TextDisabled("(?)");
			if (mdx_imgui::IsItemHovered())
			{
				// "(?)"という文字をマウスオーバーしている場合にしたい処理をここに書きます。
				mdx_imgui::BeginTooltip();
				// ポップアップウィンドウの横幅をフォントサイズの35倍の長さにします。
				// これを超える文章の時は自動的にその下の行に続きます(2行になります)。
				mdx_imgui::PushTextWrapPos(mdx_imgui::GetFontSize() * 35.0f);
				mdx_imgui::Text(u8"ポップアップ時に表示される文章をここに書きます");
				mdx_imgui::PopTextWrapPos();
				mdx_imgui::EndTooltip();
			}

			mdx_imgui::End();
		}

		{
			/*
			"選択.."ボタンを押すことでポップアップが表示されます。別のどこかをクリックするか、もしくは、Escapeキーを押すことでポップアップを閉じます。
			BeginPopupではなく、 ImGui::BeginPopupModal("popupID") にした場合は、
			ポップアップ表示以外のエリアは薄く白くなり、クリックしても反応しなくなる状態になります。
			ポップアップされた中からどれかを選択しないとポップアップ表示が閉じないようになります。
			そして、ポップアップ表示はウィンドウ全体の中心に表示されます。
			例えば、ファイルを削除する際の確認メッセージ付き "OK" / "Cancel" ウィンドウなどに使います。
			*/

			mdx_imgui::Begin(u8"ポップアップウィンドウ");

			const char* names[] = { "AAA", "BBB", "CCC", "DDD" };
			static int selected = -1;
			if (mdx_imgui::Button(u8"選択.."))
			{
				// "選択.."ボタンがクリックされた場合にここにきます。
				// 引数の"popupID"と同じID名のBeginPopupをポップアップとして表示します。
				mdx_imgui::OpenPopup("popupID");
			}
			mdx_imgui::SameLine();
			mdx_imgui::TextUnformatted(selected == -1 ? "<None>" : names[selected]);

			if (mdx_imgui::BeginPopup("popupID"))
			{
				mdx_imgui::Text(u8"選択項目");
				mdx_imgui::Separator();
				for (int i = 0; i < ((int)sizeof(names) / sizeof(const char*)); i++)
					if (mdx_imgui::Selectable(names[i]))
						selected = i;
				mdx_imgui::EndPopup();
			}
			mdx_imgui::End();
		}

		{

			/*

			プロットラインもヒストグラムもプログレスバーもすべてマウスオーバーすることでその値がポップアップ表示されるようになっています。

			*/

			mdx_imgui::Begin(u8"折れ線グラフ・ヒストグラム・プログレスバー");

			static float arr[] = { 0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f };
			// 最小値 0, 最大値 1 のグラフを作成します。
			// ImVec2(0,50)としていることで、幅は自動で決まり、高さは50になります。
			mdx_imgui::PlotLines(u8"プロットライン", arr, ((int)sizeof(arr) / sizeof(float)), 0, u8"タイトル", 0.0f, 1.0f, { 0, 50 });

			mdx_imgui::PlotHistogram(u8"ヒストグラム", arr, ((int)sizeof(arr) / sizeof(float)), 0, "", 0.0f, 1.0f, { 0, 50 });

			static float progress = 0.22f;
			// 0 ~ 1 までの中での 0.22 が何％の位置にいるのかを表示します。
			mdx_imgui::ProgressBar(progress, { 0.0f, 0.0f });

			mdx_imgui::End();
		}

		{
			mdx_imgui::Begin(u8"子ウィンドウ");

			static int line = 50;
			bool goto_line = mdx_imgui::Button("Goto");
			mdx_imgui::SameLine(); // 次のUIパーツを同じ行に配置します。
			// ImGuiInputTextFlags_EnterReturnsTrue をつけることで、入力欄で文字を入力してからEnterキーを押すことでInputInt関数がtrueを返すようになります。
			// "##"をつけることでラベル名は表示されません。
			goto_line |= mdx_imgui::InputInt("##Line", &line, 0, 0, mdx_imgui::ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue);
			// "Goto"ボタンを押すかまたは入力欄で数字を入力してからEnterキーを押すことで、goto_lineはtrueになります。
			mdx_imgui::PushStyleVar(mdx_imgui::ImGuiStyleVar_::ImGuiStyleVar_ChildRounding, 5.0f); // 外枠を角丸にします。
			mdx_imgui::PushStyleColor(mdx_imgui::ImGuiCol_::ImGuiCol_ChildBg, { 0.23f, 0, 0, 0.39f }); // 子ウィンドウの背景色を変更します。
			mdx_imgui::SetNextItemWidth(100); // 次のUIパーツの幅を100にします。
			mdx_imgui::ImGuiWindowFlags_ flag = (mdx_imgui::ImGuiWindowFlags_)0;
			// flag |= ImGuiWindowFlags_NoScrollWithMouse; // 子ウィンドウの中でマウススクロールしても子ウィンドウ内はスクロールされなくなります。
			flag |= mdx_imgui::ImGuiWindowFlags_::ImGuiWindowFlags_MenuBar; // 子ウィンドウにBeginMenuBarをつけることを可能にします。
			// 第3引数をtrueにすることで外枠が表示されます。
			mdx_imgui::BeginChild("ChildID", { mdx_imgui::GetWindowContentRegionWidth() * 0.5f, 260 }, true, flag);
			if (mdx_imgui::BeginMenuBar())
			{
				if (mdx_imgui::BeginMenu("Menu"))
				{
					// "Menu"をクリックしたら開くメニュー項目を書いていきます。
					if (mdx_imgui::MenuItem("New")) {}
					// ...
					mdx_imgui::EndMenu();
				}
				mdx_imgui::EndMenuBar();
			}
			// 子ウィンドウの中に100行作成します。
			for (int i = 0; i < 100; i++)
			{
				mdx_imgui::Text(ANSI_TO_UTF8_ENCODE(mdx_imgui::MakeString("%04d: スクロール可能領域", i)));
				if (goto_line && line == i)
					// SetScrollHereYは現在の位置にスクロール位置を持ってきます。
					// "Goto"ボタンを押した時の行数の位置が子ウィンドウの真ん中の位置になるように瞬時にスクロールされます。
					mdx_imgui::SetScrollHereY();
				// mdx_imgui::SetScrollHereY(0.0f); // 子ウィンドウの一番上に来るように瞬時にスクロールされます。
				// mdx_imgui::SetScrollHereY(1.0f); // 子ウィンドウの一番下に来るように瞬時にスクロールされます。
			}
			if (goto_line && line >= 100)
				mdx_imgui::SetScrollHereY();
			mdx_imgui::EndChild();
			mdx_imgui::PopStyleColor();
			mdx_imgui::PopStyleVar();

			mdx_imgui::End();
		}

		{
			mdx_imgui::Begin(u8"タブ");

			mdx_imgui::ImGuiTabBarFlags_ flag = mdx_imgui::ImGuiTabBarFlags_::ImGuiTabBarFlags_Reorderable; // タブをドラッグして並び替えができるようになります。
			flag |= mdx_imgui::ImGuiTabBarFlags_::ImGuiTabBarFlags_TabListPopupButton; // タブの一番左端にドロップダウンリストが表示される下向き三角形のクリックエリアを作成し、そこからタブを選択できるようになります。
			flag |= mdx_imgui::ImGuiTabBarFlags_::ImGuiTabBarFlags_AutoSelectNewTabs; // タブを新しく作成した時に自動でそのタブを選択状態にします。
			// flag |= mdx_imgui::ImGuiTabBarFlags_::ImGuiTabBarFlags_NoCloseWithMiddleMouseButton; // タブの中でマウス中央ボタンクリックすることでタブを閉じることができる機能を無効にします。
			// flag |= mdx_imgui::ImGuiTabBarFlags_::ImGuiTabBarFlags_NoTooltip; // タブ上にマウスオーバーした場合に表示されるタブ名のポップアップ表示を無効にします。
			// flag |= mdx_imgui::ImGuiTabBarFlags_::ImGuiTabBarFlags_FittingPolicyResizeDown; // タブがウィンドウ幅を超えてたくさんある場合にタブの幅を自動でリサイズしてフィットさせることができます。
			flag |= mdx_imgui::ImGuiTabBarFlags_::ImGuiTabBarFlags_FittingPolicyScroll; // タブの幅を自動でリサイズさせずに左右の矢印ボタンを右端に配置してそこからタブを順番に選択できるようにします。
			const char* names[8] = { "同じタブ名##1", "同じタブ名##2", "タブ2", "タブ3", "タブ4", "タブ5", "タブ6", "タブ7" };
			// 同じタブ名を使いたい場合は、"##1" や "##2" のように区別させる必要があります。
			static bool opened[8] = { true, true, true, true, true, true, true, true };
			if (mdx_imgui::BeginTabBar("TabBarID", flag))
			{
				for (int n = 0; n < ((int)sizeof(opened) / sizeof(bool)); n++)
					// 第2引数の&opened[n]を省略すると閉じるボタン(X)が作成されません。
					if (opened[n] && mdx_imgui::BeginTabItem(ANSI_TO_UTF8_ENCODE(names[n]), opened[n]))
					{
						mdx_imgui::Text(ANSI_TO_UTF8_ENCODE(mdx_imgui::MakeString("これは %s のタブです", names[n])));
						mdx_imgui::EndTabItem();
					}
				mdx_imgui::EndTabBar();
			}
			mdx_imgui::End();
		}

		{
			/*
			メインメニューバー
			if (ImGui::BeginMainMenuBar())の中のImGui::MenuItem以下のコードはメインメニュー内だけでなく、
				ウィンドウの中やポップアップ表示の中などにも書くことができます。
			*/

			if (mdx_imgui::BeginMainMenuBar())
			{
				// メインメニューを表示している時の処理をここに書きます。
				if (mdx_imgui::BeginMenu("File"))
				{
					// "File"をクリックしてFileメニューを開いた時の処理をここに書きます。
					// bool MenuItem(const char* label, const char* shortcut = NULL,
					//               bool selected = false, bool enabled = true);
					mdx_imgui::MenuItem("(dummy menu)", nullptr, false, false); // enabledをfalseにすることでグレー表示できます。 
					if (mdx_imgui::MenuItem("New"))
					{
						// "New"がクリックされた時の処理をここに書きます。
					}
					// ショートカットキー表示は第2引数に書きます。
					if (mdx_imgui::MenuItem("Open", "Ctrl+O")) {}
					if (mdx_imgui::BeginMenu("Open Recent"))
					{
						mdx_imgui::MenuItem("fish_hat.c");
						mdx_imgui::MenuItem("fish_hat.inl");
						mdx_imgui::MenuItem("fish_hat.h");
						if (mdx_imgui::BeginMenu("More.."))
						{
							mdx_imgui::MenuItem("Hello");
							mdx_imgui::MenuItem("Sailor");
							if (mdx_imgui::BeginMenu("Recurse.."))
							{
								// ...
								mdx_imgui::EndMenu();
							}
							mdx_imgui::EndMenu();
						}
						mdx_imgui::EndMenu();
					}
					if (mdx_imgui::MenuItem("Save", "Ctrl+S")) {}
					if (mdx_imgui::MenuItem("Save As..")) {}
					mdx_imgui::Separator();
					if (mdx_imgui::BeginMenu("Options")) { mdx_imgui::EndMenu(); }
					if (mdx_imgui::BeginMenu("Colors")) { mdx_imgui::EndMenu(); }
					if (mdx_imgui::BeginMenu("Disabled", false)) { mdx_imgui::EndMenu(); }
					if (mdx_imgui::MenuItem("Checked", nullptr, true)) {} // selectedをtrueにすることでチェックマークをつけることができます
					if (mdx_imgui::MenuItem("Quit", "Alt+F4")) {}
					mdx_imgui::EndMenu();
				}
				if (mdx_imgui::BeginMenu("Edit")) { mdx_imgui::EndMenu(); }
				mdx_imgui::EndMainMenuBar();
			}
		}

		{
			/*
			ImGui::Dummyを使うことで空白エリアを作ることができます。
			*/

			mdx_imgui::Begin(u8"ダミー");

			mdx_imgui::Button("A", { 40, 40 });
			mdx_imgui::SameLine();
			mdx_imgui::Dummy({ 40, 40 }); // ダミーを使って間隔を空けることができます。
			mdx_imgui::SameLine();
			if (mdx_imgui::Button("B", { 40, 40 }))
			{
				// ボタンが押された時にしたい処理を書きます。
			}

			mdx_imgui::Button("A", { 40, 40 });
			// mdx_imgui::SameLine(float offset_from_start_x, float spacing_w)
			mdx_imgui::SameLine(0.0f, 40.0f);
			if (mdx_imgui::Button("B", { 40, 40 }))
			{
				// ボタンが押された時にしたい処理を書きます。
			}

			/*
			これだと間隔が少し狭くなりますが、ある程度同じ見た目になります。
			*/

			mdx_imgui::End();
		}

		mdx_imgui::EndFrame();

		// Draw 
		manager.DrawBegin();
		mdx_imgui::Draw();
		manager.DrawEnd();
	}

	mdx_imgui::Uninit();
#if defined(_DEBUG)
	// コンソールデバッグ終了
	mdx::system::d3dx11debug::DebugConsoleEnd();
#endif

	return 0;
}

void CheckSize(WPARAM wParam, LPARAM lParam)
{
	RECT* rc;
	int width, height;
	int minimumwidth;

	minimumwidth = GetSystemMetrics(SM_CXMINTRACK);
	rc = (RECT*)lParam;
	width = rc->right - rc->left;
	height = rc->bottom - rc->top;
	switch (wParam)
	{
	case WMSZ_TOP:
		width = (int)(height * (g_AspectX / g_AspectY));
		if (((double)width / (double)height) < (g_AspectX / g_AspectY))
			rc->right = rc->left + (int)(height * (g_AspectX / g_AspectY));
		else
			rc->top = rc->bottom - (int)(width * (g_AspectY / g_AspectX));
		break;
	case WMSZ_BOTTOM:
		width = (int)(height * (g_AspectX / g_AspectY));
		if (((double)width / (double)height) < (g_AspectX / g_AspectY))
			rc->right = rc->left + (int)(height * (g_AspectX / g_AspectY));
		else
			rc->bottom = rc->top + (int)(width * (g_AspectY / g_AspectX));
		break;
	case WMSZ_LEFT:
	case WMSZ_RIGHT:
		rc->bottom = rc->top + (int)(width * (g_AspectY / g_AspectX));
		break;
	case WMSZ_TOPLEFT:
		if (((double)width / (double)height) < (g_AspectX / g_AspectY))
			rc->left = rc->right - (int)(height * (g_AspectX / g_AspectY));
		else
			rc->top = rc->bottom - (int)(width * (g_AspectY / g_AspectX));
		break;
	case WMSZ_TOPRIGHT:
		if (((double)width / (double)height) < (g_AspectX / g_AspectY))
			rc->right = rc->left + (int)(height * (g_AspectX / g_AspectY));
		else
			rc->top = rc->bottom - (int)(width * (g_AspectY / g_AspectX));
		break;
	case WMSZ_BOTTOMLEFT:
		if (((double)width / (double)height) < (g_AspectX / g_AspectY))
			rc->left = rc->right - (int)(height * (g_AspectX / g_AspectY));
		else
			rc->bottom = rc->top + (int)(width * (g_AspectY / g_AspectX));
		break;
	case WMSZ_BOTTOMRIGHT:
		if (((double)width / (double)height) < (g_AspectX / g_AspectY))
			rc->right = rc->left + (int)(height * (g_AspectX / g_AspectY));
		else
			rc->bottom = rc->top + (int)(width * (g_AspectY / g_AspectX));
		break;
	}
}
