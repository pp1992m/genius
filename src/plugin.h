/* GENIUS Calculator
 * Copyright (C) 1997-2002 George Lebl
 *
 * Author: George Lebl
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the  Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 * USA.
 */

#ifndef PLUGIN_H__
#define PLUGIN_H__

typedef struct _GelPlugin {
	char *base;
	char *file;
	char *name;
	char *author;
	char *copyright;
	char *description;
	gboolean gui;

	/* flags while running */
	gboolean running;
	gboolean restore;
	char *unique_id;
} GelPlugin;

/*read or reread the plugin list from the share/genius/plugins directory*/
void gel_read_plugin_list(void);
extern GSList *gel_plugin_list;

void gel_open_plugin (GelPlugin *plug);
void gel_save_plugins (void);
void gel_restore_plugins (void);

#endif /* PLUGIN_H__ */
