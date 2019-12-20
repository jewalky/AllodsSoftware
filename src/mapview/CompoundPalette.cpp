#include "CompoundPalette.h"
#include <algorithm>

#include "../logging.h"

void CompoundPalette::SetBasePalette(const std::vector<Color>& basePalette)
{
	mBasePalette = basePalette;
	mLastTint = Color(0, 0, 0, 0);
	mLastBrightness = 0;
	mLastContrast = 0;
}

void CompoundPalette::UpdatePalettes(Color tint, uint16_t brightness, uint16_t contrast)
{
	if (tint == mLastTint && brightness == mLastBrightness && contrast == mLastContrast)
		return;

	mGeneratedPalettes.resize(mBasePalette.size() * 33);

	// generate dark palettes
	for (int i = 0; i < 33; i++)
	{
		Color* generatedData = mGeneratedPalettes.data() + mBasePalette.size() * i;

		for (uint32_t j = 0; j < mBasePalette.size(); j++)
		{

			Color baseColor = mBasePalette[j];
			uint32_t r = baseColor.components.r;
			uint32_t g = baseColor.components.g;
			uint32_t b = baseColor.components.b;

			// #1: apply tint
			r = r * tint.components.r / 255;
			g = g * tint.components.g / 255;
			b = b * tint.components.b / 255;

			// #2: apply contrast
			r = ((r + 127) * contrast / 255) - 127;
			g = ((g + 127) * contrast / 255) - 127;
			b = ((b + 127) * contrast / 255) - 127;

			// #3: apply brightness
			r = r * brightness / 255;
			g = g * brightness / 255;
			b = b * brightness / 255;

			// #4: apply brightness delta
			if (i < 15)
			{
				// dark
				r = r * i / 15;
				g = g * i / 15;
				b = b * i / 15;
			}
			else if (i > 16)
			{
				int brightPower = (i - 15) * 2;
				r += r * brightPower / 15;
				g += g * brightPower / 15;
				b += b * brightPower / 15;
			}

			// #5: clamp to 0-255
			r = std::max(uint32_t(0), std::min(uint32_t(255), r));
			g = std::max(uint32_t(0), std::min(uint32_t(255), g));
			b = std::max(uint32_t(0), std::min(uint32_t(255), b));

			// write to palette
			generatedData[j] = Color(r, g, b, 255);

		}
	}
}

const Color* CompoundPalette::GetPalette(uint32_t index) const
{
	uint32_t calcIndex = mBasePalette.size() * index;
	if (calcIndex > mGeneratedPalettes.size())
		return nullptr;
	return mGeneratedPalettes.data() + calcIndex;
}
