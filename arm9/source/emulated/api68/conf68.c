/*
 *                           sc68 - Config file
 *         Copyright (C) 2001-2003 Benjamin Gerard <ben@sashipa.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation; either version 2 of the License, or (at your
 *  option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#include "typedefsTGDS.h"
#include <config68.h>

#include <stdio.h>
#include <string.h>

#include "api68/conf68.h"
#include "file68/file_error68.h"
#include "file68/rsc68.h"
#include "file68/string68.h"
#include "file68/debugmsg68.h"
#include "file68/alloc68.h"

typedef union {
  int i;          /**< Used with SC68CONFIG_INT field. */
  const char * s; /**< Used with SC68CONFIG_STRING field.  */
} SC68config_value_t;

typedef struct _SC68config_field_s SC68config_field_t;

typedef struct
{
  const char * name;      /**< name of the entry.         */
  SC68config_type_t type; /**< Type of this config entry. */
  const char * comment;   /**< Comment.                   */
  SC68config_value_t min; /**< Minimum value allowed.     */
  SC68config_value_t max; /**< Maximum value allowed.     */
  SC68config_value_t def; /**< Default value the entry.   */
  SC68config_value_t val; /**< Value for the entry.       */
} SC68config_entry_t;

struct _SC68config_s {
  int saved;    /**< True if config has been saved. */
  int size;     /**< Number of entries allocated.   */
  int n;        /**< Number of entries in used.     */

  /** Config entry table.
   * @warning Must be at the end of the struct.
   */
  SC68config_entry_t entries[1];
};


/* Defines for the config default values. */
#define AMIGA_BLEND      0x5000     /* Amiga default blending factor. */
#define	DEFAULT_TIME	 (3*60)     /* Track default time in second.  */
#define FORCE_TRACK      1          /* 0:no forced track              */
#define SKIP_TIME	 4          /* Skip music time in sec.        */
#define MAX_TIME        (24*60*60)  /* 1 day should be enought.       */
#define DEFAULT_SEEKSPD 0x0F00      /* << 8 */
#define MAX_SEEKSPD     0x1F00

static const SC68config_entry_t conftab[] = {
  {
    "version", SC68CONFIG_INT,
    "major*100+minor",
    {0}, {10000}, {VERSION68_NUM}
  },
  {
    "sampling_rate", SC68CONFIG_INT,
    "sampling rate in Hz",
    {SAMPLING_RATE_MIN},{SAMPLING_RATE_MAX},{SAMPLING_RATE_DEF}
  },
  {
    "amiga_blend", SC68CONFIG_INT,
    "Amiga left/right voices blending factor",
    {0},{65535},{AMIGA_BLEND}
  },
  {
    "amiga_interpol", SC68CONFIG_INT,
    "Amiga interpolation mode (0:none 1:linear)",
    {0},{1},{1}
  },
  {
    "force_track", SC68CONFIG_INT,
    "override default track (0:OFF)",
    {0}, {99}, {FORCE_TRACK}
  },
  {
    "force_loop", SC68CONFIG_INT,
    "override default loop (-1:OFF 0:Infinite)",
    {-1}, {100}, {-1}
  },
  {
    "skip_time", SC68CONFIG_INT,
    "skip track less than given seconds (0:OFF)",
    {0}, {MAX_TIME}, {SKIP_TIME}
  },
  {
    "default_time", SC68CONFIG_INT,
    "default time in second if unknown",
    {0}, {MAX_TIME}, {DEFAULT_TIME}
  },
  {
    "seek_speed", SC68CONFIG_INT,
    "seek speed factor * 256 (0:OFF)",
    {0}, {MAX_SEEKSPD}, {DEFAULT_SEEKSPD}
  },
  {
    "total_time", SC68CONFIG_INT,
    "total playing time since first launch",
    {0}, {0}, {0}
  },
  {
    "music_path", SC68CONFIG_STRING,
    "local sc68 music path",
    {0}, {0}, {0}
  },
  {
    "remote_on", SC68CONFIG_INT,
    "Enable sc68 remote access (using curl)",
    {0}, {1}, {1}
  },
  {
    "remote_music_path", SC68CONFIG_STRING,
    "remote sc68 music path",
    {0}, {0}, {0}
  }
};

static const int nconfig = sizeof(conftab) / sizeof(conftab[0]);

static const char config_header[] =
"# -*- sc68 config file -*-\n"
"#\n"
"# Generated by " PACKAGE68_STRING "\n"
"# (C) 1998-2003 Benjamin Gerard <ben@sashipa.com>\n"
"#\n"
"# " PACKAGE68_URL "\n"
"#\n"
"# You can edit this file. If you remove it, sc68 will generate\n"
"# a new one at start-up with default values, but you will lost your\n"
"# total playing time. To avoid it, you should either save its value\n"
"# or delete all lines you want to be resetted.\n"
"# - *int* : integer values; \"C\" number format (e.g.0xFE0120).\n"
"# - *str* : String values; quoted with (\"); must not contain (\").\n"
"#\n";

