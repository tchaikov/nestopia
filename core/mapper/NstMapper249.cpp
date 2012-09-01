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

#include "../NstMapper.hpp"
#include "../board/NstBrdMmc3.hpp"
#include "NstMapper249.hpp"

namespace Nes
{
	namespace Core
	{
		#ifdef NST_MSVC_OPTIMIZE
		#pragma optimize("s", on)
		#endif

		void Mapper249::SubReset(const bool hard)
		{
			if (hard)
				exReg = 0;

			Mmc3::SubReset( hard );

			Map( 0x5000U, &Mapper249::Poke_5000 );
		}

		void Mapper249::SubLoad(State::Loader& state)
		{
			while (const dword chunk = state.Begin())
			{
				if (chunk == AsciiId<'R','E','G'>::V)
					exReg = state.Read8() & 0x2;

				state.End();
			}
		}

		void Mapper249::SubSave(State::Saver& state) const
		{
			state.Begin( AsciiId<'R','E','G'>::V ).Write8( exReg ).End();
		}

		#ifdef NST_MSVC_OPTIMIZE
		#pragma optimize("", on)
		#endif

		NES_POKE_D(Mapper249,5000)
		{
			if (exReg != (data & 0x2))
			{
				exReg = data & 0x2;

				Mapper249::UpdatePrg();
				Mapper249::UpdateChr();
			}
		}

		uint Mapper249::UnscrambleChr(const uint data) const
		{
			return (!exReg) ? data :
			(
				(data >> 0 & 0x03) |
				(data >> 1 & 0x04) |
				(data >> 4 & 0x08) |
				(data >> 2 & 0x10) |
				(data << 3 & 0x20) |
				(data << 2 & 0x40) |
				(data << 2 & 0x80)
			);
		}

		uint Mapper249::UnscramblePrg(const uint data) const
		{
			return (!exReg) ? data :
			(
				(data << 0 & 0x01) |
				(data >> 3 & 0x02) |
				(data >> 1 & 0x04) |
				(data << 2 & 0x18)
			);
		}

		void Mapper249::UpdatePrg()
		{
			const uint i = (regs.ctrl0 & Regs::CTRL0_XOR_PRG) >> 5;

			prg.SwapBanks<SIZE_8K,0x0000>
			(
				UnscramblePrg( banks.prg[i]   ),
				UnscramblePrg( banks.prg[1]   ),
				UnscramblePrg( banks.prg[i^2] ),
				UnscramblePrg( banks.prg[3]   )
			);
		}

		void Mapper249::UpdateChr() const
		{
			ppu.Update();

			const uint swap = (regs.ctrl0 & Regs::CTRL0_XOR_CHR) << 5;

			chr.SwapBanks<SIZE_2K>
			(
				0x0000 ^ swap,
				UnscrambleChr( banks.chr[0] << 1 | 0 ) >> 1,
				UnscrambleChr( banks.chr[1] << 1 | 1 ) >> 1
			);

			chr.SwapBanks<SIZE_1K>
			(
				0x1000 ^ swap,
				UnscrambleChr( banks.chr[2] ),
				UnscrambleChr( banks.chr[3] ),
				UnscrambleChr( banks.chr[4] ),
				UnscrambleChr( banks.chr[5] )
			);
		}
	}
}
