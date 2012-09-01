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
#include "../NstCrc32.hpp"
#include "NstMapper053.hpp"

namespace Nes
{
	namespace Core
	{
		#ifdef NST_MSVC_OPTIMIZE
		#pragma optimize("s", on)
		#endif

		Mapper53::Mapper53(Context& c)
		:
		Mapper     (c,CROM_MAX_8K|WRAM_NONE|NMT_VERTICAL),
		epromFirst (c.prg.Size() >= SIZE_32K && Crc32::Compute(c.prg.Mem(),SIZE_32K) == EPROM_CRC)
		{
		}

		void Mapper53::SubReset(const bool hard)
		{
			if (hard)
			{
				regs[0] = 0;
				regs[1] = 0;

				UpdatePrg();
			}

			Map( WRK_PEEK );
			Map( 0x6000U, 0x7FFFU, &Mapper53::Poke_6000 );
			Map( 0x8000U, 0xFFFFU, &Mapper53::Poke_8000  );
		}

		void Mapper53::SubLoad(State::Loader& state)
		{
			while (const dword chunk = state.Begin())
			{
				if (chunk == AsciiId<'R','E','G'>::V)
				{
					{
						State::Loader::Data<2> data( state );

						regs[0] = data[0];
						regs[1] = data[1];
					}

					UpdatePrg();
				}

				state.End();
			}
		}

		void Mapper53::SubSave(State::Saver& state) const
		{
			state.Begin( AsciiId<'R','E','G'>::V ).Write16( regs[0] | uint(regs[1]) << 8 ).End();
		}

		#ifdef NST_MSVC_OPTIMIZE
		#pragma optimize("", on)
		#endif

		void Mapper53::UpdatePrg()
		{
			const uint r = regs[0] << 3 & 0x78;

			wrk.SwapBank<SIZE_8K,0x0000>
			(
				(r << 1 | 0xF) + (epromFirst ? 0x4 : 0x0)
			);

			prg.SwapBanks<SIZE_16K,0x0000>
			(
				(regs[0] & 0x10) ? (r | (regs[1] & 0x7)) + (epromFirst ? 0x2 : 0x0) : epromFirst ? 0x00 : 0x80,
				(regs[0] & 0x10) ? (r | (0xFF    & 0x7)) + (epromFirst ? 0x2 : 0x0) : epromFirst ? 0x01 : 0x81
			);
		}

		NES_POKE_D(Mapper53,6000)
		{
			regs[0] = data;
			UpdatePrg();
			ppu.SetMirroring( (data & 0x20) ? Ppu::NMT_HORIZONTAL : Ppu::NMT_VERTICAL );
		}

		NES_POKE_D(Mapper53,8000)
		{
			regs[1] = data;
			UpdatePrg();
		}
	}
}