static int is_symbol_char(int c)
{
  return
    (c>='0' && c<='9')
    || (c>='a' && c<='z')
    || (c>='A' && c<='Z')
    || c=='_'
    || c=='.';
}

static int digit(int c, unsigned int base)
{
  int n = -1;
  if (c <= '9') {
    n = c - '0';
  } else if (c <= 'Z') {
    n = c - 'A' + 10;
  } else if (c <= 'z'){
    n = c - 'a' + 10;
  }
  if ((unsigned int)n < base) {
    return n;
  }
  return -1;
}

/** $$$ Need this function for dcplaya version. */
static long mystrtoul(const char * s,
		      char * * end,
		      unsigned int base)
{
  const char * start = s;
  unsigned long v = 0;
  int neg = 0, c;

  /* Skip starting spaces. */
  for (c = *s; (c == ' ' || c == '9' || c =='\n'); c = *++s)
    ;

  /* Get optionnal sign. */
  /* $$$ ben : Does not make sens with unsigned value ! */
  if (c == '-' || c == '+') {
    neg = (c == '-');
    c = *++s;
  }

  /* Get the base. */
  if (!base) {
    /* Assume default base is 10 */
    base = 10;

    /* Could be either octal(8) or hexidecimal(16) */
    if (c == '0') {
      base = 8;
      c = *++s;
      if (c == 'x' || c == 'X') {
	base = 16;
	c = *++s;
      }
    }
  } else if (base == 16 && c == '0') {
    /* Hexa mode must skip "0x" sequence */
    c = *++s;
    if (c == 'x' || c == 'X') {
      c = *++s;
    }
  }

  c = digit(c,base);
  if (c < 0) {
    s = start;
  } else {
    do {
      v = v * base + c;
      c = digit(*++s,base);
    } while (c >= 0);
  }

  if (end) {
    *end = (char *)s;
  }

  return neg ? -(signed long)v : v;
}

static int config_set_int(SC68config_t * conf, SC68config_entry_t *e, int v)
{
  int m,M;

  if (e->type != SC68CONFIG_INT) {
    return -1;
  }
  m = e->min.i;
  M = e->max.i;
  if (m != M) {
    if (m==0 && M == 1) {
      v = !!v;
    } else if (v < m) {
      v = m;
    } else if (v > M) {
      v = M;
    }
  }

  if (v != e->val.i) {
    conf->saved = 0;
    e->val.i = v;
  }
  return 0;
}

static int config_set_str(SC68config_t * conf, SC68config_entry_t *e,
			  const char * s)
{
  int err = 0;
  int m,M;

  if (e->type != SC68CONFIG_STRING) {
    return -1;
  }

  m = e->min.i;
  M = e->max.i;
  if (m != M) {
    int v = s ? strlen(s) : 0;
    if (v < m || v > M) {
      s = 0;
      err = -1;
    }
  }

  if (!s) {
    if (e->val.s) {
      SC68free((void*)e->val.s);
      e->val.s = 0;
      conf->saved = 0;
    }
  } else if (!e->val.s || strcmp(s,e->val.s)) {
    SC68free((void*)e->val.s);
    e->val.s = SC68strdup(s);
    err = -!e->val.s;
    conf->saved = 0;
  }

  return err;
}


/* Check config values and correct invalid ones
 */
int SC68config_valid(SC68config_t * conf)
{
  int err = 0;
  int i;

  if (!conf) {
    return -1;
  }

  for (i=0; i < conf->n; i++) {
    SC68config_entry_t *e = conf->entries+i;
    switch (e->type) {
    case SC68CONFIG_INT:
      err |= config_set_int(conf, e, e->val.i);
      break;
    case SC68CONFIG_STRING:
      err |= config_set_str(conf, e, e->val.s);
      break;
    default:
      err = -1;
    }
  }

  return -!!err;
}

int SC68config_get_idx(const SC68config_t * conf, const char * name)
{
  if (!conf) {
    return -1;
  }
  if (name) {
    int i;
    for (i=0; i<conf->n; i++) {
      if (!SC68strcmp(name,conf->entries[i].name)) {
	return i;
      }
    }
  }
  return -1;
}

SC68config_type_t SC68config_range(const SC68config_t * conf, int idx,
                                   int * min, int * max, int * def)
{
  SC68config_type_t type = SC68CONFIG_ERROR;
  int vmin = 0 , vmax = 0, vdef = 0;

  if (conf && idx >= 0 && idx < conf->n) {
    type = conf->entries[idx].type;
    vmin = conf->entries[idx].min.i;
    vmax = conf->entries[idx].max.i;
    vdef = conf->entries[idx].def.i;
  }
  if (min) *min = vmin;
  if (max) *max = vmax;
  if (def) *def = vdef;
  return type;
}

