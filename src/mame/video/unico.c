/***************************************************************************

                              -= Unico Games =-

                    driver by   Luca Elia (l.elia@tin.it)


Note:   if MAME_DEBUG is defined, pressing Z with:

        Q / W / E       Shows Layer 0 / 1 / 2
        A               Shows Sprites

        Keys can be used together!


    [ 3 Scrolling Layers ]

        Tile Size:              16 x 16 x 8
        Layer Size (tiles):     64 x 64

    [ 512 Sprites ]

        Sprites are made of 16 x 16 x 8 tiles. Size can vary from 1 to
        16 tiles horizontally, while their height is always 1 tile.
        There seems to be 4 levels of priority (wrt layers) for each
        sprite, following this simple scheme:

        [if we denote the three layers with 0-3 (0 being the backmost)
         and the sprite with S]

        Sprite Priority         Order (back -> front)
                0                   S 0 1 2
                1                   0 S 1 2
                2                   0 1 S 2
                3                   0 1 2 S

***************************************************************************/

#include "emu.h"
#include "includes/unico.h"


/***************************************************************************

                                    Palette

    Byte:   0   1   2   3
    Gun:    R   G   B   0

    6 Bits x Gun

***************************************************************************/

WRITE16_HANDLER( unico_palette_w )
{
	UINT16 data1, data2;
	COMBINE_DATA(&space->machine->generic.paletteram.u16[offset]);
	data1 = space->machine->generic.paletteram.u16[offset & ~1];
	data2 = space->machine->generic.paletteram.u16[offset |  1];
	palette_set_color_rgb( space->machine,offset/2,
		 (data1 >> 8) & 0xFC,
		 (data1 >> 0) & 0xFC,
		 (data2 >> 8) & 0xFC	);
}

WRITE32_HANDLER( unico_palette32_w )
{
	UINT32 rgb0 = COMBINE_DATA(&space->machine->generic.paletteram.u32[offset]);
	palette_set_color_rgb( space->machine,offset,
		 (rgb0 >> 24) & 0xFC,
		 (rgb0 >> 16) & 0xFC,
		 (rgb0 >>  8) & 0xFC	);
}


/***************************************************************************

                                Tilemaps

    Offset:     Bits:                   Value:

        0.w                             Code
        2.w     fedc ba98 7--- ----
                ---- ---- -6-- ----     Flip Y
                ---- ---- --5- ----     Flip X
                ---- ---- ---4 3210     Color

***************************************************************************/


static TILE_GET_INFO( get_tile_info )
{
	UINT16 *vram = (UINT16 *)param;
	UINT16 code = vram[2 * tile_index + 0 ];
	UINT16 attr = vram[2 * tile_index + 1 ];
	SET_TILE_INFO(1, code, attr & 0x1f, TILE_FLIPYX( attr >> 5 ));
}

static TILE_GET_INFO( get_tile_info32 )
{
	UINT32 *vram = (UINT32 *)param;
	UINT16 code = vram[tile_index] >> 16;
	UINT16 attr = vram[tile_index] & 0xff;
	SET_TILE_INFO(1, code, attr & 0x1f, TILE_FLIPYX( attr >> 5 ));
}

WRITE16_HANDLER( unico_vram_w )
{
	unico_state *state = space->machine->driver_data<unico_state>();
	UINT16 *vram = state->vram;
	int tile = ((offset / 0x2000) + 1) % 3;
	COMBINE_DATA(&vram[offset]);
	tilemap_mark_tile_dirty(state->tilemap[tile],(offset & 0x3fff)/2);
}

WRITE32_HANDLER( unico_vram32_w )
{
	unico_state *state = space->machine->driver_data<unico_state>();
	UINT32 *vram = state->vram32;
	int tile = ((offset / 0x1000) + 1) % 3;
	COMBINE_DATA(&vram[offset]);
	tilemap_mark_tile_dirty(state->tilemap[tile],(offset & 0x3fff));
}



/***************************************************************************


                            Video Hardware Init


***************************************************************************/


