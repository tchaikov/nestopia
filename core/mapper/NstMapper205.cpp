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
#include "NstMapper205.hpp"

namespace Nes
{
	namespace Core
	{
		#ifdef NST_MSVC_OPTIMIZE
		#pragma optimize("s", on)
		#endif

		void Mapper205::SubReset(const bool hard)
		{
			if (hard)
				exReg = 0;

			Mmc3::SubReset( hard );

			Map( 0x6800U, 0x6FFFU, &Mapper205::Poke_6800 );
			Map( 0x7800U, 0x7FFFU, &Mapper205::Poke_6800 );
		}

		void Mapper205::SubLoad(State::Loader& state)
		{
			while (const dword chunk = state.Begin())
			{
				if (chunk == AsciiId<'R','E','G'>::V)
					exReg = (state.Read8() & 0x3) << 4;

				state.End();
			}
		}

		void Mapper205::SubSave(State::Saver& state) const
		{
			state.Begin( AsciiId<'R','E','G'>::V ).Write8( exReg >> 4 ).End();
		}

		#ifdef NST_MSVC_OPTIMIZE
		#pragma optimize("", on)
		#endif

		NES_POKE_D(Mapper205,6800)
		{
			data = data << 4 & 0x30;

			if (exReg != data)
			{
				exReg = data;
				Mapper205::UpdatePrg();
				Mapper205::UpdateChr();
			}
		}

		void Mapper205::UpdatePrg()
		{
			const uint i = (regs.ctrl0 & Regs::CTRL0_XOR_PRG) >> 5;
			const uint mask = (exReg & 0x20) ? 0x0F : 0x1F;

			prg.SwapBanks<SIZE_8K,0x0000>
			(
				exReg | ( mask & banks.prg[i]   ),
				exReg | ( mask & banks.prg[1]   ),
				exReg | ( mask & banks.prg[i^2] ),
				exReg | ( mask & banks.prg[3]   )
			);
		}

		void Mapper205::UpdateChr() const
		{
			ppu.Update();

			const uint swap = (regs.ctrl0 & Regs::CTRL0_XOR_CHR) << 5;

			uint base = exReg << 2;

			chr.SwapBanks<SIZE_2K>
			(
				0x0000 ^ swap,
				base | banks.chr[0],
				base | banks.chr[1]
			);

			base <<= 1;

			chr.SwapBanks<SIZE_1K>
			(
				0x1000 ^ swap,
				base | banks.chr[2],
				base | banks.chr[3],
				base | banks.chr[4],
				base | banks.chr[5]
			);
		}
	}
}
