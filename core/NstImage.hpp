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

#ifndef NST_IMAGE_H
#define NST_IMAGE_H

#ifdef NST_PRAGMA_ONCE
#pragma once
#endif

namespace Nes
{
	namespace Core
	{
		namespace State
		{
			class Loader;
			class Saver;
		}

		class ImageDatabase;
		class Cpu;
		class Apu;
		class Ppu;

		class NST_NO_VTABLE Image
		{
		public:

			enum Type
			{
				UNKNOWN   = 0x0,
				CARTRIDGE = 0x1,
				DISK      = 0x2,
				SOUND     = 0x4
			};

			typedef void* ExternalDevice;

			enum ExternalDeviceType
			{
				EXT_DIP_SWITCHES = 1,
				EXT_DATA_RECORDER,
				EXT_BARCODE_READER,
				EXT_TURBO_FILE
			};

			struct Context
			{
				const Type type;
				Cpu& cpu;
				Apu& apu;
				Ppu& ppu;
				StdStream const stream;
				const ImageDatabase* const database;
				Result result;

				Context(Type t,Cpu& c,Apu& a,Ppu& p,StdStream s,const ImageDatabase* d=NULL)
				: type(t), cpu(c), apu(a), ppu(p), stream(s), database(d), result(RESULT_OK) {}
			};

			static Image* Load(Context&);
			static void Unload(Image*);

			virtual void Reset(bool) = 0;

			virtual bool PowerOff()
			{
				return true;
			}

			virtual void VSync() {}

			virtual void LoadState(State::Loader&) {}
			virtual void SaveState(State::Saver&,dword) const {}

			virtual uint GetDesiredController(uint) const;
			virtual uint GetDesiredAdapter() const;

			virtual Region::Type GetRegion() const = 0;
			virtual void SetRegion(Region::Type) {}

			virtual dword GetPrgCrc() const
			{
				return 0;
			}

			virtual ExternalDevice QueryExternalDevice(ExternalDeviceType)
			{
				return NULL;
			}

			virtual Revision::Ppu QueryPpu(bool)
			{
				return Revision::PPU_RP2C02;
			}

		protected:

			explicit Image(Type);
			virtual ~Image() {}

			enum
			{
				INES_ID    = AsciiId<'N','E','S'>::V | 0x1AUL << 24,
				UNIF_ID    = AsciiId<'U','N','I','F'>::V,
				FDS_ID     = AsciiId<'F','D','S'>::V | 0x1AUL << 24,
				FDS_RAW_ID = 0x494E2A01,
				NSF_ID     = AsciiId<'N','E','S','M'>::V
			};

		private:

			const Type type;

		public:

			Type GetType() const
			{
				return type;
			}
		};
	}
}

#endif
