#ifdef USE_WIIC

/*
 *    ir.h
 *
 *	  This file is part of WiiC, written by:
 *		Gabriele Randelli
 *		Email: randelli@dis.uniroma1.it
 *
 *    Copyright 2010
 *		
 *	  This file is based on Wiiuse, written By:
 *		Michael Laforest	< para >
 *		Email: < thepara (--AT--) g m a i l [--DOT--] com >
 *
 *	  Copyright 2006-2007
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *	  $Header: /home/wis/Unix/cvsroot/WiiVolume/WiiVolume/wiic/ir.h,v 1.2 2014/06/23 08:16:25 wis Exp $
 */

/**
 *	@file
 *	@brief Handles IR data.
 */

#ifndef IR_H_INCLUDED
#define IR_H_INCLUDED

#include "wiic_internal.h"

#define WII_VRES_X		560
#define WII_VRES_Y		340

#ifdef __cplusplus
extern "C" {
#endif

void calculate_basic_ir(struct wiimote_t* wm, byte* data);
void calculate_extended_ir(struct wiimote_t* wm, byte* data);
float calc_yaw(struct ir_t* ir);

#ifdef __cplusplus
}
#endif

#endif // IR_H_INCLUDED




#endif // USE_WIIC
