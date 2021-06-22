#include "font_component.h"

ax::com::CFontStock::~CFontStock()
{
	hash_ = 0;

	for (auto& itr : texture_tag_list_)
	{
		hash_ = hash::GetStringHashCode(itr);

		for (auto& data : texture_list_[hash_])
		{
			data->Release();
		}
		texture_list_.erase(hash_);
		font_index_list_.erase(hash_);
	}
	texture_tag_list_.clear();
}

bool ax::com::CFontStock::CreateTexture(const std::string& tag, const std::string& font_name, int font_size, const std::string& text)
{
	if (text.empty())
	{
		return false;
	}

	hash_ = hash::GetStringHashCode(tag);

	// タグ被り
	if (texture_index_list_.count(hash_) != 0)
	{
		return false;
	}

	// リストにタグが存在しなかったら空のデータを作成
	texture_list_.emplace(hash_, std::vector<UTexture>());
	font_index_list_.emplace(hash_, std::map<UINT, int>());
	texture_tag_list_.emplace_back(tag);

	// 文字化け対策
	setlocale(LC_ALL, "japanese");

	// set_stringが空になるまでテクスチャ生成
	std::string set_string = text;
	while(!set_string.empty())
	{
		HFONT h_font = CreateFont(
			font_size,					// height
			0,							// width
			0,							// escapement
			0,							// orientation
			FW_NORMAL,					// fnWeight
			false,						// fdwItalic
			false,						// fdwUnderline
			false,						// fdwStrikeOut
			SHIFTJIS_CHARSET,			// fdwCharset
			OUT_DEFAULT_PRECIS,			// fdwOutputPrecision
			CLIP_DEFAULT_PRECIS,		// fdwClipPrecision
			PROOF_QUALITY,				// fdwQuality
			FIXED_PITCH | FF_MODERN,	// fdwPitchAndFamily
			font_name.c_str()			// szFace
		);

		if (!h_font)
		{
			return false;
		}

		// デバイスコンテキストの取得
		HDC dc			= GetDC(NULL);
		HFONT old_font	= (HFONT)SelectObject(dc, h_font);

		// 文字コード取得
		UINT code = 0;
		if (IsDBCSLeadByte(set_string[0]) == 0)
		{
			code = (UINT)set_string[0];
			set_string.erase(0, 1);

		} else {

			code = (BYTE)set_string[0] << 8 | (BYTE)set_string[1];
			set_string.erase(0, 2);
		}

		// フォント情報の取得
		TEXTMETRIC tm;
		GetTextMetrics(dc, &tm);

		// ビットマップの取得
		GLYPHMETRICS gm;
		const MAT2 mat	= { { 0, 1 },{ 0, 0 },{ 0, 0 },{ 0, 1 } };
		DWORD size		= GetGlyphOutline(dc, code, GGO_GRAY4_BITMAP, &gm, 0, NULL, &mat);
		BYTE* font_bmp	= new BYTE[size];
		GetGlyphOutline(dc, code, GGO_GRAY4_BITMAP, &gm, size, font_bmp, &mat);

		// 使わないので破棄
		SelectObject(dc, old_font);
		DeleteObject(h_font);
		ReleaseDC(NULL, dc);

		const int font_width	= gm.gmCellIncX;
		const int font_height	= tm.tmHeight;

		// テクスチャの作成
		ax::UTexture2D font_texture;
		D3D11_TEXTURE2D_DESC font_tex_desc;
		ZeroMemory(&font_tex_desc, sizeof(font_tex_desc));
		font_tex_desc.Width					= font_width;
		font_tex_desc.Height				= font_height;
		font_tex_desc.MipLevels				= 1;
		font_tex_desc.ArraySize				= 1;
		font_tex_desc.Format				= DXGI_FORMAT_R8G8B8A8_UNORM;
		font_tex_desc.SampleDesc.Count		= 1;
		font_tex_desc.SampleDesc.Quality	= 0;
		font_tex_desc.Usage					= D3D11_USAGE_DYNAMIC;
		font_tex_desc.BindFlags				= D3D11_BIND_SHADER_RESOURCE;
		font_tex_desc.CPUAccessFlags		= D3D11_CPU_ACCESS_WRITE;
		font_tex_desc.MiscFlags				= 0;
		directx_->GetDevice()->CreateTexture2D(&font_tex_desc, NULL, &font_texture);

		// 書き込み開始
		D3D11_MAPPED_SUBRESOURCE map_resource;
		if (SUCCEEDED(directx_->GetDeviceContext()->Map(font_texture, 0, D3D11_MAP_WRITE_DISCARD, 0, &map_resource))) 
		{
			BYTE* bits	= (BYTE*)map_resource.pData;
			int ofs_x	= gm.gmptGlyphOrigin.x;
			int ofs_y	= tm.tmAscent - gm.gmptGlyphOrigin.y;
			int bmp_w	= gm.gmBlackBoxX + (4 - (gm.gmBlackBoxX % 4)) % 4;
			int bmp_h	= gm.gmBlackBoxY;
			int level	= 17;
			int x, y;
			DWORD alpha, color;

			ZeroMemory(bits, map_resource.RowPitch * tm.tmHeight);
			for (y = ofs_y; y < ofs_y + bmp_h; y++)
			{
				for (x = ofs_x; x < ofs_x + bmp_w; x++)
				{
					alpha = (255 * font_bmp[x - ofs_x + bmp_w * (y - ofs_y)]) / (level - 1);
					color = 0x00ffffff | (alpha << 24);
					memcpy((BYTE*)bits + map_resource.RowPitch * y + 4 * x, &color, sizeof(DWORD));
				}
			}
			directx_->GetDeviceContext()->Unmap(font_texture, 0);
		}
		delete[] font_bmp;

		// シェーダーリソース作成
		D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_desc;
		ZeroMemory(&shader_resource_desc, sizeof(shader_resource_desc));
		shader_resource_desc.Format						= font_tex_desc.Format;
		shader_resource_desc.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURE2D;
		shader_resource_desc.Texture2D.MostDetailedMip	= 0;
		shader_resource_desc.Texture2D.MipLevels		= font_tex_desc.MipLevels;

		UTexture texture;
		directx_->GetDevice()->CreateShaderResourceView(font_texture, nullptr, &texture);
		font_texture->Release();

		// テクスチャバッファを追加
		texture_list_[hash_].emplace_back(texture);
		font_index_list_[hash_].emplace(code, texture_list_[hash_].size() - 1);
	}

	return true;
}
