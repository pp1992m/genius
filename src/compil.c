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

#include "config.h"

#ifdef GNOME_SUPPORT
#include <gnome.h>
#else
#include <libintl.h>
#define _(x) gettext(x)
#endif

#include <stdio.h>
#include <string.h>
#include <glib.h>
#include "mpwrap.h"
#include "eval.h"
#include "calc.h"
#include "dict.h"
#include "util.h"
#include "matrix.h"
#include "matrixw.h"

#include "compil.h"

extern void (*errorout)(char *);

extern GelETree *free_trees;

/*sort of weird encoding, use 'a'+upper 4 bits and 'a'+lower 4 bits*/
static void
append_string(GString *gs,char *s)
{
	char *p;
	char out[3]="aa";
	for(p=s;*p;p++) {
		out[0]='a'+((*p)&0xF);
		out[1]='a'+((*p)>>4);
		g_string_append(gs,out);
	}
}

/*sort of weird encoding, use 'a'+upper 4 bits and 'a'+lower 4 bits*/
static char *
decode_string(char *s)
{
	int len = strlen(s);
	char *p,*pp,*ps;
	if(len%2 == 1)
		return NULL;
	
	/*the 0 takes care of the termination*/
	p = g_new0(char,(len/2)+1);
	
	for(ps=s,pp=p;*ps;ps+=2,pp++) {
		if(*ps<'a' || *ps >'a'+0xF ||
		   *(ps+1)<'a' || *(ps+1) >'a'+0xF) {
			g_free(p);
			return NULL;
		}
		*pp = (*ps-'a') + ((*(ps+1)-'a')<<4);
	}
	return p;
}

static void
gel_compile_node(GelETree *t,GString *gs)
{
	char *s;
	int i,j;
	GSList *li;
	GelETree *ali;
	g_string_sprintfa(gs,";%d",t->type);
	switch(t->type) {
	case NULL_NODE:
		break;
	case VALUE_NODE:
		s = mpw_getstring(t->val.value,0,FALSE,FALSE,FALSE,10);
		g_string_append_c(gs,';');
		g_string_append(gs,s);
		g_free(s);
		break;
	case MATRIX_NODE:
		g_string_sprintfa(gs,";%dx%d;%d",gel_matrixw_width(t->mat.matrix),
				 gel_matrixw_height(t->mat.matrix),
				 t->mat.quoted);
		for(i=0;i<gel_matrixw_width(t->mat.matrix);i++) {
			for(j=0;j<gel_matrixw_height(t->mat.matrix);j++) {
				GelETree *tt = gel_matrixw_set_index(t->mat.matrix,i,j);
				if(!tt) g_string_append(gs,";0");
				else {
					g_string_append(gs,";N");
					gel_compile_node(tt,gs);
				}
			}
		}
		break;
	case OPERATOR_NODE:
		g_string_sprintfa(gs,";%d;%d",t->op.oper,
				  t->op.nargs);
		for(ali=t->op.args;ali;ali=ali->any.next) {
			gel_compile_node(ali,gs);
		}
		break;
	case IDENTIFIER_NODE:
		g_string_sprintfa(gs,";%s",t->id.id->token);
		break;
	case STRING_NODE:
		if(*t->str.str) {
			g_string_append_c(gs,';');
			append_string(gs,t->str.str);
		} else {
			g_string_append(gs,";E");
		}
		break;
	case FUNCTION_NODE:
		g_assert(t->func.func->type==GEL_USER_FUNC);
		g_assert(t->func.func->id==NULL);
		g_string_sprintfa(gs,";%d",t->func.func->nargs);
		for(li=t->func.func->named_args;li;li=g_slist_next(li)) {
			GelToken *tok = li->data;
			g_string_sprintfa(gs,";%s",tok->token);
		}
		gel_compile_node(t->func.func->data.user,gs);
		break;
	case COMPARISON_NODE:
		g_string_sprintfa(gs,";%d",t->comp.nargs);
		for(li=t->comp.comp;li;li=g_slist_next(li)) {
			int oper = GPOINTER_TO_INT(li->data);
			g_string_sprintfa(gs,";%d",oper);
		}
		for(ali=t->comp.args;ali;ali=ali->any.next) {
			gel_compile_node(ali,gs);
		}
		break;
	default:
		g_assert_not_reached(); break;
	}
}

char *
gel_compile_tree(GelETree *t)
{
	GString *gs;
	char *s;
	
	gs = g_string_new("T");
	
	gel_compile_node(t,gs);
	
	s = gs->str;
	g_string_free(gs,FALSE);
	return s;
}

