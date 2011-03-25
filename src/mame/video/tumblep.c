/***************************************************************************

   Tumblepop Video emulation - Bryan McPhail, mish@tendril.co.uk

*********************************************************************

Uses Data East custom chip 55 for backgrounds, custom chip 52 for sprites.

See Dark Seal & Caveman Ninja drivers for info on these chips.

Tumblepop is one of few games to take advantage of the playfields ability
to switch between 8*8 tiles and 16*16 tiles.

***************************************************************************/

#include "emu.h"
#include "video/deco16ic.h"
#include "includes/tumblep.h"
#include "video/decospr.h"


SCREEN_UPDATE( tumblep )
{
	tumblep_state *state = screen->machine->driver_data<tumblep_state>();
	UINT16 flip = deco16ic_pf_control_r(state->deco16ic, 0, 0xffff);

	flip_screen_set(screen->machine, BIT(flip, 7));
	deco16ic_pf_update(state->deco16ic, state->pf1_rowscroll, state->pf2_rowscroll);

	bitmap_fill(bitmap, cliprect, 256); /* not verified */

	deco16ic_tilemap_2_draw(state->deco16ic, bitmap, cliprect, TILEMAP_DRAW_OPAQUE, 0);
	deco16ic_tilemap_1_draw(state->deco16ic, bitmap, cliprect, 0, 0);

	screen->machine->device<decospr_device>("spritegen")->draw_sprites(screen->machine, bitmap, cliprect, state->spriteram, 0x400);
	return 0;
}
