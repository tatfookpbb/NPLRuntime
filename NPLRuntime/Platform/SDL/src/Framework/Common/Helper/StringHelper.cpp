#include "ParaEngine.h"
#include <boost/thread/tss.hpp>
#include <boost/locale/encoding_utf.hpp>
#include "util/StringHelper.h"
#include "2dengine/GUIEdit.h"
#include "SDL2/SDL.h"

namespace ParaEngine {
	void CGUIEditBox::CopyToClipboard()
	{
		// Copy the selection text to the clipboard
		if (m_nCaret != m_nSelStart)
		{
			int nFirst = Math::Min(m_nCaret, m_nSelStart);
			int nLast = Math::Max(m_nCaret, m_nSelStart);
			std::u16string text;
			if (nLast - nFirst > 0)
			{
				text.resize(m_Buffer.GetTextSize() + 1);
				if (m_PasswordChar == '\0')
				{
					text = std::u16string(m_Buffer.GetBuffer() + nFirst, m_Buffer.GetBuffer() + nLast);
				}
				else
				{
					for (int i = 0; i < (nLast - nFirst); ++i)
					{
						text[i] = m_PasswordChar;
					}
				}
			}
			text[nLast - nFirst] = '\0';  // Terminate it
			std::string texta = "";
			if (StringHelper::UTF16ToUTF8(text, texta)) {
				StringHelper::CopyTextToClipboard(texta);
			}
		}
	}

	void CGUIEditBox::PasteFromClipboard()
	{
		DeleteSelectionText();
		std::string text = StringHelper::GetTextFromClipboard();
		if (m_Buffer.InsertStringA(m_nCaret, text.c_str())) {
			PlaceCaret(m_nCaret + text.length());
		}
		m_nSelStart = m_nCaret;
		m_bIsModified = true;
	}

	bool StringHelper::CopyTextToClipboard(const string& text_)
	{
		return 0 == SDL_SetClipboardText(text_.c_str());
	}

	const char* StringHelper::GetTextFromClipboard()
	{
		static std::string text;
		if (SDL_HasClipboardText() == SDL_FALSE) return "";
		char* str = SDL_GetClipboardText();
		text = str;
		SDL_free(str);
		return text.c_str();
	}
}