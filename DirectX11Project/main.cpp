//==========================================================================
// ���C�� [main.cpp]
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
			if (MessageBox(hWnd, "�I�����܂����H", "�I�����b�Z�[�W", MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON1) == IDYES)
				PostQuitMessage(0);
		break;
	case WM_LBUTTONDOWN:
		SetFocus(hWnd);
		break;
	case WM_CLOSE:
		if (MessageBox(hWnd, "�I�����܂����H", "�I�����b�Z�[�W", MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON1) == IDYES)
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
		sizeof(WNDCLASSEX),				// �\���̂̃T�C�Y
		CS_VREDRAW | CS_HREDRAW,		// �E�C���h�E�X�^�C��
		WndProc,						// ���̃E�C���h�E�̃��b�Z�[�W����������R�[���o�b�N�֐��ւ̃|�C���^
		0L,								// �E�C���h�E�N���X�\���̂̌��Ɋ��蓖�Ă�⑫�o�C�g���D����0�D
		0L,								// �E�C���h�E�C���X�^���X�̌��Ɋ��蓖�Ă�⑫�o�C�g���D����0�D
		hInstance, 						// ���̃N���X�̂��߂̃E�C���h�E�v���V�[�W��������C���X�^���X�n��
		LoadIcon(hInstance, nullptr), 	// �A�C�R���̃n���h��
		LoadCursor(nullptr, IDC_ARROW), // �}�E�X�J�[�\���̃n���h���DLoadCursor(nullptr, IDC_ARROW )�Ƃ�
		(HBRUSH)(COLOR_WINDOW + 1), 	// �E�C���h�E�w�i�F(�w�i�`��p�u���V�̃n���h��)�D
		nullptr,						// �f�t�H���g���j���[��(MAKEINTRESOURCE(���j���[ID))
		"DirectX11",					// ���̃E�C���h�E�N���X�ɂ��閼�O
		LoadIcon(hInstance, nullptr)	// 16�~16�̏������T�C�Y�̃A�C�R��
	};

#if defined(_DEBUG)
	// �R���\�[���f�o�b�O
	mdx::system::d3dx11debug::DebugConsoleBegin();
