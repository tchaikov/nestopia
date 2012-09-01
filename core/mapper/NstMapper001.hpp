////////////////////////////////////////////////////////////////////////////////////////
//
// Nestopia - NES/Famicom emulator written in C++
//
// Copyright (C) 2003-2007 Martin Freij
//
// This file is part of Nestopia.
//
// Nestopia is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// Nestopia is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Nestopia; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
////////////////////////////////////////////////////////////////////////////////////////

#ifndef NST_MAPPER_1_H
#define NST_MAPPER_1_H

#ifdef NST_PRAGMA_ONCE
#pragma once
#endif

namespace Nes
{
	namespace Core
	{
		class Mapper1 : public Boards::Mmc1
		{
		public:

			explicit Mapper1(Context&);

		private:

			~Mapper1() {}

			enum
			{
				ATR_SAROM = 1,
				ATR_SBROM,
				ATR_SCROM,
				ATR_SC1ROM,
				ATR_SEROM,
				ATR_SFROM,
				ATR_SF1ROM,
				ATR_SFEOROM,
				ATR_SFEXPROM,
				ATR_SGROM,
				ATR_SHROM,
				ATR_SH1ROM,
				ATR_SJROM,
				ATR_SKROM,
				ATR_SLROM,
				ATR_SL1ROM,
				ATR_SL3ROM,
				ATR_SLRROM,
				ATR_SNROM,
				ATR_SOROM,
				ATR_SUROM,
				ATR_SXROM,
				ATR_BOARD = 0x1F,
				ATR_MMC1A = 0x40
			};

			static Board GetBoard(const Context&);
		};
	}
}

#endif
