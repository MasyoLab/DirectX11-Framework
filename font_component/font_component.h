
/*

Author : Otake Yoshiki
Project creation date : 2019/09/01~

*/

#pragma once

#include "resource_data.h"
#include "component.h"
#include <string>
#include <vector>

namespace ax
{
	namespace com
	{
		/*
		-------------------------------------------------------------------	
		
		ax::com : フォントストックコンポーネント

		フォントテクスチャをタグ指定により作成でき、
		このコンポーネントを登録したオブジェクトが破棄されるまでストックされる

		指定したタグでフォントテクスチャのデータを作成することで、
		IRendererを継承したコンポーネントが、テクスチャを参照する際にタグ（ハッシュ）でセットできる

		-------------------------------------------------------------------	
		*/
		class CFontStock : private resource::CTexture,
						   private resource::CFont,
						   public base::IComponent
		{
		private:
			UHashCode hash_;
			std::vector<std::string> texture_tag_list_;

		public:
			CFontStock()
			{
			}
			~CFontStock();

			bool CreateTexture(const std::string& tag, const std::string& font_name, int font_size, const std::string& text);
		};
	}
}