#endif

	// �}�l�[�W���[
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
			mdx_imgui::Begin(u8"�E�B���h�E�̊�{����");
			mdx_imgui::TextWrapped(u8"ImGui�ō쐬�����E�B���h�E�̊�{�I�ȑ���͈ȉ��̒ʂ�ł��B");
			mdx_imgui::Bullet();
			mdx_imgui::TextWrapped(u8"�^�C�g���o�[���_�u���N���b�N����ƃ^�C�g���o�[��艺�̃E�B���h�E�G���A�����ă^�C�g���o�[�����ɂȂ�܂��B");
			mdx_imgui::Bullet();
			mdx_imgui::TextWrapped(u8"�E�B���h�E�̉E���܂��͍����̎O�p�R�[�i�[���h���b�O����ƃE�B���h�E�T�C�Y��ύX�ł��܂��B �_�u���N���b�N����ƃE�B���h�E���̓��e�Ƀt�B�b�g����T�C�Y�Ƀ��T�C�Y����܂��B");
			mdx_imgui::Bullet();
			mdx_imgui::TextWrapped(u8"�E�B���h�E���̋󔒂̃G���A���h���b�O���邱�ƂŃE�B���h�E���̂����R�ɓ��������Ƃ��ł��܂��B");
			mdx_imgui::Bullet();
			mdx_imgui::TextWrapped(u8"�}�E�X�z�C�[���ŃE�B���h�E�����X�N���[�����邱�Ƃ��ł��܂��B");
			mdx_imgui::Bullet();
			mdx_imgui::TextWrapped(u8"�E�B���h�E���m�̃h�b�L���O���ł��܂��B");
			mdx_imgui::End();
		}

		{
			mdx_imgui::Begin(u8"�E�B���h�E�̍쐬");
			mdx_imgui::Text(u8"�ʏ�̕���");
			mdx_imgui::Text(u8"2�s��", { 1.0f, 0.0f, 1.0f, 1.0f }); // �F�t������
			mdx_imgui::BulletText(u8"�����ۂ��擪�ɕt��������\n\"���s\"��������"); // \n �ŉ��s���܂��B " ���g�������Ƃ��� \" �̂悤�ɂ��܂��B
			mdx_imgui::Indent();
			mdx_imgui::BulletText(u8"�C���f���g���ꂽ����");
			mdx_imgui::TextDisabled(u8"�����ȕ���"); // �O���[�ŕ\������܂��B
			mdx_imgui::Unindent();
			mdx_imgui::Separator();
			mdx_imgui::TextWrapped(u8"�ƂĂ��������͂ł��E�B���h�E�̉����ɉ����Ď����I�ɐ܂�Ԃ��Ď��̍s�ɕ\�����Ă���܂�");
			mdx_imgui::NewLine(); // �󔒍s��ǉ����܂��B
			mdx_imgui::PushTextWrapPos(200.0f); // �܂�Ԃ������镝���w�肵�܂��B
			mdx_imgui::Text(u8"PushTextWrapPos����PopTextWrapPos�܂ł̊Ԃ�Text�͐܂�Ԃ��ĕ\������܂�");
			// ... 
			// �����ɒǉ����镶�͂͂��ׂ� PushTextWrapPos(200.0f); �̉e�����󂯂܂��B
			// ...
			mdx_imgui::PopTextWrapPos();
			mdx_imgui::End();
		}

		{
			/*
			���̂悤�ɂ��ă{�^����`�F�b�N�{�b�N�X��z�u���܂��B
			�{�^�����������A�������́A�`�F�b�N�{�b�N�X�������ƕʂ̃E�B���h�E���J���悤�ɂ������̂ł����A
			���͂��̂܂܂ł̓`�F�b�N�{�b�N�X���N���b�N���Ă��ʂ̃E�B���h�E���J���܂���B
			�{�^���̂ق��̓N���b�N������Ƃ����ƕʂ̃E�B���h�E���J���܂��B�������͕��܂��B

			���̂悤�ɂ��� "##1" �� "##2" �̂悤�ȕ���������邱�ƂŌ݂�����ʂł���悤�ɂȂ�A
			�`�F�b�N�{�b�N�X���N���b�N���Ă��ʂ̃E�B���h�E�̊J���ł���悤�ɂȂ�܂��B
			*/
			static bool is_open = false;

			mdx_imgui::Begin(u8"�{�^���ƃ`�F�b�N�{�b�N�X");
			if (mdx_imgui::Button("Open/Close##1"))
			{
				// �{�^�����N���b�N�����Ƃ����ɂ��܂��B
				is_open ^= 1;
			}

			mdx_imgui::SmallButton(u8"�������{�^��");

			mdx_imgui::SameLine(); // ���ɏ���UI�p�[�c�����݂Ɠ����s�ɔz�u���܂��B

			// �`�F�b�N�{�b�N�X���N���b�N������is_open�����]���Atrue�Ȃ�`�F�b�N�}�[�N���\������܂��B
			mdx_imgui::Checkbox("Open/Close##2", is_open);
			if (is_open)
			{
				// is_open��true�Ȃ�"�ʂ̃E�B���h�E"���\������܂��B
				mdx_imgui::Begin(u8"�ʂ̃E�B���h�E", &is_open);
				mdx_imgui::Text("Hello");
				mdx_imgui::End();
			}

			/*
			���[�v���ő�ʂɓ������O��UI�p�[�c���쐬�������ꍇ
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
			mdx_imgui::Begin(u8"���W�I�{�^��");

			static int radio = 0;
			// ���W�I�{�^�����N���b�N�����Ƒ�3�����̐�������2������radio�Ɋi�[����܂��B
			mdx_imgui::RadioButton(u8"���W�I�{�^�� 0", &radio, 0); mdx_imgui::SameLine();
			mdx_imgui::RadioButton(u8"���W�I�{�^�� 1", &radio, 1); mdx_imgui::SameLine();
			mdx_imgui::RadioButton(u8"���W�I�{�^�� 2", &radio, 2);

			mdx_imgui::Text(ANSI_TO_UTF8_ENCODE(mdx_imgui::MakeString("���W�I�{�^����%d��I�����Ă��܂�", radio)));

			mdx_imgui::End();
		}

		{
			mdx_imgui::Begin(u8"�������ŋ}��/�}������O�p���{�^��");

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
			mdx_imgui::Begin(u8"�h���b�v�_�E�����X�g�ƕ��͓��͗�");

			{
				mdx_imgui::LabelText(u8"���x��", u8"�l");

				const char* items[] = { "AAA", "BBB", "CCC" };
				static int item_current = 0; // 0�Ȃ�"AAA", 1�Ȃ�"BBB", 2�Ȃ�"CCC"

				mdx_imgui::Combo(u8"�h���b�v�_�E�����X�g##1", &item_current, items, ((int)(sizeof(items) / sizeof(*items))));
				// �ʂ̏������Ƃ��� \0 �ō��ڂ���؂��ď������@������܂��B
				mdx_imgui::Combo(u8"�h���b�v�_�E�����X�g##2", &item_current, "AAA\0BBB\0CCC\0");

				mdx_imgui::Text(ANSI_TO_UTF8_ENCODE(mdx_imgui::MakeString("���ݑI������Ă���̂� %d �ŁA%s �ł�", item_current, items[item_current])));

				static std::string str1, str2;
				mdx_imgui::InputText(u8"���͓��͗�##1", &str1);
				mdx_imgui::InputTextWithHint(u8"���͓��͗�##2", u8"�󗓎��ɕ\������镶�͂��w��ł��܂�", &str2);

				/*
				ImGui::Combo���g�����ƂŃh���b�v�_�E�����X�g���쐬���邱�Ƃ��ł��܂��B
				���͓��͗����������鎞�́ATAB�L�[�܂���SHIFT+TAB�L�[�œ��͗��̃t�H�[�J�X��ύX�ł��܂��B
				*/
			}

			{
				static std::string text;

				auto flags = mdx_imgui::ImGuiInputTextFlags_::ImGuiInputTextFlags_AllowTabInput; // Tab�L�[���������ƂŃ^�u�����͂����悤�ɂȂ�܂��B
				// flags |= ImGuiInputTextFlags_ReadOnly; // �ҏW�ł��Ȃ��悤�ɂ���ɂ͂��̂悤�ɂ��܂��B

				// ������3�s�̕��͓��͗����쐬���܂��B
				// �X�N���[�����邱�Ƃŉ��̍s�����邱�Ƃ��ł��܂��B
				// "##"�����Ă��邱�ƂŃ��x�����͕\������܂���B
				mdx_imgui::InputTextMultiline("##text", &text, { -FLT_MIN, mdx_imgui::GetTextLineHeight() * 3 }, flags);
			}

			{
				static char buf1[64] = "";
				mdx_imgui::InputText(u8"���͗�", buf1, 64);
				static char buf2[64] = "";
				// ImGuiInputTextFlags_CharsDecimal �����邱�Ƃ� 0123456789.+-*/ �̕����������͂ł��Ȃ����͗��ɂȂ�܂��B
				// InputTextWithHint���g�����Ƃœ��͗����󔒂̎��ɃO���[�ŕ\������镶�͂��w��ł��܂��B
				mdx_imgui::InputTextWithHint(u8"�������͗�", "only 0123456789.+-*/", buf2, 64, mdx_imgui::ImGuiInputTextFlags_::ImGuiInputTextFlags_CharsDecimal);

				static char bufpass[64] = "password123"; // �ŏ��ɓ��͂��Ă�������
				// ***�\���̃p�X���[�h���͗��ɂ���ɂ� ImGuiInputTextFlags_Password ���g���܂��B
				// �󔒂������Ȃ����͗��ɂ���ɂ� ImGuiInputTextFlags_CharsNoBlank ���g���܂��B
				mdx_imgui::InputText("Password", bufpass, 64, mdx_imgui::ImGuiInputTextFlags_::ImGuiInputTextFlags_Password | mdx_imgui::ImGuiInputTextFlags_::ImGuiInputTextFlags_CharsNoBlank);
			}

			mdx_imgui::End();
		}

		{
			mdx_imgui::Begin(u8"�������͗�");

			static int i0 = 123;
			mdx_imgui::InputInt(u8"�������͗�", &i0);

			static float f0 = 0.001f;
			mdx_imgui::InputFloat(u8"�������͗�", &f0, 0.01f, 1.0f, "%.3f");

			static float vec3[3] = { 0.10f, 0.20f, 0.30f };
			mdx_imgui::InputFloat3(u8"3�̒l", vec3, "%.2f");
			mdx_imgui::End();
		}

		{
			mdx_imgui::Begin(u8"�h���b�O���Ēl�𑝌�������(�X���C�_�[)");

			static int i1 = 50, i2 = 42;
			mdx_imgui::DragInt(u8"�h���b�O���Đ����ύX", &i1, 1);
			mdx_imgui::DragInt(u8"���\��", &i2, 1, 0, 100, "%d%%"); // �Ō�̈����Œl�̕\���̎d�����w��ł��܂��B

			mdx_imgui::SetNextItemWidth(100); // ����UI�p�[�c�̕���100�ɂ��܂��B

			static float f1 = 1.00f;
			mdx_imgui::DragFloat(u8"�h���b�O���ď����ύX", &f1, 0.005f);

			// ����UI�p�[�c�̕����E�B���h�E���̔����ɂ��܂��B
			mdx_imgui::SetNextItemWidth(mdx_imgui::GetWindowWidth() * 0.5f);

			static int i3 = 0;
			// -1����3�܂Ńh���b�O���ĕω������邱�Ƃ��ł��܂��B
			mdx_imgui::SliderInt(u8"�X���C�_�[�Ő����ύX", &i3, -1, 3);

			// ����UI�p�[�c�̕����E�B���h�E�̍��[����A�E�[����100�̈ʒu�܂łɂ��܂��B
			mdx_imgui::SetNextItemWidth(-100);

			static float f2 = 0.123f, f3 = 0.0f;
			// 0����1�܂ŕω����܂��B"ratio = %.3f"�̕����Œl�̕\���̎d�����w��ł��܂��B
			mdx_imgui::SliderFloat(u8"�X���C�_�[�ŏ����ύX##1", &f2, 0.0f, 1.0f, "ratio = %.3f");

			// �Ō�̈�����5.0f�Ƃ��Ă��邱�ƂŃX���C�_�[�����E�̒[�ɋ߂Â��قǒl���傫����������悤�ɂȂ�܂��B
			// �t�ɃX���C�_�[�������t�߂ɂ���ꍇ�͏����Ȓl�ł����������Ȃ��悤�ɂȂ�܂��B
			// -10 ���� 10 �܂Ńh���b�O���ĕω������邱�Ƃ��ł��܂��B
			mdx_imgui::SliderFloat(u8"�X���C�_�[�ŏ����ύX##2", &f3, -10.0f, 10.0f, "%.4f", 5.0f);

			static float angle = 0.0f;
			mdx_imgui::SliderAngle(u8"�p�x", &angle); // -360����360�܂Ńh���b�O���ĕω������邱�Ƃ��ł��܂��B

			static float begin = 10, end = 90;

			// �ŏ��l0,�ő�l100�̃X���C�_�[��2�쐬���܂��B
			// "Min:"�̃X���C�_�[�́A"Max:"�̃X���C�_�[�̒l�𒴂��Ȃ��悤�ɃX���C�_�[�œ��������Ƃ��ł��܂��B
			// 0.2f�̓h���b�O���鎞�ɒl���ω�����ʂł��B2.0f�ɂ����ꍇ��2���l���ω�����悤�ɂȂ�܂��B
			mdx_imgui::DragFloatRange2("range", &begin, &end, 0.2f, 0.0f, 100.0f, "Min: %.1f %%", "Max: %.1f %%");

			static float vec3f[4] = { 0.10f, 0.20f, 0.30f };
			// �e���ڂ� 0 ���� 1 �܂Ńh���b�O���ĕω������邱�Ƃ��ł��܂��B
			mdx_imgui::SliderFloat3(u8"3�̒l���X���C�_�[����", vec3f, 0.0f, 1.0f);

			static float x = 1.0f, y = 2.0f, z = 3.0f;
			mdx_imgui::PushItemWidth(70); // ���ꂩ����UI�p�[�c�̕���70�ŌŒ肵�܂��B

			const char* items[] = { "AAA", "BBB", "CCC", "DDD" };
			static int item = -1;
			mdx_imgui::Combo("##XYZ", &item, items, ((int)(sizeof(items) / sizeof(*items))));

			mdx_imgui::SameLine(0, 10); // ����UI�p�[�c�𓯂��s�ɔz�u���A���̍ہA�E��10�����X�y�[�X���󂯂܂��B

			mdx_imgui::SliderFloat("X", &x, 0.0f, 5.0f); mdx_imgui::SameLine();
			mdx_imgui::SliderFloat("Y", &y, 0.0f, 5.0f); mdx_imgui::SameLine();
			mdx_imgui::SliderFloat("Z", &z, 0.0f, 5.0f);

			mdx_imgui::PopItemWidth();

			mdx_imgui::End();
		}

		{
			mdx_imgui::Begin(u8"�����X���C�_�[");

			static float values[7] = { 0.0f, 0.60f, 0.35f, 0.9f, 0.70f, 0.20f, 0.0f };
			for (int i = 0; i < (int)(sizeof(values) / sizeof(float)); i++)
			{
				if (i > 0) mdx_imgui::SameLine();
				mdx_imgui::PushID(i);

				// �����X���C�_�[��18,����160,�ŏ��l0,�ő�l1�ō쐬���܂��B
				mdx_imgui::VSliderFloat("##v", { 18, 160 }, &values[i], 0.0f, 1.0f, "");

				if (mdx_imgui::IsItemActive() || mdx_imgui::IsItemHovered())
					// �X���C�_�[����}�E�X�I�[�o�[�������Ɍ��݂̒l���|�b�v�A�b�v�ŕ\������܂��B
					mdx_imgui::SetTooltip(mdx_imgui::MakeString("%.3f", values[i]));

				mdx_imgui::PopID();
			}
			mdx_imgui::End();
		}

		{

			/*
			�F�̂����l�p�`���N���b�N����ƃJ���[�s�b�J�[�ŐF��I���ł���悤�ɂȂ�܂��B
			RGB�̂��ꂼ��̓��͗����h���b�O���邱�ƂŁARGB�̒l�����ꂼ��ω������邱�Ƃ��ł��܂��B
			���̓��͗����E�N���b�N���邱�ƂŁARGB�\���ł͂Ȃ�HSV�\���ɂ��邱�Ƃ��ł��܂��B
			���͗���CTRL+�N���b�N���邱�ƂŁA�l���L�[�{�[�h�œ��͂����ԂɂȂ�܂��B
			*/

			mdx_imgui::Begin(u8"enum�I��UI�ƃJ���[�I��UI");

			enum Element { Element_Fire, Element_Earth, Element_Air, Element_Water, Element_COUNT };

			static int current_element = 0;
			auto strEnumName = NAMEOF_ENUM((Element)current_element);
			mdx_imgui::SliderInt(u8"enum�̑I��", &current_element, 0, Element_COUNT - 1, strEnumName.data());

			static float col1[3] = { 1.0f,0.0f,0.2f };
			static float col2[4] = { 0.4f,0.7f,0.0f,0.5f };

			mdx_imgui::ColorEdit3(u8"�J���[ 1", col1); // RGB
			mdx_imgui::ColorEdit4(u8"�J���[ 2", col2); // RGBA�̃A���t�@�t��

			mdx_imgui::ImGuiColorEditFlags_ flag = mdx_imgui::ImGuiColorEditFlags_::ImGuiColorEditFlags_Float; // 0 ~ 255�\�L�ł͂Ȃ��A0.0 ~ 1.0�\�L�ɂ��܂��B
			flag |= mdx_imgui::ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs; // �������͗����\���ɂ��܂��B
			flag |= mdx_imgui::ImGuiColorEditFlags_::ImGuiColorEditFlags_NoLabel;  // �J���[�{�b�N�X�̉E�ׂɔz�u����郉�x�����Ȃ����܂��B
			mdx_imgui::ColorEdit3(u8"�J���[ID", col1, flag);
			mdx_imgui::End();
		}

		{
			mdx_imgui::Begin(u8"���X�g�{�b�N�X");

			const char* listbox_items[] = { "Apple", "Banana", "Cherry", "Kiwi", "Mango", "Orange", "Pineapple", "Strawberry", "Watermelon" };
			static int listbox_item_current = 1;

			mdx_imgui::ListBox(u8"���X�g�{�b�N�X", &listbox_item_current, listbox_items, ((int)sizeof(listbox_items) / sizeof(int)), 4);
			mdx_imgui::End();
		}

		{
			mdx_imgui::Begin(u8"�\�E�e�[�u��");

			// ��3������true�ɂ��邱�ƂŊe���ڂ̊ԂɃO���[�̏c�_�����邱�Ƃ��ł��܂��B�f�t�H���g��true�B
			// Columns�ō쐬�����c�_�̓h���b�O���邱�ƂŁA�e�R�����̉�����ύX�����邱�Ƃ��ł��܂��B
			// �c��3�̕\���쐬���܂��B
			mdx_imgui::Columns(3, "columnID", true);
			static bool selected[12] = {};
			for (int i = 0; i < 12; i++)
			{
				// Windows�ł�sprintf�Ōx�����o��̂Ńv���W�F�N�g�̃v���p�e�B����AC/C++ -> �v���v���Z�b�T�� �u_CRT_SECURE_NO_WARNINGS�v ��ǉ����܂��B
				char label[32]; sprintf(label, "Item %d", i);
				if (mdx_imgui::Selectable(label, &selected[i]))
				{
					// �I�����ꂽ���ɂ����������������ɏ����܂��B
				}
				mdx_imgui::NextColumn();
			}
			mdx_imgui::Columns(1); // ���ꂪ�Ȃ��Ƃ���ȍ~��UI�p�[�c�����ׂ�Columns���ڂ̉����ɐ�������Ă��܂��܂��B
			mdx_imgui::Separator();
			// �c��4�̕\���쐬���܂��B
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
				// ImGuiSelectableFlags_SpanAllColumns�����邱�ƂŃR�������̍��ڂ�I����������
				// ���̍s�S�̂�I�������悤�Ȍ����ڂɂȂ�܂��B
				// ���ꂪ�Ȃ��ꍇ�́A"ID"������c��̍��ڂ����n�C���C�g����܂���B
				if (mdx_imgui::Selectable(label, select == i, mdx_imgui::ImGuiSelectableFlags_::ImGuiSelectableFlags_SpanAllColumns))
					select = i;
				bool hovered = mdx_imgui::IsItemHovered();
				mdx_imgui::NextColumn();
				mdx_imgui::Text(mdx_imgui::MakeString("%s", names[i])); mdx_imgui::NextColumn();
				mdx_imgui::Text(mdx_imgui::MakeString("%s", paths[i])); mdx_imgui::NextColumn();
				mdx_imgui::Text(mdx_imgui::MakeString("%d", hovered));  mdx_imgui::NextColumn();
			}
			mdx_imgui::Columns(1); // �Y�ꂸ�ɂ�������܂��B
			mdx_imgui::End();
		}

		{
			/*

			TreeNode��if�߂̒��̍Ō�ɕK�� TreePop(); �������K�v������܂��B
			TreeNode��if�߂̒��ƊO�ł�UI�p�[�c�ɂ��镶�����ID�͓����ł����Ă���ʂ����悤�ɂȂ��Ă��܂��B

			*/

			static bool bOpen2 = true;
			mdx_imgui::Begin(u8"�J�\�ȃt�B�[���h");

			if (mdx_imgui::CollapsingHeader(u8"�J�\�ȃt�B�[���h1"))
			{
				// ���̃t�B�[���h���J���Ă���ꍇ�ɂ����������������ɏ����܂��B
				if (mdx_imgui::TreeNode(u8"�c���[�m�[�h A"))
				{
					mdx_imgui::Text(u8"A�L�[������");
					// "�c���[�m�[�h A"���J���Ă���ꍇ�ɂ����������������ɏ����܂��B
					if (mdx_imgui::IsKeyDown(mdx_imgui::GetKeyIndex(mdx_imgui::ImGuiKey_::ImGuiKey_A)))
						mdx_imgui::Text(u8"A�L�[��������Ă���ꍇ�͕\������܂�");
					mdx_imgui::TreePop();
				}
				// SetNextItemOpen���g�����ƂŎ��̃c���[�m�[�h�͍ŏ�����J���Ă����ԂɂȂ�܂��B
				mdx_imgui::SetNextItemOpen(true, mdx_imgui::ImGuiCond_::ImGuiCond_Once);
				if (mdx_imgui::TreeNode(u8"�c���[�m�[�h B"))
				{
					mdx_imgui::Text(u8"�c���[�m�[�h B�͍ŏ�����J������Ԃł�");
					mdx_imgui::TreePop();
				}
			}

			if (mdx_imgui::NewTreeNode(u8"�J�\�ȃt�B�[���h2", bOpen2))
			{
				mdx_imgui::Text(u8"�֐���Ńc���[�m�[�h���쐬�ł��܂�");
				mdx_imgui::EndTreeNode();
			}

			mdx_imgui::End();
		}

		{

			/*

			(?)�Ƃ����������O���[�ŕ\�����A�}�E�X�I�[�o�[���Ƀ|�b�v�A�b�v�ŕ��͂�\�������܂��B
			Begin~���R�[��������AEnd~���R�[�����APush~���R�[��������Pop~���R�[�����܂��B

			*/

			mdx_imgui::Begin(u8"�c�[���`�b�v");

			mdx_imgui::TextDisabled("(?)");
			if (mdx_imgui::IsItemHovered())
			{
				// "(?)"�Ƃ����������}�E�X�I�[�o�[���Ă���ꍇ�ɂ����������������ɏ����܂��B
				mdx_imgui::BeginTooltip();
				// �|�b�v�A�b�v�E�B���h�E�̉������t�H���g�T�C�Y��35�{�̒����ɂ��܂��B
				// ����𒴂��镶�͂̎��͎����I�ɂ��̉��̍s�ɑ����܂�(2�s�ɂȂ�܂�)�B
				mdx_imgui::PushTextWrapPos(mdx_imgui::GetFontSize() * 35.0f);
				mdx_imgui::Text(u8"�|�b�v�A�b�v���ɕ\������镶�͂������ɏ����܂�");
				mdx_imgui::PopTextWrapPos();
				mdx_imgui::EndTooltip();
			}

			mdx_imgui::End();
		}

		{
			/*
			"�I��.."�{�^�����������ƂŃ|�b�v�A�b�v���\������܂��B�ʂ̂ǂ������N���b�N���邩�A�������́AEscape�L�[���������ƂŃ|�b�v�A�b�v����܂��B
			BeginPopup�ł͂Ȃ��A ImGui::BeginPopupModal("popupID") �ɂ����ꍇ�́A
			�|�b�v�A�b�v�\���ȊO�̃G���A�͔��������Ȃ�A�N���b�N���Ă��������Ȃ��Ȃ��ԂɂȂ�܂��B
			�|�b�v�A�b�v���ꂽ������ǂꂩ��I�����Ȃ��ƃ|�b�v�A�b�v�\�������Ȃ��悤�ɂȂ�܂��B
			�����āA�|�b�v�A�b�v�\���̓E�B���h�E�S�̂̒��S�ɕ\������܂��B
			�Ⴆ�΁A�t�@�C�����폜����ۂ̊m�F���b�Z�[�W�t�� "OK" / "Cancel" �E�B���h�E�ȂǂɎg���܂��B
			*/

			mdx_imgui::Begin(u8"�|�b�v�A�b�v�E�B���h�E");

			const char* names[] = { "AAA", "BBB", "CCC", "DDD" };
			static int selected = -1;
			if (mdx_imgui::Button(u8"�I��.."))
			{
				// "�I��.."�{�^�����N���b�N���ꂽ�ꍇ�ɂ����ɂ��܂��B
				// ������"popupID"�Ɠ���ID����BeginPopup���|�b�v�A�b�v�Ƃ��ĕ\�����܂��B
				mdx_imgui::OpenPopup("popupID");
			}
			mdx_imgui::SameLine();
			mdx_imgui::TextUnformatted(selected == -1 ? "<None>" : names[selected]);

			if (mdx_imgui::BeginPopup("popupID"))
			{
				mdx_imgui::Text(u8"�I������");
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

			�v���b�g���C�����q�X�g�O�������v���O���X�o�[�����ׂă}�E�X�I�[�o�[���邱�Ƃł��̒l���|�b�v�A�b�v�\�������悤�ɂȂ��Ă��܂��B

			*/

			mdx_imgui::Begin(u8"�܂���O���t�E�q�X�g�O�����E�v���O���X�o�[");

			static float arr[] = { 0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f };
			// �ŏ��l 0, �ő�l 1 �̃O���t���쐬���܂��B
			// ImVec2(0,50)�Ƃ��Ă��邱�ƂŁA���͎����Ō��܂�A������50�ɂȂ�܂��B
			mdx_imgui::PlotLines(u8"�v���b�g���C��", arr, ((int)sizeof(arr) / sizeof(float)), 0, u8"�^�C�g��", 0.0f, 1.0f, { 0, 50 });

			mdx_imgui::PlotHistogram(u8"�q�X�g�O����", arr, ((int)sizeof(arr) / sizeof(float)), 0, "", 0.0f, 1.0f, { 0, 50 });

			static float progress = 0.22f;
			// 0 ~ 1 �܂ł̒��ł� 0.22 �������̈ʒu�ɂ���̂���\�����܂��B
			mdx_imgui::ProgressBar(progress, { 0.0f, 0.0f });

			mdx_imgui::End();
		}

		{
			mdx_imgui::Begin(u8"�q�E�B���h�E");

			static int line = 50;
			bool goto_line = mdx_imgui::Button("Goto");
			mdx_imgui::SameLine(); // ����UI�p�[�c�𓯂��s�ɔz�u���܂��B
			// ImGuiInputTextFlags_EnterReturnsTrue �����邱�ƂŁA���͗��ŕ�������͂��Ă���Enter�L�[���������Ƃ�InputInt�֐���true��Ԃ��悤�ɂȂ�܂��B
			// "##"�����邱�ƂŃ��x�����͕\������܂���B
			goto_line |= mdx_imgui::InputInt("##Line", &line, 0, 0, mdx_imgui::ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue);
			// "Goto"�{�^�����������܂��͓��͗��Ő�������͂��Ă���Enter�L�[���������ƂŁAgoto_line��true�ɂȂ�܂��B
			mdx_imgui::PushStyleVar(mdx_imgui::ImGuiStyleVar_::ImGuiStyleVar_ChildRounding, 5.0f); // �O�g���p�ۂɂ��܂��B
			mdx_imgui::PushStyleColor(mdx_imgui::ImGuiCol_::ImGuiCol_ChildBg, { 0.23f, 0, 0, 0.39f }); // �q�E�B���h�E�̔w�i�F��ύX���܂��B
			mdx_imgui::SetNextItemWidth(100); // ����UI�p�[�c�̕���100�ɂ��܂��B
			mdx_imgui::ImGuiWindowFlags_ flag = (mdx_imgui::ImGuiWindowFlags_)0;
			// flag |= ImGuiWindowFlags_NoScrollWithMouse; // �q�E�B���h�E�̒��Ń}�E�X�X�N���[�����Ă��q�E�B���h�E���̓X�N���[������Ȃ��Ȃ�܂��B
			flag |= mdx_imgui::ImGuiWindowFlags_::ImGuiWindowFlags_MenuBar; // �q�E�B���h�E��BeginMenuBar�����邱�Ƃ��\�ɂ��܂��B
			// ��3������true�ɂ��邱�ƂŊO�g���\������܂��B
			mdx_imgui::BeginChild("ChildID", { mdx_imgui::GetWindowContentRegionWidth() * 0.5f, 260 }, true, flag);
			if (mdx_imgui::BeginMenuBar())
			{
				if (mdx_imgui::BeginMenu("Menu"))
				{
					// "Menu"���N���b�N������J�����j���[���ڂ������Ă����܂��B
					if (mdx_imgui::MenuItem("New")) {}
					// ...
					mdx_imgui::EndMenu();
				}
				mdx_imgui::EndMenuBar();
			}
			// �q�E�B���h�E�̒���100�s�쐬���܂��B
			for (int i = 0; i < 100; i++)
			{
				mdx_imgui::Text(ANSI_TO_UTF8_ENCODE(mdx_imgui::MakeString("%04d: �X�N���[���\�̈�", i)));
				if (goto_line && line == i)
					// SetScrollHereY�͌��݂̈ʒu�ɃX�N���[���ʒu�������Ă��܂��B
					// "Goto"�{�^�������������̍s���̈ʒu���q�E�B���h�E�̐^�񒆂̈ʒu�ɂȂ�悤�ɏu���ɃX�N���[������܂��B
					mdx_imgui::SetScrollHereY();
				// mdx_imgui::SetScrollHereY(0.0f); // �q�E�B���h�E�̈�ԏ�ɗ���悤�ɏu���ɃX�N���[������܂��B
				// mdx_imgui::SetScrollHereY(1.0f); // �q�E�B���h�E�̈�ԉ��ɗ���悤�ɏu���ɃX�N���[������܂��B
			}
			if (goto_line && line >= 100)
				mdx_imgui::SetScrollHereY();
			mdx_imgui::EndChild();
			mdx_imgui::PopStyleColor();
			mdx_imgui::PopStyleVar();

			mdx_imgui::End();
		}

		{
			mdx_imgui::Begin(u8"�^�u");

			mdx_imgui::ImGuiTabBarFlags_ flag = mdx_imgui::ImGuiTabBarFlags_::ImGuiTabBarFlags_Reorderable; // �^�u���h���b�O���ĕ��ёւ����ł���悤�ɂȂ�܂��B
			flag |= mdx_imgui::ImGuiTabBarFlags_::ImGuiTabBarFlags_TabListPopupButton; // �^�u�̈�ԍ��[�Ƀh���b�v�_�E�����X�g���\������鉺�����O�p�`�̃N���b�N�G���A���쐬���A��������^�u��I���ł���悤�ɂȂ�܂��B
			flag |= mdx_imgui::ImGuiTabBarFlags_::ImGuiTabBarFlags_AutoSelectNewTabs; // �^�u��V�����쐬�������Ɏ����ł��̃^�u��I����Ԃɂ��܂��B
			// flag |= mdx_imgui::ImGuiTabBarFlags_::ImGuiTabBarFlags_NoCloseWithMiddleMouseButton; // �^�u�̒��Ń}�E�X�����{�^���N���b�N���邱�ƂŃ^�u����邱�Ƃ��ł���@�\�𖳌��ɂ��܂��B
			// flag |= mdx_imgui::ImGuiTabBarFlags_::ImGuiTabBarFlags_NoTooltip; // �^�u��Ƀ}�E�X�I�[�o�[�����ꍇ�ɕ\�������^�u���̃|�b�v�A�b�v�\���𖳌��ɂ��܂��B
			// flag |= mdx_imgui::ImGuiTabBarFlags_::ImGuiTabBarFlags_FittingPolicyResizeDown; // �^�u���E�B���h�E���𒴂��Ă������񂠂�ꍇ�Ƀ^�u�̕��������Ń��T�C�Y���ăt�B�b�g�����邱�Ƃ��ł��܂��B
			flag |= mdx_imgui::ImGuiTabBarFlags_::ImGuiTabBarFlags_FittingPolicyScroll; // �^�u�̕��������Ń��T�C�Y�������ɍ��E�̖��{�^�����E�[�ɔz�u���Ă�������^�u�����ԂɑI���ł���悤�ɂ��܂��B
			const char* names[8] = { "�����^�u��##1", "�����^�u��##2", "�^�u2", "�^�u3", "�^�u4", "�^�u5", "�^�u6", "�^�u7" };
			// �����^�u�����g�������ꍇ�́A"##1" �� "##2" �̂悤�ɋ�ʂ�����K�v������܂��B
			static bool opened[8] = { true, true, true, true, true, true, true, true };
			if (mdx_imgui::BeginTabBar("TabBarID", flag))
			{
				for (int n = 0; n < ((int)sizeof(opened) / sizeof(bool)); n++)
					// ��2������&opened[n]���ȗ�����ƕ���{�^��(X)���쐬����܂���B
					if (opened[n] && mdx_imgui::BeginTabItem(ANSI_TO_UTF8_ENCODE(names[n]), opened[n]))
					{
						mdx_imgui::Text(ANSI_TO_UTF8_ENCODE(mdx_imgui::MakeString("����� %s �̃^�u�ł�", names[n])));
						mdx_imgui::EndTabItem();
					}
				mdx_imgui::EndTabBar();
			}
			mdx_imgui::End();
		}

		{
			/*
			���C�����j���[�o�[
			if (ImGui::BeginMainMenuBar())�̒���ImGui::MenuItem�ȉ��̃R�[�h�̓��C�����j���[�������łȂ��A
				�E�B���h�E�̒���|�b�v�A�b�v�\���̒��Ȃǂɂ��������Ƃ��ł��܂��B
			*/

			if (mdx_imgui::BeginMainMenuBar())
			{
				// ���C�����j���[��\�����Ă��鎞�̏����������ɏ����܂��B
				if (mdx_imgui::BeginMenu("File"))
				{
					// "File"���N���b�N����File���j���[���J�������̏����������ɏ����܂��B
					// bool MenuItem(const char* label, const char* shortcut = NULL,
					//               bool selected = false, bool enabled = true);
					mdx_imgui::MenuItem("(dummy menu)", nullptr, false, false); // enabled��false�ɂ��邱�ƂŃO���[�\���ł��܂��B 
					if (mdx_imgui::MenuItem("New"))
					{
						// "New"���N���b�N���ꂽ���̏����������ɏ����܂��B
					}
					// �V���[�g�J�b�g�L�[�\���͑�2�����ɏ����܂��B
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
					if (mdx_imgui::MenuItem("Checked", nullptr, true)) {} // selected��true�ɂ��邱�ƂŃ`�F�b�N�}�[�N�����邱�Ƃ��ł��܂�
					if (mdx_imgui::MenuItem("Quit", "Alt+F4")) {}
					mdx_imgui::EndMenu();
				}
				if (mdx_imgui::BeginMenu("Edit")) { mdx_imgui::EndMenu(); }
				mdx_imgui::EndMainMenuBar();
			}
		}

		{
			/*
			ImGui::Dummy���g�����Ƃŋ󔒃G���A����邱�Ƃ��ł��܂��B
			*/

			mdx_imgui::Begin(u8"�_�~�[");

			mdx_imgui::Button("A", { 40, 40 });
			mdx_imgui::SameLine();
			mdx_imgui::Dummy({ 40, 40 }); // �_�~�[���g���ĊԊu���󂯂邱�Ƃ��ł��܂��B
			mdx_imgui::SameLine();
			if (mdx_imgui::Button("B", { 40, 40 }))
			{
				// �{�^���������ꂽ���ɂ����������������܂��B
			}

			mdx_imgui::Button("A", { 40, 40 });
			// mdx_imgui::SameLine(float offset_from_start_x, float spacing_w)
			mdx_imgui::SameLine(0.0f, 40.0f);
			if (mdx_imgui::Button("B", { 40, 40 }))
			{
				// �{�^���������ꂽ���ɂ����������������܂��B
			}

			/*
			���ꂾ�ƊԊu�����������Ȃ�܂����A������x���������ڂɂȂ�܂��B
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
	// �R���\�[���f�o�b�O�I��
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