VIDEO_START( unico )
{
	unico_state *state = machine->driver_data<unico_state>();
	state->tilemap[0] = tilemap_create(	machine, get_tile_info,tilemap_scan_rows,
									16,16,	0x40, 0x40);

	state->tilemap[1] = tilemap_create(	machine, get_tile_info,tilemap_scan_rows,
									16,16,	0x40, 0x40);

	state->tilemap[2] = tilemap_create(	machine, get_tile_info,tilemap_scan_rows,
									16,16,	0x40, 0x40);

	tilemap_set_user_data(state->tilemap[0], &state->vram[0x8000/2]);
	tilemap_set_user_data(state->tilemap[1], &state->vram[0x0000/2]);
	tilemap_set_user_data(state->tilemap[2], &state->vram[0x4000/2]);

	state->sprites_scrolldx = -0x3f;
	state->sprites_scrolldy = -0x0e;

	tilemap_set_scrolldx(state->tilemap[0],-0x32,0);
	tilemap_set_scrolldx(state->tilemap[1],-0x30,0);
	tilemap_set_scrolldx(state->tilemap[2],-0x2e,0);

	tilemap_set_scrolldy(state->tilemap[0],-0x0f,0);
	tilemap_set_scrolldy(state->tilemap[1],-0x0f,0);
	tilemap_set_scrolldy(state->tilemap[2],-0x0f,0);

	tilemap_set_transparent_pen(state->tilemap[0],0x00);
	tilemap_set_transparent_pen(state->tilemap[1],0x00);
	tilemap_set_transparent_pen(state->tilemap[2],0x00);
}

VIDEO_START( zeropnt2 )
{
	unico_state *state = machine->driver_data<unico_state>();
	state->tilemap[0] = tilemap_create(	machine, get_tile_info32,tilemap_scan_rows,
									16,16,	0x40, 0x40);

	state->tilemap[1] = tilemap_create(	machine, get_tile_info32,tilemap_scan_rows,
									16,16,	0x40, 0x40);

	state->tilemap[2] = tilemap_create(	machine, get_tile_info32,tilemap_scan_rows,
									16,16,	0x40, 0x40);

	tilemap_set_user_data(state->tilemap[0], &state->vram32[0x8000/4]);
	tilemap_set_user_data(state->tilemap[1], &state->vram32[0x0000/4]);
	tilemap_set_user_data(state->tilemap[2], &state->vram32[0x4000/4]);

	state->sprites_scrolldx = -0x3f;
	state->sprites_scrolldy = -0x0e;

	tilemap_set_scrolldx(state->tilemap[0],-0x32,0);
	tilemap_set_scrolldx(state->tilemap[1],-0x30,0);
	tilemap_set_scrolldx(state->tilemap[2],-0x2e,0);

	tilemap_set_scrolldy(state->tilemap[0],-0x0f,0);
	tilemap_set_scrolldy(state->tilemap[1],-0x0f,0);
	tilemap_set_scrolldy(state->tilemap[2],-0x0f,0);

	tilemap_set_transparent_pen(state->tilemap[0],0x00);
	tilemap_set_transparent_pen(state->tilemap[1],0x00);
	tilemap_set_transparent_pen(state->tilemap[2],0x00);
}



/***************************************************************************

                                Sprites Drawing


        0.w                             X

        2.w                             Y

        4.w                             Code

        6.w     fe-- ---- ---- ----
                --dc ---- ---- ----     Priority
                ---- ba98 ---- ----     Number of tiles along X, minus 1
                ---- ---- 7--- ----
                ---- ---- -6-- ----     Flip Y?
                ---- ---- --5- ----     Flip X
                ---- ---- ---4 3210     Color


***************************************************************************/

