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

#ifndef NST_MAPPER_H
#define NST_MAPPER_H

#include "NstCpu.hpp"
#include "NstPpu.hpp"
#include "NstState.hpp"

#ifdef NST_PRAGMA_ONCE
#pragma once
#endif

namespace Nes
{
	namespace Core
	{
		namespace Input
		{
			class Controllers;
		}

		class NST_NO_VTABLE Mapper
		{
		public:

			struct Context
			{
				const uint id;

				Cpu& cpu;
				Apu& apu;
				Ppu& ppu;

				Ram& prg;
				Ram& chr;
				Ram& wrk;

				const Ppu::Mirroring mirroring;
				const dword wrkBacked;
				const uint attribute;
				dword chrRam;
				bool wrkAuto;

				Context
				(
					uint i,
					Cpu& c,
					Apu& a,
					Ppu& p,
					Ram& pr,
					Ram& cr,
					Ram& wr,
					Ppu::Mirroring m,
					dword b,
					uint at
				)
				:
				id        (i),
				cpu       (c),
				apu       (a),
				ppu       (p),
				prg       (pr),
				chr       (cr),
				wrk       (wr),
				mirroring (m),
				wrkBacked (b),
				attribute (at),
				chrRam    (0),
				wrkAuto   (false)
				{}
			};

			static Mapper* Create(Context&);
			static void Destroy(Mapper*);

			void Reset     (bool);
			void SaveState (State::Saver&,dword) const;
			void LoadState (State::Loader&);

			enum Event
			{
				EVENT_END_FRAME,
				EVENT_BEGIN_FRAME,
				EVENT_POWER_OFF
			};

			virtual void Sync(Event,Input::Controllers*) {};

			typedef void* Device;

			enum DeviceType
			{
				DEVICE_DIP_SWITCHES = 1,
				DEVICE_BARCODE_READER
			};

			virtual Device QueryDevice(DeviceType)
			{
				return NULL;
			}

			enum
			{
				EXT_SUPER24IN1 = 256,
				EXT_8157,
				EXT_8237,
				EXT_WS,
				EXT_DREAMTECH01,
				EXT_CC21,
				EXT_KOF97,
				EXT_64IN1NR,
				EXT_STREETHEROES,
				EXT_T262,
				EXT_FK23C,
				EXT_6035052,
				EXT_A65AS,
				EXT_EDU2000,
				EXT_TF1201,
				EXT_GS2004,
				EXT_AX5705,
				EXT_T230,
				EXT_190IN1,
				EXT_CTC65,
				NUM_EXT_MAPPERS = 20
			};

			static cstring GetBoard(uint);

		protected:

			explicit Mapper(Context&,dword=0);
			virtual ~Mapper();

			enum
			{
				PROM_DEFAULT   = 0U   << 0,
				PROM_MAX_16K   = 1U   << 0,
				PROM_MAX_32K   = 2U   << 0,
				PROM_MAX_64K   = 3U   << 0,
				PROM_MAX_128K  = 4U   << 0,
				PROM_MAX_256K  = 5U   << 0,
				PROM_MAX_512K  = 6U   << 0,
				PROM_MAX_1024K = 7U   << 0,
				PROM_SETTINGS  = 7U   << 0,
				CROM_DEFAULT   = 0U   << 3,
				CROM_NONE      = 1U   << 3,
				CROM_MAX_8K    = 2U   << 3,
				CROM_MAX_16K   = 3U   << 3,
				CROM_MAX_32K   = 4U   << 3,
				CROM_MAX_64K   = 5U   << 3,
				CROM_MAX_128K  = 6U   << 3,
				CROM_MAX_256K  = 7U   << 3,
				CROM_MAX_512K  = 8U   << 3,
				CROM_MAX_1024K = 9U   << 3,
				CROM_SETTINGS  = 15U  << 3,
				CRAM_DEFAULT   = 0U   << 7,
				CRAM_1K        = 1U   << 7,
				CRAM_2K        = 2U   << 7,
				CRAM_4K        = 3U   << 7,
				CRAM_8K        = 4U   << 7,
				CRAM_16K       = 5U   << 7,
				CRAM_32K       = 6U   << 7,
				CRAM_SETTINGS  = 7U   << 7,
				WRAM_AUTO      = 0U   << 10,
				WRAM_DEFAULT   = 1U   << 10,
				WRAM_NONE      = 2U   << 10,
				WRAM_1K        = 3U   << 10,
				WRAM_2K        = 4U   << 10,
				WRAM_4K        = 5U   << 10,
				WRAM_8K        = 6U   << 10,
				WRAM_16K       = 7U   << 10,
				WRAM_32K       = 8U   << 10,
				WRAM_40K       = 9U   << 10,
				WRAM_64K       = 10U  << 10,
				WRAM_SETTINGS  = 15U  << 10,
				NMT_DEFAULT    = 0UL  << 14,
				NMT_HORIZONTAL = 1UL  << 14,
				NMT_VERTICAL   = 2UL  << 14,
				NMT_ZERO       = 3UL  << 14,
				NMT_FOURSCREEN = 4UL  << 14,
				NMT_SETTINGS   = 7UL  << 14
			};

