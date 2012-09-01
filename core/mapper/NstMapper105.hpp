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

#ifndef NST_MAPPER_105_H
#define NST_MAPPER_105_H

#ifdef NST_PRAGMA_ONCE
#pragma once
#endif

#include "../NstDipSwitches.hpp"

namespace Nes
{
	namespace Core
	{
		class Mapper105 : public Boards::Mmc1
		{
		public:

			explicit Mapper105(Context&);

		private:

			class CartSwitches : public DipSwitches
			{
			public:

				CartSwitches();

				uint GetTime() const;
				inline bool ShowTime() const;

			private:

				uint NumDips() const;
				uint NumValues(uint) const;
				cstring GetDipName(uint) const;
				cstring GetValueName(uint,uint) const;
				uint GetValue(uint) const;
				void SetValue(uint,uint);

				uint time;
				ibool showTime;
			};

			~Mapper105() {}

			void SubReset(bool);
			void SubSave(State::Saver&) const;
			void SubLoad(State::Loader&);
			Device QueryDevice(DeviceType);

			void UpdateTimer();
			void UpdateRegisters(uint);
			void Sync(Event,Input::Controllers*);

			enum
			{
				IRQ_DISABLE = 0x10
			};

			enum
			{
				TIME_OFFSET = 11
			};

			ibool irqEnabled;
			uint frames;
			dword seconds;
			dword time;
			CartSwitches cartSwitches;
			char text[32];
		};
	}
}

#endif