static void unico_draw_sprites(running_machine *machine, bitmap_t *bitmap,const rectangle *cliprect)
{
	unico_state *state = machine->driver_data<unico_state>();
	UINT16 *spriteram16 = machine->generic.spriteram.u16;
	int offs;

	/* Draw them backwards, for pdrawgfx */
	for ( offs = (machine->generic.spriteram_size-8)/2; offs >= 0 ; offs -= 8/2 )
	{
		int x, startx, endx, incx;

		int	sx			=	spriteram16[ offs + 0 ];
		int	sy			=	spriteram16[ offs + 1 ];
		int	code		=	spriteram16[ offs + 2 ];
		int	attr		=	spriteram16[ offs + 3 ];

		int	flipx		=	attr & 0x020;
		int	flipy		=	attr & 0x040;	// not sure

		int dimx		=	((attr >> 8) & 0xf) + 1;

		int priority	=	((attr >> 12) & 0x3);
		int pri_mask;

		switch( priority )
		{
			case 0:		pri_mask = 0xfe;	break;	// below all
			case 1:		pri_mask = 0xf0;	break;	// above layer 0
			case 2:		pri_mask = 0xfc;	break;	// above layer 1
			default:
			case 3:		pri_mask = 0x00;			// above all
		}

		sx	+=	state->sprites_scrolldx;
		sy	+=	state->sprites_scrolldy;

		sx	=	(sx & 0x1ff) - (sx & 0x200);
		sy	=	(sy & 0x1ff) - (sy & 0x200);

		if (flipx)	{	startx = sx+(dimx-1)*16;	endx = sx-16;		incx = -16;	}
		else		{	startx = sx;				endx = sx+dimx*16;	incx = +16;	}

		for (x = startx ; x != endx ; x += incx)
		{
			pdrawgfx_transpen(	bitmap, cliprect, machine->gfx[0],
						code++,
						attr & 0x1f,
						flipx, flipy,
						x, sy,
						machine->priority_bitmap,
						pri_mask,0x00	);
		}
	}
}

static void zeropnt2_draw_sprites(running_machine *machine, bitmap_t *bitmap,const rectangle *cliprect)
{
	unico_state *state = machine->driver_data<unico_state>();
	UINT32 *spriteram32 = machine->generic.spriteram.u32;
	int offs;

	/* Draw them backwards, for pdrawgfx */
	for ( offs = (machine->generic.spriteram_size-8)/4; offs >= 0 ; offs -= 8/4 )
	{
		int x, startx, endx, incx;

		int	sx			=	spriteram32[ offs + 0 ] >> 16;
		int	sy			=	spriteram32[ offs + 0 ] & 0xffff;
		int	code		=	spriteram32[ offs + 1 ] >> 16;
		int	attr		=	spriteram32[ offs + 1 ] & 0xffff;

		int	flipx		=	attr & 0x020;
		int	flipy		=	attr & 0x040;	// not sure

		int dimx		=	((attr >> 8) & 0xf) + 1;

		int priority	=	((attr >> 12) & 0x3);
		int pri_mask;

		switch( priority )
		{
			case 0:		pri_mask = 0xfe;	break;	// below all
			case 1:		pri_mask = 0xf0;	break;	// above layer 0
			case 2:		pri_mask = 0xfc;	break;	// above layer 1
			default:
			case 3:		pri_mask = 0x00;			// above all
		}

		sx	+=	state->sprites_scrolldx;
		sy	+=	state->sprites_scrolldy;

		sx	=	(sx & 0x1ff) - (sx & 0x200);
		sy	=	(sy & 0x1ff) - (sy & 0x200);

		if (flipx)	{	startx = sx+(dimx-1)*16;	endx = sx-16;		incx = -16;	}
		else		{	startx = sx;				endx = sx+dimx*16;	incx = +16;	}

		for (x = startx ; x != endx ; x += incx)
		{
			pdrawgfx_transpen(	bitmap, cliprect, machine->gfx[0],
						code++,
						attr & 0x1f,
						flipx, flipy,
						x, sy,
						machine->priority_bitmap,
						pri_mask,0x00	);
		}
	}
}



/***************************************************************************


                                Screen Drawing


***************************************************************************/

