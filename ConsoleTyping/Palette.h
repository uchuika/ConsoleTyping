#pragma once

#include "Math.h"

// ターミナル風 UI の配色 (CLI 版 Grok を基にしたモック準拠)
namespace Palette
{
	inline Vector3 Hex(unsigned int rgb)
	{
		return Vector3(((rgb >> 16) & 0xff) / 255.0f,
					   ((rgb >> 8) & 0xff) / 255.0f,
					   (rgb & 0xff) / 255.0f);
	}

	static const Vector3 Outside = Hex(0x0b0b0b);
	static const Vector3 Panel = Hex(0x121212);
	static const Vector3 Line = Hex(0x2a2a2a);
	static const Vector3 DotRed = Hex(0xff5f57);
	static const Vector3 DotYellow = Hex(0xfebc2e);
	static const Vector3 DotGreen = Hex(0x28c840);
	static const Vector3 Cyan = Hex(0x5ac8fa);
	static const Vector3 Green = Hex(0x3fd0a0);
	static const Vector3 Purple = Hex(0xb79cff);
	static const Vector3 TextBright = Hex(0xe8e8e8);
	static const Vector3 Text = Hex(0xd0d0d0);
	static const Vector3 TextDim = Hex(0x8a8a8a);
	static const Vector3 TextMute = Hex(0x7a7a7a);
	static const Vector3 TextFaint = Hex(0x6b6b6b);
	static const Vector3 TextDark = Hex(0x4a4a4a);
	static const Vector3 SelectBg = Hex(0x222222);
	static const Vector3 SelectLine = Hex(0x333333);
}