static GelETree *
gel_decompile_node(void)
{
	GelETree *n;
	char *p;
	int type=-1;
	int nargs=-1;
	int quote;
	int oper;
	int i,j;
	int w,h;
	GelMatrixW *m;
	GelETree *li = NULL;
	GelETree *args;
	GSList *oli;
	GelEFunc *func;
	mpw_t tmp;

	p = strtok(NULL,";");
	if(!p) return NULL;
	sscanf(p,"%d",&type);
	if(type==-1) return NULL;

	switch(type) {
	case NULL_NODE:
		return gel_makenum_null();
	case VALUE_NODE:
		p = strtok(NULL,";");
		if(!p) return NULL;
		mpw_init(tmp);
		mpw_set_str(tmp,p,10);
		return gel_makenum_use(tmp);
	case MATRIX_NODE:
		p = strtok(NULL,";");
		if(!p) return NULL;
		h = w = -1;
		sscanf(p,"%dx%d",&w,&h);
		if(h==-1 || w ==-1)
			return NULL;

		p = strtok(NULL,";");
		if(!p) return NULL;
		quote = -1;
		sscanf(p,"%d",&quote);
		if(quote==-1) return NULL;

		m = gel_matrixw_new();
		gel_matrixw_set_size(m,w,h);
		for(i=0;i<w;i++) {
			for(j=0;j<h;j++) {
				p = strtok(NULL,";");
				if(!p) {
					gel_matrixw_free(m);
					return NULL;
				}
				if(*p=='N') {
					GelETree *tt = gel_decompile_node();
					if(!tt) {
						gel_matrixw_free(m);
						return NULL;
					}
					gel_matrixw_set_index(m,i,j)=tt;
				} else if(*p!='0') {
					gel_matrixw_free(m);
					return NULL;
				}
			}
		}
		GET_NEW_NODE(n);
		n->type = MATRIX_NODE;
		n->mat.matrix = m;
		n->mat.quoted = quote;
		return n;
	case OPERATOR_NODE:
		p = strtok(NULL,";");
		if(!p) return NULL;
		oper = -1;
		sscanf(p,"%d",&oper);
		if(oper==-1) return NULL;

		p = strtok(NULL,";");
		if(!p) return NULL;
		sscanf(p,"%d",&nargs);
		if(nargs==-1) return NULL;
		
		args = li = NULL;
		for(i=0;i<nargs;i++) {
			GelETree *tt = gel_decompile_node();
			if(!tt) {
				while(args) {
					li = args->any.next;
					gel_freetree(args);
					args = li;
				}
				return NULL;
			}
			if(!args) {
				args = li = tt;
			} else {
				li = li->any.next = tt;
			}
			li->any.next = NULL;
		}

		GET_NEW_NODE(n);
		n->type = OPERATOR_NODE;
		n->op.args = args;
		n->op.nargs = nargs;
		n->op.oper = oper;
		return n;
	case IDENTIFIER_NODE:
		p = strtok(NULL,";");
		if(!p) return NULL;
		GET_NEW_NODE(n);
		n->type = IDENTIFIER_NODE;
		n->id.id = d_intern(p);
		return n;
	case STRING_NODE:
		p = strtok(NULL,";");
		if(!p) return NULL;
		
		if(*p=='E')
			p = g_strdup("");
		else {
			p = decode_string(p);
			if(!p) return NULL;
		}
		GET_NEW_NODE(n);
		n->type = STRING_NODE;
		n->str.str = p;
		return n;
	case FUNCTION_NODE:
		p = strtok(NULL,";");
		if(!p) return NULL;
		nargs = -1;
		sscanf(p,"%d",&nargs);
		if(nargs==-1) return NULL;

		oli = NULL;
		for(i=0;i<nargs;i++) {
			p = strtok(NULL,";");
			if(!p) {
				g_slist_free(oli);
				return NULL;
			}
			oli = g_slist_append(oli,d_intern(p));
		}
		
		n = gel_decompile_node();
		if(!n) {
			g_slist_free(oli);
			return NULL;
		}

		func = d_makeufunc(NULL,n,oli,nargs);
		func->context = -1;

		GET_NEW_NODE(n);
		n->type = FUNCTION_NODE;
		n->func.func = func;
		return n;
	case COMPARISON_NODE:
		p = strtok(NULL,";");
		if(!p) return NULL;
		sscanf(p,"%d",&nargs);
		if(nargs==-1) return NULL;
	
		oli = NULL;
		for(i=0;i<nargs-1;i++) {
			p = strtok(NULL,";");
			if(!p) {
				g_slist_free(oli);
				return NULL;
			}
			j = -1;
			sscanf(p,"%d",&j);
			if(j==-1) {
				g_slist_free(oli);
				return NULL;
			}
			oli = g_slist_append(oli,GINT_TO_POINTER(j));
		}

		args = li = NULL;
		for(i=0;i<nargs;i++) {
			GelETree *tt = gel_decompile_node();
			if(!tt) {
				while(args) {
					li = args->any.next;
					gel_freetree(args);
					args = li;
				}
				g_slist_free(oli);
				return NULL;
			}
			if(!args) {
				args = li = tt;
			} else {
				li = li->any.next = tt;
			}
			li->any.next = NULL;
		}

		GET_NEW_NODE(n);
		n->type = COMPARISON_NODE;
		n->comp.args = args;
		n->comp.nargs = nargs;
		n->comp.comp = oli;
		return n;
	default:
		return NULL;
	}
}

GelETree *
gel_decompile_tree(char *s)
{
	GelETree *t;
	char *p;
	
	if(!s) return NULL;
	
	p = strtok(s,";");
	
	if(strcmp(p,"T")!=0) {
		(*errorout)(_("Bad tree record when decompiling"));
		return NULL;
	}
	
	t = gel_decompile_node();
	if(!t) {
		(*errorout)(_("Bad tree record when decompiling"));
		return NULL;
	}

	g_free(s);
	
	return t;
}
