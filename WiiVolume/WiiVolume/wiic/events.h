#ifdef USE_WIIC

/*
 *    events.h
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
 *	  $Header: /home/wis/Unix/cvsroot/WiiVolume/WiiVolume/wiic/events.h,v 1.2 2014/06/23 08:16:25 wis Exp $
 */

/**
 *	@file
 *	@brief Handles wiimote events.
 *
 *	The file includes functions that handle the events
 *	that are sent from the wiimote to us.
 */

#ifndef EVENTS_H_INCLUDED
#define EVENTS_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

void wiic_pressed_buttons(struct wiimote_t* wm, byte* msg);

void handshake_expansion(struct wiimote_t* wm, byte* data, unsigned short len);
void disable_expansion(struct wiimote_t* wm);

#ifdef __cplusplus
}
#endif


#endif // EVENTS_H_INCLUDED


#endif // USE_WIIC
