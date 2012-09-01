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
#include "NstMapper230.hpp"

namespace Nes
{
	namespace Core
	{
		#ifdef NST_MSVC_OPTIMIZE
		#pragma optimize("s", on)
		#endif

		void Mapper230::SubReset(const bool hard)
		{
			if (hard)
				romSwitch = 0;
			else
				romSwitch ^= 1;

			prg.SwapBanks<SIZE_16K,0x0000>( romSwitch ? 0 : 8, romSwitch ? 7 : 39 );

			if (romSwitch)
				ppu.SetMirroring( Ppu::NMT_VERTICAL );

			Map( 0x8000U, 0xFFFFU, &Mapper230::Poke_Prg );

			// for the soft reset triggering feature
			cpu.Poke( 0x2000, 0x00 );
		}

		void Mapper230::SubLoad(State::Loader& state)
		{
			while (const dword chunk = state.Begin())
			{
				if (chunk == AsciiId<'R','E','G'>::V)
					romSwitch = state.Read8() & 0x1;

				state.End();
			}
		}

		void Mapper230::SubSave(State::Saver& state) const
		{
			state.Begin( AsciiId<'R','E','G'>::V ).Write8( romSwitch ).End();
		}

		#ifdef NST_MSVC_OPTIMIZE
		#pragma optimize("", on)
		#endif

		NES_POKE_D(Mapper230,Prg)
		{
			if (romSwitch)
			{
				prg.SwapBank<SIZE_16K,0x0000>( data & 0x7 );
			}
			else
			{
				prg.SwapBanks<SIZE_16K,0x0000>( 8 + (data & 0x1F), (8 + (data & 0x1F)) | (~data >> 5 & 0x1) );
				ppu.SetMirroring( (data & 0x40) ? Ppu::NMT_VERTICAL : Ppu::NMT_HORIZONTAL );
			}
		}
	}
}
