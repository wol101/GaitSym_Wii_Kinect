#ifdef USE_WIIC

/*
 *    nunchuck.h
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
 *	  $Header: /home/wis/Unix/cvsroot/WiiVolume/WiiVolume/wiic/nunchuk.h,v 1.2 2014/06/23 08:16:25 wis Exp $
 */

/**
 *	@file
 *	@brief Nunchuk expansion device.
 */

#ifndef NUNCHUK_H_INCLUDED
#define NUNCHUK_H_INCLUDED

#include "wiic_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

int nunchuk_handshake(struct wiimote_t* wm, struct nunchuk_t* nc, byte* data, unsigned short len);

void nunchuk_disconnected(struct nunchuk_t* nc);

void nunchuk_event(struct nunchuk_t* nc, byte* msg);

#ifdef __cplusplus
}
#endif

#endif // NUNCHUK_H_INCLUDED


#endif // USE_WIIC
