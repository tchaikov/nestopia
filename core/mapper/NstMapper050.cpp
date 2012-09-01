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
#include "../NstClock.hpp"
#include "NstMapper050.hpp"

namespace Nes
{
	namespace Core
	{
		#ifdef NST_MSVC_OPTIMIZE
		#pragma optimize("s", on)
		#endif

		Mapper50::Mapper50(Context& c)
		:
		Mapper (c,PROM_MAX_128K|CROM_MAX_8K|WRAM_NONE),
		irq    (c.cpu)
		{}

		void Mapper50::Irq::Reset(const bool hard)
		{
			if (hard)
				count = 0;
		}

		void Mapper50::SubReset(const bool hard)
		{
			if (hard)
				prg.SwapBanks<SIZE_8K,0x0000>( 8, 9, 0, 11 );

			irq.Reset( hard, hard ? false : irq.Connected() );

			for (uint i=0x4020; i < 0x6000; i += 0x80)
				Map( i+0x00, i+0x20, (i & 0x100) ? &Mapper50::Poke_4120 : &Mapper50::Poke_4020 );

			Map( 0x6000U, 0x7FFFU, &Mapper50::Peek_wRom );
		}

		void Mapper50::SubLoad(State::Loader& state)
		{
			while (const dword chunk = state.Begin())
			{
				if (chunk == AsciiId<'I','R','Q'>::V)
				{
					State::Loader::Data<3> data( state );

					irq.Connect( data[0] & 0x1 );
					irq.unit.count = data[1] | data[2] << 8;
				}

				state.End();
			}
		}

		void Mapper50::SubSave(State::Saver& state) const
		{
			const byte data[3] =
			{
				irq.Connected() ? 0x1 : 0x0,
				irq.unit.count & 0xFF,
				irq.unit.count >> 8
			};

			state.Begin( AsciiId<'I','R','Q'>::V ).Write( data ).End();
		}

		#ifdef NST_MSVC_OPTIMIZE
		#pragma optimize("", on)
		#endif

		NES_PEEK_A(Mapper50,wRom)
		{
			return *prg.Source().Mem( (SIZE_128K-SIZE_8K-0x6000) + address );
		}

		NES_POKE_D(Mapper50,4020)
		{
			prg.SwapBank<SIZE_8K,0x4000>
			(
				(data << 0 & 0x8) |
				(data << 2 & 0x4) |
				(data >> 1 & 0x3)
			);
		}

		NES_POKE_D(Mapper50,4120)
		{
			irq.Update();
			irq.Connect( data & 0x1 );
			irq.ClearIRQ();
		}

		bool Mapper50::Irq::Clock()
		{
			return ++count == 0x1000;
		}

		void Mapper50::Sync(Event event,Input::Controllers*)
		{
			if (event == EVENT_END_FRAME)
			{
				irq.unit.count = 0;
				irq.VSync();
			}
		}
	}
}
