#include "pch.h"
#include "FontEnumerator.h"
#include <dwrite.h>
#include <collection.h>

using namespace Fonts;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;

template <class T> void SafeRelease(T **ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}

IVector<String^>^ FontEnumerator::GetFonts()
{
	Vector<String^>^ result = ref new Vector<String^>();

	IDWriteFactory* pDWriteFactory = NULL;

	HRESULT hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&pDWriteFactory)
		);

	IDWriteFontCollection* pFontCollection = NULL;

	if (SUCCEEDED(hr))
		hr = pDWriteFactory->GetSystemFontCollection(&pFontCollection);

	UINT32 familyCount = 0;

	if (SUCCEEDED(hr))
		familyCount = pFontCollection->GetFontFamilyCount();

	for (UINT32 i = 0; i < familyCount; ++i)
	{
		IDWriteFontFamily* pFontFamily = NULL;

		if (SUCCEEDED(hr))
			hr = pFontCollection->GetFontFamily(i, &pFontFamily);

		IDWriteLocalizedStrings* pFamilyNames = NULL;

		if (SUCCEEDED(hr))
			hr = pFontFamily->GetFamilyNames(&pFamilyNames);

		UINT32 index = 0;
		BOOL exists = false;

		wchar_t localeName[LOCALE_NAME_MAX_LENGTH];

		if (SUCCEEDED(hr))
		{
			int defaultLocaleSuccess = GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_LENGTH);

			if (defaultLocaleSuccess)
				hr = pFamilyNames->FindLocaleName(localeName, &index, &exists);
			if (SUCCEEDED(hr) && !exists) // if the above find did not find a match, retry with US English
				hr = pFamilyNames->FindLocaleName(L"en-us", &index, &exists);
		}

		if (!exists)
			index = 0;

		UINT32 length = 0;

		if (SUCCEEDED(hr))
			hr = pFamilyNames->GetStringLength(index, &length);

		wchar_t* name = new (std::nothrow) wchar_t[length + 1];
		if (name == NULL)
			hr = E_OUTOFMEMORY;

		if (SUCCEEDED(hr))
			hr = pFamilyNames->GetString(index, name, length + 1);
		if (SUCCEEDED(hr))
			result->Append(ref new String(name));

		SafeRelease(&pFontFamily);
		SafeRelease(&pFamilyNames);

		delete[] name;
	}

	SafeRelease(&pFontCollection);
	SafeRelease(&pDWriteFactory);

	return result;
}