SC68config_type_t SC68config_get(const SC68config_t * conf,
				 int * v,
				 const char ** name)
{
  int idx;
  SC68config_type_t type = SC68CONFIG_ERROR;

  if (conf) {
    idx = v ? *v : -1;
    if (idx == -1 && name) {
      idx = SC68config_get_idx(conf, *name);
    }
    if (idx >= 0 && idx < conf->n) {
      switch (type = conf->entries[idx].type) {
      case SC68CONFIG_INT:
	if (v) *v = conf->entries[idx].val.i;
	break;
    
      case SC68CONFIG_STRING:
	if (name) *name = conf->entries[idx].val.s
		    ? conf->entries[idx].val.s
		    : conf->entries[idx].def.s;
	break;

      default:
	type = SC68CONFIG_ERROR;
	break;
      }
    }
  }
  return type;
}

SC68config_type_t SC68config_set(SC68config_t * conf,
				 int idx,
				 const char * name,
				 int v,
				 const char * s)
{
  SC68config_type_t type = SC68CONFIG_ERROR;
  if (conf) {
    if (name) {
      idx = SC68config_get_idx(conf, name);
    }
    if (idx >= 0 && idx < conf->n) {
      switch (type = conf->entries[idx].type) {
      case SC68CONFIG_INT:
	config_set_int(conf, conf->entries+idx, v);
	break;
	
      case SC68CONFIG_STRING:
	if (!config_set_str(conf, conf->entries+idx, s)) {
	  break;
	}
      default:
	type = SC68CONFIG_ERROR;
	break;
      }
    }
  }
  return type;
}



static int save_config_entry(istream_t * os, const SC68config_entry_t * e)
{
  int err = 0;
  char tmp[64];

  err |= istream_puts(os, "\n# ") < 0;
  err |= istream_puts(os, e->comment) < 0;

  switch (e->type) {
  case SC68CONFIG_INT:
    sprintf(tmp, "; *int* [%d..%d]", e->min.i, e->max.i);
    err |= istream_puts(os, tmp) < 0;
    sprintf(tmp, " (%d)\n", e->def.i);
    err |= istream_puts(os, tmp) < 0;
    break;

  case SC68CONFIG_STRING:
    err |= istream_puts(os, "; *str* (\"") < 0;
    err |= istream_puts(os, e->def.s) < 0;
    err |= istream_puts(os, "\")\n") < 0;
    break;

  default:
    istream_puts(os, e->name);
    istream_puts(os, ": invalid type\n");
    return -1;
  }

  switch (e->type) {
  case SC68CONFIG_INT:
    err |= istream_puts(os, e->name) < 0;
    err |= istream_putc(os, '=') < 0;
    sprintf(tmp, "%d", e->val.i);
    err |= istream_puts(os, tmp) < 0;
    debugmsg68("conf_save:[%s]=%d (int)\n",e->name, e->val.i);
    break;

  case SC68CONFIG_STRING: {
    const char * s = e->val.s ? e->val.s : e->def.s;
    if (!s) {
      err |= istream_putc(os, '#') < 0;
    }
    err |= istream_puts(os, e->name) < 0;
    err |= istream_putc(os, '=') < 0;
    err |= istream_putc(os, '"') < 0;
    err |= istream_puts(os, s) < 0;
    err |= istream_putc(os, '"') < 0;
    debugmsg68("conf_save:[%s]='%s' (str)\n",e->name, s);
  } break;

  default:
    break;
  }
  err |= istream_putc(os, '\n') < 0;
  return err;
}


int SC68config_save(SC68config_t * conf)
{
  int i,err;
/*   char tmp[1024]; */
  istream_t * os; 
  const int sizeof_config_hd = sizeof(config_header)-1; /* Remove '\0' */

  SC68os_pdebug("SC68config_save() : enter\n");

  if (!conf) {
    return -1;
  }

  os = rsc68_open(rsc68_config, "config", 2, 0);
  if (!os) {
    return -1;
  }

  err = 0;
  if (istream_write(os, config_header, sizeof_config_hd) != sizeof_config_hd) {
    err = -1;
  }

  for (i=0; !err && i < conf->n; ++i) {
    err = save_config_entry(os, conf->entries+i);
  }
  istream_close(os);
  istream_destroy(os);

  SC68os_pdebug("SC68config_save() : %s\n", err ? "Failed" : "Success");
  return err;
}


/* Load config from file
 */
