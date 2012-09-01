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
#include "NstMapper169.hpp"

namespace Nes
{
	namespace Core
	{
		#ifdef NST_MSVC_OPTIMIZE
		#pragma optimize("s", on)
		#endif

		void Mapper169::SubReset(const bool hard)
		{
			Map( 0x8000U, 0xBFFFU, &Mapper169::Poke_8000 );
			Map( 0xC000U, 0xFFFFU, &Mapper169::Poke_C000 );

			if (hard)
			{
				ctrl = 0;
				bank = 0;

				UpdatePrg();
			}
		}

		void Mapper169::SubSave(State::Saver& state) const
		{
			state.Begin( AsciiId<'R','E','G'>::V ).Write16( ctrl ).Write8( bank ).End();
		}

		void Mapper169::SubLoad(State::Loader& state)
		{
			while (const dword chunk = state.Begin())
			{
				if (chunk == AsciiId<'R','E','G'>::V)
				{
					ctrl = state.Read16();
					bank = state.Read8();

					UpdatePrg();
				}

				state.End();
			}
		}

		#ifdef NST_MSVC_OPTIMIZE
		#pragma optimize("", on)
		#endif

		void Mapper169::UpdatePrg()
		{
			prg.SwapBanks<SIZE_16K,0x0000>
			(
				(ctrl >> 2 & 0x38) | ((ctrl & 0x102) == 0x002 ? (bank & 0x6) : bank),
				(ctrl >> 2 & 0x38) | ((ctrl & 0x102) == 0x102 ? 0x7 : (bank | (ctrl >> 1 & 0x1)))
			);
		}

		NES_POKE_A(Mapper169,8000)
		{
			if (ctrl != address)
			{
				ctrl = address;
				UpdatePrg();
				ppu.SetMirroring( (ctrl & 0x1) ? Ppu::NMT_HORIZONTAL : Ppu::NMT_VERTICAL );
			}
		}

		NES_POKE_A(Mapper169,C000)
		{
			address &= 0x7;

			if (bank != address)
			{
				bank = address;
				UpdatePrg();
			}
		}
	}
}
