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
#include "NstMapper217.hpp"

namespace Nes
{
	namespace Core
	{
		#ifdef NST_MSVC_OPTIMIZE
		#pragma optimize("s", on)
		#endif

		void Mapper217::SubReset(const bool hard)
		{
			if (hard)
			{
				exRegs[0] = 0x00;
				exRegs[1] = 0xFF;
				exRegs[2] = 0x03;
			}

			exRegs[3] = false;

			Mmc3::SubReset( hard );

			Map( 0x5000U, &Mapper217::Poke_5000 );
			Map( 0x5001U, &Mapper217::Poke_5001 );
			Map( 0x5007U, &Mapper217::Poke_5007 );

			for (uint i=0x0000; i < 0x2000; i += 0x2)
			{
				Map( 0x8000 + i, &Mapper217::Poke_8000 );
				Map( 0x8001 + i, &Mapper217::Poke_8001 );
				Map( 0xA000 + i, &Mapper217::Poke_A000 );
				Map( 0xA001 + i, &Mapper217::Poke_A001 );
			}
		}

		void Mapper217::SubLoad(State::Loader& state)
		{
			while (const dword chunk = state.Begin())
			{
				if (chunk == AsciiId<'R','E','G'>::V)
				{
					state.Read( exRegs );
					exRegs[3] &= 0x1U;
				}

				state.End();
			}
		}

		void Mapper217::SubSave(State::Saver& state) const
		{
			state.Begin( AsciiId<'R','E','G'>::V ).Write( exRegs ).End();
		}

		#ifdef NST_MSVC_OPTIMIZE
		#pragma optimize("", on)
		#endif

		uint Mapper217::GetPrgBank(const uint bank) const
		{
			const uint high = exRegs[1] << 5 & 0x60U;

			if (exRegs[1] & 0x8U)
				return high | (bank & 0x1F);
			else
				return high | (exRegs[1] & 0x10U) | (bank & 0x0F);
		}

		void Mapper217::UpdatePrg()
		{
			const uint i = (regs.ctrl0 & Regs::CTRL0_XOR_PRG) >> 5;

			prg.SwapBanks<SIZE_8K,0x0000>
			(
				GetPrgBank( banks.prg[i]   ),
				GetPrgBank( banks.prg[1]   ),
				GetPrgBank( banks.prg[i^2] ),
				GetPrgBank( banks.prg[3]   )
			);
		}

		uint Mapper217::GetChrBank(const uint bank) const
		{
			const uint high = exRegs[1] << 8 & 0x300U;

			if (exRegs[1] & 0x8U)
				return high | bank;
			else
				return high | (exRegs[1] << 3 & 0x80U) | (bank & 0x7F);
		}

		void Mapper217::UpdateChr() const
		{
			ppu.Update();

			const uint swap = (regs.ctrl0 & Regs::CTRL0_XOR_CHR) << 5;

			chr.SwapBanks<SIZE_1K>
			(
				0x0000 ^ swap,
				GetChrBank( banks.chr[0] << 1 | 0x0 ),
				GetChrBank( banks.chr[0] << 1 | 0x1 ),
				GetChrBank( banks.chr[1] << 1 | 0x0 ),
				GetChrBank( banks.chr[1] << 1 | 0x1 )
			);

			chr.SwapBanks<SIZE_1K>
			(
				0x1000 ^ swap,
				GetChrBank( banks.chr[2] ),
				GetChrBank( banks.chr[3] ),
				GetChrBank( banks.chr[4] ),
				GetChrBank( banks.chr[5] )
			);
		}

		NES_POKE_D(Mapper217,5000)
		{
			exRegs[0] = data;

			if (data & 0x80)
			{
				data = (data & 0x0F) | (exRegs[1] << 4 & 0x30U);
				prg.SwapBanks<SIZE_16K,0x0000>( data, data );
			}
			else
			{
				Mapper217::UpdatePrg();
			}
		}

		NES_POKE_D(Mapper217,5001)
		{
			if (exRegs[1] != data)
			{
				exRegs[1] = data;
				Mapper217::UpdatePrg();
			}
		}

		NES_POKE_D(Mapper217,5007)
		{
			exRegs[2] = data;
		}

		NES_POKE_D(Mapper217,8000)
		{
			if (exRegs[2])
				Mmc3::NES_DO_POKE(C000,0xC000,data);
			else
				Mmc3::NES_DO_POKE(8000,0x8000,data);
		}

		NES_POKE_D(Mapper217,8001)
		{
			if (exRegs[2])
			{
				static const byte lut[8] = {0,6,3,7,5,2,4,1};

				data = (data & 0xC0) | lut[data & 0x07];
				exRegs[3] = true;

				Mmc3::NES_DO_POKE(8000,0x8000,data);
			}
			else
			{
				Mmc3::NES_DO_POKE(8001,0x8001,data);
			}
		}

		NES_POKE_D(Mapper217,A000)
		{
			if (exRegs[2])
			{
				if (exRegs[3] && ((exRegs[0] & 0x80U) == 0 || (regs.ctrl0 & Regs::CTRL0_MODE) < 6))
				{
					exRegs[3] = false;
					Mmc3::NES_DO_POKE(8001,0x8001,data);
				}
			}
			else
			{
				SetMirroringHV( data );
			}
		}

		NES_POKE_D(Mapper217,A001)
		{
			if (exRegs[2])
				SetMirroringHV( data );
			else
				Mmc3::NES_DO_POKE(A001,0xA001,data);
		}
	}
}