int SC68config_load(SC68config_t *conf)
{
  istream_t * is; 
  char s[1024], * word;
  int err;
  SC68config_type_t type;

  SC68os_pdebug("SC68config_load() : enter\n");

  if (SC68config_default(conf)) {
    return -1;
  }

  is = rsc68_open(rsc68_config, "config", 1, 0);
  if (!is) {
    return -1;
  }

  for (err=0;;) {
    char * name;
    int i, len, c = 0, idx;

    err = istream_gets(is, s, sizeof(s));
    if (err == -1) {
      break;
    }

    len = err;
    err = 0;
    if (len == 0) {
      break;
    }

    i = 0;

    /* Skip space */
    while (i < len && (c=s[i++], (c == ' ' || c == 9)))
      ;

    if (!is_symbol_char(c)) {
      continue;
    }

    /* Get symbol name. */
    name = s+i-1;
    while (i < len && is_symbol_char(c = s[i++]))
      ;
    s[i-1] = 0;

    /*debugmsg68("conf_load:get name [%s]\n", name);*/

    /* Skip space */
    while (i < len && (c == ' ' || c == 9)) {
      c=s[i++];
    }

    /* Must have '=' */
    if (c != '=') {
      continue;
    }
    c=s[i++];
    /* Skip space */
    while (i < len && (c == ' ' || c == 9)) {
      c=s[i++];
    }

    if (c == '"') {
      type = SC68CONFIG_STRING;
      word = s + i;
    } else if (c == '-' || digit(c, 10) != -1) {
      type = SC68CONFIG_INT;
      word = s + i - 1;
    } else {
      continue;
    }
    /*debugmsg68("conf_load:get type (%d)\n",type);*/

    idx = SC68config_get_idx(conf, name);
    if (idx < 0) {
      /* Create this config entry */
    }
    if (idx < 0 || conf->entries[idx].type != type) {
      debugmsg68("conf_load:[%s] (%d) missing\n",
		 conf->entries[idx].name, conf->entries[idx].type);
      continue;
    }

    switch (type) {
    case SC68CONFIG_INT:
      config_set_int(conf, conf->entries+idx, mystrtoul(word, 0, 0));
      debugmsg68("conf_load:[%s]=%d\n",
		 conf->entries[idx].name, conf->entries[idx].val.i);
      break;
    case SC68CONFIG_STRING:
      while (i < len && (c=s[i++], c && c != '"'))
	;
      s[i-1] = 0;
      config_set_str(conf, conf->entries+idx, word);
      debugmsg68("conf_load:[%s]='%s'\n",
		 conf->entries[idx].name, conf->entries[idx].val.s);
    default:
      break;
    }
  }

  if (!err) {
    err = SC68config_valid(conf);
  }

  istream_destroy(is);
  SC68os_pdebug("SC68config_load() : %s\n", err ? "Failed" : "Success");

  return err;
}

/* Fill config struct with default values.
 */
int SC68config_default(SC68config_t * conf)
{
  int i;

  if(!conf) {
    return -1;
  }
  for (i=0; i < conf->n; i++) {
    SC68config_entry_t * e = conf->entries+i;
    switch (e->type) {
    case SC68CONFIG_INT:
      e->val.i = e->def.i;
      break;
    case SC68CONFIG_STRING:
      SC68free((void*)e->val.s);
      e->val.s = 0;
    default:
      break;
    }
  }
  conf->saved = 0;
  return SC68config_valid(conf);
}

SC68config_t * SC68config_create(int size)
{
  SC68config_t * c;
  int i,j;

  if (size < nconfig) {
    size = nconfig;
  }
  c = SC68alloc(sizeof(*c) - sizeof(c->entries) + sizeof(*c->entries) * size);
  if (c) {
    c->size = size;
    c->saved = 0;
    for (j=i=0; i<nconfig; ++i) {
      c->entries[j] = conftab[i];
      switch(c->entries[j].type) {
      case SC68CONFIG_INT:
	config_set_int(c, c->entries+j, c->entries[j].def.i);
	debugmsg68("conf_create:[%s]=%d (int)\n",
		   c->entries[j].name, c->entries[j].val.i);
	++j;
	break;

      case SC68CONFIG_STRING:
	c->entries[j].val.s = 0;
	c->entries[j].def.s = 0;
	config_set_str(c, c->entries+j, 0);
	debugmsg68("conf_create:[%s]='%s' (str)\n",
		   c->entries[j].name, c->entries[j].val.s);
	++j;
	break;

      default:
	debugmsg68("conf_create:[%s] (%d) [invalid type]\n",
		   c->entries[j].name, c->entries[j].type);
	break;
      }
    }
    c->n = j;
  }
  return c;
}

void SC68config_destroy(SC68config_t * c)
{
  if (c) {
    int i;

    for (i=0; i<c->n; ++i) {
      if (c->entries[i].type == SC68CONFIG_STRING) {
	SC68free((void*)c->entries[i].val.s);
      }
    }
    SC68free(c);
  }
}

