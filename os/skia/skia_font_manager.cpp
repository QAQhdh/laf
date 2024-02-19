// LAF OS Library
// Copyright (c) 2019-2024  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#include "os/skia/skia_font_manager.h"

#include "os/skia/skia_font.h"

#include "include/core/SkFont.h"
#include "include/core/SkString.h"

#if LAF_WINDOWS
  #include "include/ports/SkTypeface_win.h"
#elif LAF_MACOS
  #include "include/ports/SkFontMgr_mac_ct.h"
#elif LAF_LINUX
  #include "include/ports/SkFontMgr_fontconfig.h"
#endif

namespace os {

//////////////////////////////////////////////////////////////////////
// SkiaTypeface

SkiaTypeface::SkiaTypeface(sk_sp<SkTypeface> skTypeface)
  : m_skTypeface(skTypeface)
{
}

FontStyle SkiaTypeface::fontStyle() const
{
  SkFontStyle skStyle = m_skTypeface->fontStyle();
  return FontStyle((FontStyle::Weight)skStyle.weight(),
                   (FontStyle::Width)skStyle.width(),
                   (FontStyle::Slant)skStyle.slant());
}

//////////////////////////////////////////////////////////////////////
// SkiaFontStyleSet

SkiaFontStyleSet::SkiaFontStyleSet(sk_sp<SkFontStyleSet> set)
  : m_skSet(set)
{
}

int SkiaFontStyleSet::count()
{
  return m_skSet->count();
}

void SkiaFontStyleSet::getStyle(int index,
                                FontStyle& style,
                                std::string& name)
{
  SkFontStyle skStyle;
  SkString skName;
  m_skSet->getStyle(index, &skStyle, &skName);
  style = FontStyle((FontStyle::Weight)skStyle.weight(),
                    (FontStyle::Width)skStyle.width(),
                    (FontStyle::Slant)skStyle.slant());
  name = skName.c_str();
}

TypefaceRef SkiaFontStyleSet::typeface(int index)
{
  return make_ref<SkiaTypeface>(m_skSet->createTypeface(index));
}

TypefaceRef SkiaFontStyleSet::matchStyle(const FontStyle& style)
{
  SkFontStyle skStyle((SkFontStyle::Weight)style.weight(),
                      (SkFontStyle::Width)style.width(),
                      (SkFontStyle::Slant)style.slant());
  return make_ref<SkiaTypeface>(m_skSet->matchStyle(skStyle));
}

//////////////////////////////////////////////////////////////////////
// SkiaFontManager

SkiaFontManager::SkiaFontManager()
{
#if LAF_WINDOWS
  m_skFontMgr = SkFontMgr_New_DirectWrite();
#elif LAF_MACOS
  m_skFontMgr = SkFontMgr_New_CoreText(nullptr);
#elif LAF_LINUX
  m_skFontMgr = SkFontMgr_New_FontConfig(nullptr);
#else
  m_skFontMgr = SkFontMgr::RefEmpty();
#endif
}

SkiaFontManager::~SkiaFontManager()
{
}

Ref<Font> SkiaFontManager::defaultFont(float size) const
{
  sk_sp<SkTypeface> face =
    m_skFontMgr->legacyMakeTypeface(nullptr, SkFontStyle());
  ASSERT(face);
  SkFont skFont(face, size);
  return make_ref<SkiaFont>(skFont);
}

int SkiaFontManager::countFamilies() const
{
  return m_skFontMgr->countFamilies();
}

std::string SkiaFontManager::familyName(int i) const
{
  SkString name;
  m_skFontMgr->getFamilyName(i, &name);
  return std::string(name.c_str());
}

Ref<FontStyleSet> SkiaFontManager::familyStyleSet(int i) const
{
  return make_ref<SkiaFontStyleSet>(m_skFontMgr->createStyleSet(i));
}

Ref<FontStyleSet> SkiaFontManager::matchFamily(const std::string& familyName) const
{
  return make_ref<SkiaFontStyleSet>(m_skFontMgr->matchFamily(familyName.c_str()));
}

} // namespace os