			enum PrgMapping
			{
				PRG_SWAP_8K_0,
				PRG_SWAP_8K_1,
				PRG_SWAP_8K_2,
				PRG_SWAP_8K_3,
				PRG_SWAP_16K_0,
				PRG_SWAP_16K_1,
				PRG_SWAP_32K
			};

			enum ChrMapping
			{
				CHR_SWAP_1K_0,
				CHR_SWAP_1K_1,
				CHR_SWAP_1K_2,
				CHR_SWAP_1K_3,
				CHR_SWAP_1K_4,
				CHR_SWAP_1K_5,
				CHR_SWAP_1K_6,
				CHR_SWAP_1K_7,
				CHR_SWAP_2K_0,
				CHR_SWAP_2K_1,
				CHR_SWAP_2K_2,
				CHR_SWAP_2K_3,
				CHR_SWAP_4K_0,
				CHR_SWAP_4K_1,
				CHR_SWAP_8K
			};

			enum NmtMapping
			{
				NMT_SWAP_HV,
				NMT_SWAP_VH,
				NMT_SWAP_VH01,
				NMT_SWAP_HV01
			};

			enum WrkMapping
			{
				WRK_PEEK,
				WRK_POKE,
				WRK_PEEK_POKE,
				WRK_SAFE_PEEK,
				WRK_SAFE_POKE,
				WRK_SAFE_PEEK_POKE
			};

			enum NopMapping
			{
				NOP_PEEK,
				NOP_POKE,
				NOP_PEEK_POKE
			};

			void Map(WrkMapping) const;

			typedef Memory<SIZE_32K,SIZE_8K,2> Prg;
			typedef Ppu::ChrMem Chr;
			typedef Ppu::NmtMem Nmt;

			struct Wrk : Memory<SIZE_8K,SIZE_8K,2>
			{
				bool Available() const
				{
					return Source(0).Mem() != Source(1).Mem();
				}

				dword Size() const
				{
					return Available() ? Source(0).Size() : 0;
				}
			};

			Prg prg;
			Cpu& cpu;
			Ppu& ppu;
			Chr& chr;
			Nmt& nmt;
			Wrk wrk;
			const word id;
			const word mirroring;

		private:

			dword GetStateName() const;

			virtual void SubReset(bool) = 0;
			virtual void SubSave(State::Saver&) const {}
			virtual void SubLoad(State::Loader&) {}
			virtual void BaseSave(State::Saver&) const {}
			virtual void BaseLoad(State::Loader&,dword) {}

			NES_DECL_PEEK( Prg_8 );
			NES_DECL_PEEK( Prg_A );
			NES_DECL_PEEK( Prg_C );
			NES_DECL_PEEK( Prg_E );

			NES_DECL_POKE( Prg_8k_0  );
			NES_DECL_POKE( Prg_8k_1  );
			NES_DECL_POKE( Prg_8k_2  );
			NES_DECL_POKE( Prg_8k_3  );
			NES_DECL_POKE( Prg_16k_0 );
			NES_DECL_POKE( Prg_16k_1 );
			NES_DECL_POKE( Prg_32k   );

			NES_DECL_POKE( Chr_1k_0 );
			NES_DECL_POKE( Chr_1k_1 );
			NES_DECL_POKE( Chr_1k_2 );
			NES_DECL_POKE( Chr_1k_3 );
			NES_DECL_POKE( Chr_1k_4 );
			NES_DECL_POKE( Chr_1k_5 );
			NES_DECL_POKE( Chr_1k_6 );
			NES_DECL_POKE( Chr_1k_7 );
			NES_DECL_POKE( Chr_2k_0 );
			NES_DECL_POKE( Chr_2k_1 );
			NES_DECL_POKE( Chr_2k_2 );
			NES_DECL_POKE( Chr_2k_3 );
			NES_DECL_POKE( Chr_4k_0 );
			NES_DECL_POKE( Chr_4k_1 );
			NES_DECL_POKE( Chr_8k   );

			NES_DECL_PEEK( Wrk_6 );
			NES_DECL_POKE( Wrk_6 );
			NES_DECL_PEEK( Wrk_Safe_6 );
			NES_DECL_POKE( Wrk_Safe_6 );

			NES_DECL_POKE( Nmt_Hv );
			NES_DECL_POKE( Nmt_Vh );
			NES_DECL_POKE( Nmt_Vh01 );
			NES_DECL_POKE( Nmt_Hv01 );

