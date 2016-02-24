#pragma once
using namespace Platform;
using namespace Windows::Foundation::Collections;

namespace Fonts
{
	public ref class FontEnumerator sealed
	{
	public:

		static IVector<String^>^ GetFonts();
	};
}