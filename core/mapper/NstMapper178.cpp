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
#include "NstMapper178.hpp"

namespace Nes
{
	namespace Core
	{
		#ifdef NST_MSVC_OPTIMIZE
		#pragma optimize("s", on)
		#endif

		void Mapper178::SubReset(const bool hard)
		{
			Map( 0x4800U, NMT_SWAP_HV );
			Map( 0x4801U, &Mapper178::Poke_4801 );
			Map( 0x4802U, &Mapper178::Poke_4802 );

			if (hard)
			{
				reg = 0;
				NES_DO_POKE(4801,0x4801,0x00);
			}
		}

		void Mapper178::SubLoad(State::Loader& state)
		{
			while (const dword chunk = state.Begin())
			{
				if (chunk == AsciiId<'R','E','G'>::V)
					reg = state.Read8() & 0xF;

				state.End();
			}
		}

		void Mapper178::SubSave(State::Saver& state) const
		{
			state.Begin( AsciiId<'R','E','G'>::V ).Write8( reg ).End();
		}

		#ifdef NST_MSVC_OPTIMIZE
		#pragma optimize("", on)
		#endif

		NES_POKE_D(Mapper178,4801)
		{
			reg = (data >> 1 & 0x3) | (reg & 0xC);
			prg.SwapBank<SIZE_32K,0x0000>( reg );
		}

		NES_POKE_D(Mapper178,4802)
		{
			reg = (reg & 0x3) | (data << 2 & 0xC);
		}
	}
}
