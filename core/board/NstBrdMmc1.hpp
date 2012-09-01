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

#ifndef NST_BOARDS_MMC1_H
#define NST_BOARDS_MMC1_H

#ifdef NST_PRAGMA_ONCE
#pragma once
#endif

namespace Nes
{
	namespace Core
	{
		namespace Boards
		{
			class NST_NO_VTABLE Mmc1 : public Mapper
			{
			protected:

				enum Board
				{
					BRD_GENERIC,
					BRD_SAROM,
					BRD_SBROM,
					BRD_SCROM,
					BRD_SC1ROM,
					BRD_SEROM,
					BRD_SFROM,
					BRD_SF1ROM,
					BRD_SFEOROM,
					BRD_SFEXPROM,
					BRD_SGROM,
					BRD_SHROM,
					BRD_SH1ROM,
					BRD_SJROM,
					BRD_SKROM,
					BRD_SLROM,
					BRD_SL1ROM,
					BRD_SL3ROM,
					BRD_SLRROM,
					BRD_SNROM,
					BRD_SOROM,
					BRD_SUROM,
					BRD_SXROM,
					BRD_GENERIC_WRAM
				};

				enum Revision
				{
					REV_A,
					REV_B,
					REV_C
				};

				explicit Mmc1(Context&,Board=BRD_GENERIC,Revision=REV_B);
				~Mmc1() {}

				void SubReset(bool);
				void UpdatePrg();
				void UpdateWrk();
				void UpdateChr() const;
				void UpdateNmt();
				void Sync(Event,Input::Controllers*);

				enum
				{
					CTRL,
					CHR0,
					CHR1,
					PRG0
				};

				enum
				{
					CTRL_MIRRORING     = 0x03,
					CTRL_PRG_SWAP_LOW  = 0x04,
					CTRL_PRG_SWAP_16K  = 0x08,
					CTRL_CHR_SWAP_4K   = 0x10,
					CTRL_RESET         = CTRL_PRG_SWAP_LOW|CTRL_PRG_SWAP_16K,
					PRG0_WRAM_DISABLED = 0x10
				};

			private:

				static dword BoardToWRam(Board);

				void BaseSave(State::Saver&) const;
				void BaseLoad(State::Loader&,dword);

				void ResetRegisters();
				virtual void UpdateRegisters(uint);

				NES_DECL_POKE( Prg );

				struct Serial
				{
					enum
					{
						RESET_BIT = 0x80,
						RESET_CYCLES = 2
					};

					uint buffer;
					uint shifter;
					Cycle ready;
				};

				Serial serial;

			protected:

				byte regs[4];

				const Revision revision;
			};
		}
	}
}

#endif
