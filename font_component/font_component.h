
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
		
		ax::com : �t�H���g�X�g�b�N�R���|�[�l���g

		�t�H���g�e�N�X�`�����^�O�w��ɂ��쐬�ł��A
		���̃R���|�[�l���g��o�^�����I�u�W�F�N�g���j�������܂ŃX�g�b�N�����

		�w�肵���^�O�Ńt�H���g�e�N�X�`���̃f�[�^���쐬���邱�ƂŁA
		IRenderer���p�������R���|�[�l���g���A�e�N�X�`�����Q�Ƃ���ۂɃ^�O�i�n�b�V���j�ŃZ�b�g�ł���

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