SCREEN_UPDATE( unico )
{
	unico_state *state = screen->machine->driver_data<unico_state>();
	int layers_ctrl = -1;

	tilemap_set_scrollx(state->tilemap[0], 0, state->scroll[0x00]);
	tilemap_set_scrolly(state->tilemap[0], 0, state->scroll[0x01]);

	tilemap_set_scrollx(state->tilemap[1], 0, state->scroll[0x05]);
	tilemap_set_scrolly(state->tilemap[1], 0, state->scroll[0x0a]);

	tilemap_set_scrollx(state->tilemap[2], 0, state->scroll[0x04]);
	tilemap_set_scrolly(state->tilemap[2], 0, state->scroll[0x02]);

#ifdef MAME_DEBUG
if ( input_code_pressed(screen->machine, KEYCODE_Z) || input_code_pressed(screen->machine, KEYCODE_X) )
{
	int msk = 0;
	if (input_code_pressed(screen->machine, KEYCODE_Q))	msk |= 1;
	if (input_code_pressed(screen->machine, KEYCODE_W))	msk |= 2;
	if (input_code_pressed(screen->machine, KEYCODE_E))	msk |= 4;
	if (input_code_pressed(screen->machine, KEYCODE_A))	msk |= 8;
	if (msk != 0) layers_ctrl &= msk;
}
#endif

	/* The background color is the first of the last palette */
	bitmap_fill(bitmap,cliprect,0x1f00);
	bitmap_fill(screen->machine->priority_bitmap,cliprect,0);

	if (layers_ctrl & 1)	tilemap_draw(bitmap,cliprect,state->tilemap[0],0,1);
	if (layers_ctrl & 2)	tilemap_draw(bitmap,cliprect,state->tilemap[1],0,2);
	if (layers_ctrl & 4)	tilemap_draw(bitmap,cliprect,state->tilemap[2],0,4);

	/* Sprites are drawn last, using pdrawgfx */
	if (layers_ctrl & 8)	unico_draw_sprites(screen->machine, bitmap,cliprect);

	return 0;
}

SCREEN_UPDATE( zeropnt2 )
{
	unico_state *state = screen->machine->driver_data<unico_state>();
	int layers_ctrl = -1;

	tilemap_set_scrollx(state->tilemap[0], 0, state->scroll32[0] >> 16);
	tilemap_set_scrolly(state->tilemap[0], 0, state->scroll32[0] & 0xffff);

	tilemap_set_scrollx(state->tilemap[1], 0, state->scroll32[2] & 0xffff);
	tilemap_set_scrolly(state->tilemap[1], 0, state->scroll32[5] >> 16);

	tilemap_set_scrollx(state->tilemap[2], 0, state->scroll32[2] >> 16);
	tilemap_set_scrolly(state->tilemap[2], 0, state->scroll32[1] >> 16);

#ifdef MAME_DEBUG
if ( input_code_pressed(screen->machine, KEYCODE_Z) || input_code_pressed(screen->machine, KEYCODE_X) )
{
	int msk = 0;
	if (input_code_pressed(screen->machine, KEYCODE_Q))	msk |= 1;
	if (input_code_pressed(screen->machine, KEYCODE_W))	msk |= 2;
	if (input_code_pressed(screen->machine, KEYCODE_E))	msk |= 4;
	if (input_code_pressed(screen->machine, KEYCODE_A))	msk |= 8;
	if (msk != 0) layers_ctrl &= msk;
}
#endif

	/* The background color is the first of the last palette */
	bitmap_fill(bitmap,cliprect,0x1f00);
	bitmap_fill(screen->machine->priority_bitmap,cliprect,0);

	if (layers_ctrl & 1)	tilemap_draw(bitmap,cliprect,state->tilemap[0],0,1);
	if (layers_ctrl & 2)	tilemap_draw(bitmap,cliprect,state->tilemap[1],0,2);
	if (layers_ctrl & 4)	tilemap_draw(bitmap,cliprect,state->tilemap[2],0,4);

	/* Sprites are drawn last, using pdrawgfx */
	if (layers_ctrl & 8)	zeropnt2_draw_sprites(screen->machine, bitmap,cliprect);

	return 0;
}

