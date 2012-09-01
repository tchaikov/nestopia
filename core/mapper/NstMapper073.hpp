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

#ifndef NST_MAPPER_73_H
#define NST_MAPPER_73_H

#ifdef NST_PRAGMA_ONCE
#pragma once
#endif

namespace Nes
{
	namespace Core
	{
		class Mapper73 : public Mapper
		{
		public:

			explicit Mapper73(Context&);

		private:

			~Mapper73() {}

			void SubReset(bool);
			void SubSave(State::Saver&) const;
			void SubLoad(State::Loader&);
			void Sync(Event,Input::Controllers*);

			NES_DECL_POKE( 8000 );
			NES_DECL_POKE( 9000 );
			NES_DECL_POKE( A000 );
			NES_DECL_POKE( B000 );
			NES_DECL_POKE( C000 );
			NES_DECL_POKE( D000 );

			struct Irq
			{
				void Reset(bool);
				bool Clock();

				ibool enabled;
				uint count;
			};

			ClockUnits::M2<Irq> irq;
		};
	}
}

#endif
