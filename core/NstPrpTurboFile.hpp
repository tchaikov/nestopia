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

#ifndef NST_PRP_TURBOFILE_H
#define NST_PRP_TURBOFILE_H

#ifdef NST_PRAGMA_ONCE
#pragma once
#endif

namespace Nes
{
	namespace Core
	{
		namespace Peripherals
		{
			class TurboFile
			{
			public:

				explicit TurboFile(Cpu&);

				void Reset();
				void PowerOff() const;
				void SaveState(State::Saver&,const dword) const;
				void LoadState(State::Loader&);

			private:

				NES_DECL_POKE( 4016 );
				NES_DECL_PEEK( 4016 );
				NES_DECL_POKE( 4017 );
				NES_DECL_PEEK( 4017 );

				enum
				{
					SIZE = SIZE_8K
				};

				enum
				{
					WRITE_BIT    = 0x01,
					NO_RESET     = 0x02,
					WRITE_ENABLE = 0x04,
					READ_BIT     = 0x04
				};

				uint pos;
				uint bit;
				uint old;
				uint out;

				byte ram[SIZE];

				Io::Port p4016;
				Io::Port p4017;

				Cpu& cpu;

				File file;
			};
		}
	}
}

#endif
