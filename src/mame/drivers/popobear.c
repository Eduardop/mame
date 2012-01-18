/* Popo Bear */


#include "emu.h"
#include "cpu/m68000/m68000.h"
#include "sound/okim6295.h"


class popobear_state : public driver_device
{
public:
	popobear_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_maincpu(*this,"maincpu")
		{ }


	UINT16 *popobear_vram;
	required_device<cpu_device> m_maincpu;
};

READ16_HANDLER( unk_r )
{
	static int i = 0x00;
	i ^=0xff;
	return i;
}

static ADDRESS_MAP_START( popobear_mem, AS_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x03ffff) AM_ROM
	AM_RANGE(0x210000, 0x21ffff) AM_RAM
	AM_RANGE(0x3E0000, 0x3EFFFF) AM_RAM  AM_BASE_MEMBER(popobear_state, popobear_vram) 

	
	AM_RANGE(0x500000, 0x500001) AM_READ(unk_r)

ADDRESS_MAP_END

static INPUT_PORTS_START( popobear )
INPUT_PORTS_END


static const gfx_layout tilelayout =
{
	4,8,
	RGN_FRAC(1,1),
	8,
	{ 0,1,2,3,4,5,6,7 },
	{ 24, 16, 8, 0 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32 },
	8*32
};

static GFXDECODE_START( popobear )
	GFXDECODE_ENTRY( "gfx1", 0, tilelayout,  0, 1 )
GFXDECODE_END


SCREEN_UPDATE_IND16( popobear )
{
	popobear_state *state = screen.machine().driver_data<popobear_state>();
	bitmap.fill(0, cliprect);

	UINT16* popobear_vram = state->popobear_vram;
	int count = 0;
	for (int y=0;y<256;y++)
	{
		for (int x=0;x<128;x++)
		{
			UINT8 dat;
			dat = (popobear_vram[count]&0xf000)>>12;
			bitmap.pix16(y, (x*4)+0) =dat;

			dat = (popobear_vram[count]&0x0f00)>>8;
			bitmap.pix16(y, (x*4)+1) =dat;

			dat = (popobear_vram[count]&0x00f0)>>4;
			bitmap.pix16(y, (x*4)+2) =dat;

			dat = (popobear_vram[count]&0x000f)>>0;
			bitmap.pix16(y, (x*4)+3) =dat;

			count++;
		}
	}

	return 0;
}

VIDEO_START(popobear)
{

}

static MACHINE_CONFIG_START( popobear, popobear_state )
	MCFG_CPU_ADD("maincpu", M68000, 10000000 )
	MCFG_CPU_PROGRAM_MAP(popobear_mem) 
	// levels 2,3,5 look interesting
	MCFG_CPU_VBLANK_INT("screen",irq2_line_hold)

	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(60)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(2500)) /* not accurate */
	MCFG_SCREEN_UPDATE_STATIC(popobear)

	MCFG_GFXDECODE(popobear)

	MCFG_SCREEN_SIZE(64*8, 32*8)
	MCFG_SCREEN_VISIBLE_AREA(0*8, 64*8-1, 0*8, 32*8-1)
	MCFG_PALETTE_LENGTH(256)

	MCFG_VIDEO_START(popobear)

	MCFG_SPEAKER_STANDARD_STEREO("lspeaker", "rspeaker")

	MCFG_OKIM6295_ADD("oki", 1000000, OKIM6295_PIN7_LOW)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "lspeaker", 0.50)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "rspeaker", 0.50)
MACHINE_CONFIG_END


ROM_START( popobear )
	ROM_REGION( 0x400000, "maincpu", 0 ) /* 68000 Code */
	ROM_LOAD16_BYTE( "popobear_en-a-301_1.6.u3", 0x000001, 0x20000, CRC(b934adf6) SHA1(93431c7a19af812b549aad35cc1176a81805ffab) )
	ROM_LOAD16_BYTE( "popobear_en-a-401_1.6.u4", 0x000000, 0x20000, CRC(0568af9c) SHA1(920531dbc4bbde2d1db062bd5c48b97dd50b7185) )

	ROM_REGION( 0x400000, "gfx1", 0 ) /* tiles */
	ROM_LOAD16_BYTE(	"popobear_en-a-501.u5", 0x000000, 0x100000, CRC(185901a9) SHA1(7ff82b5751645df53435eaa66edce589684cc5c7) )
	ROM_LOAD16_BYTE(	"popobear_en-a-601.u6", 0x000001, 0x100000, CRC(84fa9f3f) SHA1(34dd7873f88b0dae5fb81fe84e82d2b6b49f7332) )
	ROM_LOAD16_BYTE(	"popobear_en-a-701.u7", 0x200000, 0x100000, CRC(45eba6d0) SHA1(0278602ed57ac45040619d590e6cc85e2cfeed31) )
	ROM_LOAD16_BYTE(	"popobear_en-a-801.u8", 0x200001, 0x100000, CRC(2760f2e6) SHA1(58af59f486c9df930f7c124f89154f8f389a5bd7) )

	ROM_REGION( 0x040000, "oki", 0 ) /* Samples */
	ROM_LOAD( "popobear_ta-a-901.u9", 0x00000, 0x40000,  CRC(f1e94926) SHA1(f4d6f5b5811d90d0069f6efbb44d725ff0d07e1c) )
ROM_END

GAME( 2000, popobear,    0, popobear,    popobear,    0, ROT0,  "BMC", "PoPo Bear", GAME_NOT_WORKING | GAME_IS_SKELETON )