			NES_DECL_PEEK( Nop );
			NES_DECL_POKE( Nop );

			static cstring const boards[256+NUM_EXT_MAPPERS];

		protected:

			void SetMirroringHV(uint data) { NES_DO_POKE(Nmt_Hv,0,data); }
			void SetMirroringVH(uint data) { NES_DO_POKE(Nmt_Vh,0,data); }

			void SetMirroringVH01(uint data) { NES_DO_POKE(Nmt_Vh01,0,data); }
			void SetMirroringHV01(uint data) { NES_DO_POKE(Nmt_Hv01,0,data); }

			template<typename T>
			void Map(uint first,uint last,T t) const
			{
				cpu.Map( first, last ).Set( t );
			}

			template<typename T,typename U>
			void Map(uint first,uint last,T t,U u) const
			{
				cpu.Map( first, last ).Set( t, u );
			}

			template<typename T>
			void Map(uint address,T t) const
			{
				Map( address, address, t );
			}

			template<typename T,typename U>
			void Map(uint address,T t,U u) const
			{
				cpu.Map( address ).Set( t, u );
			}
		};

		template<>
		inline void Mapper::Map(uint first,uint last,NopMapping m) const
		{
			switch (m)
			{
				case NOP_PEEK:      cpu.Map( first, last ).Set( &Mapper::Peek_Nop ); break;
				case NOP_POKE:      cpu.Map( first, last ).Set( &Mapper::Poke_Nop ); break;
				case NOP_PEEK_POKE: cpu.Map( first, last ).Set( &Mapper::Peek_Nop, &Mapper::Poke_Nop ); break;
			}
		}

		template<>
		inline void Mapper::Map(uint first,uint last,PrgMapping mapping) const
		{
			cpu.Map( first, last ).Set
			(
				mapping == PRG_SWAP_8K_0  ? &Mapper::Poke_Prg_8k_0 :
				mapping == PRG_SWAP_8K_1  ? &Mapper::Poke_Prg_8k_1 :
				mapping == PRG_SWAP_8K_2  ? &Mapper::Poke_Prg_8k_2 :
				mapping == PRG_SWAP_8K_3  ? &Mapper::Poke_Prg_8k_3 :
				mapping == PRG_SWAP_16K_0 ? &Mapper::Poke_Prg_16k_0 :
				mapping == PRG_SWAP_16K_1 ? &Mapper::Poke_Prg_16k_1 :
											&Mapper::Poke_Prg_32k
			);
		}

		template<>
		inline void Mapper::Map(uint first,uint last,ChrMapping mapping) const
		{
			cpu.Map( first, last ).Set
			(
				mapping == CHR_SWAP_1K_0 ? &Mapper::Poke_Chr_1k_0 :
				mapping == CHR_SWAP_1K_1 ? &Mapper::Poke_Chr_1k_1 :
				mapping == CHR_SWAP_1K_2 ? &Mapper::Poke_Chr_1k_2 :
				mapping == CHR_SWAP_1K_3 ? &Mapper::Poke_Chr_1k_3 :
				mapping == CHR_SWAP_1K_4 ? &Mapper::Poke_Chr_1k_4 :
				mapping == CHR_SWAP_1K_5 ? &Mapper::Poke_Chr_1k_5 :
				mapping == CHR_SWAP_1K_6 ? &Mapper::Poke_Chr_1k_6 :
				mapping == CHR_SWAP_1K_7 ? &Mapper::Poke_Chr_1k_7 :
				mapping == CHR_SWAP_2K_0 ? &Mapper::Poke_Chr_2k_0 :
				mapping == CHR_SWAP_2K_1 ? &Mapper::Poke_Chr_2k_1 :
				mapping == CHR_SWAP_2K_2 ? &Mapper::Poke_Chr_2k_2 :
				mapping == CHR_SWAP_2K_3 ? &Mapper::Poke_Chr_2k_3 :
				mapping == CHR_SWAP_4K_0 ? &Mapper::Poke_Chr_4k_0 :
				mapping == CHR_SWAP_4K_1 ? &Mapper::Poke_Chr_4k_1 :
                                           &Mapper::Poke_Chr_8k
			);
		}

		template<>
		inline void Mapper::Map(uint first,uint last,NmtMapping mapping) const
		{
			cpu.Map( first, last ).Set
			(
				mapping == NMT_SWAP_HV   ? &Mapper::Poke_Nmt_Hv :
				mapping == NMT_SWAP_VH   ? &Mapper::Poke_Nmt_Vh :
				mapping == NMT_SWAP_VH01 ? &Mapper::Poke_Nmt_Vh01 :
                                           &Mapper::Poke_Nmt_Hv01
			);
		}
	}
}

#endif
