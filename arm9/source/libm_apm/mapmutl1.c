
/* 
 *  M_APM  -  mapmutl1.c
 *
 *  Copyright (C) 2003 Michael C. Ring
 *
 *  Permission to use, copy, and distribute this software and its
 *  documentation for any purpose with or without fee is hereby granted, 
 *  provided that the above copyright notice appear in all copies and 
 *  that both that copyright notice and this permission notice appear 
 *  in supporting documentation.
 *
 *  Permission to modify the software is granted, but not the right to
 *  distribute the modified code.  Modifications are to be distributed 
 *  as patches to released version.
 *  
 *  This software is provided "as is" without express or implied warranty.
 */

/*
 *      $Id: mapmutl1.c,v 1.3 2003/07/21 21:00:34 mike Exp $
 *
 *      This file contains the utility function 'M_apm_log_error_msg'
 *
 *	This is the only function in this file so a user can supply
 *	their own custom version easily without changing the base library.
 *
 *      $Log: mapmutl1.c,v $
 *      Revision 1.3  2003/07/21 21:00:34  mike
 *      Modify error messages to be in a consistent format.
 *
 *      Revision 1.2  2003/05/05 18:38:27  mike
 *      fix comment
 *
 *      Revision 1.1  2003/05/04 18:19:14  mike
 *      Initial revision
 */

#include "m_apm_lc.h"

extern void debugPrintHalt(char *);

/****************************************************************************/
void	M_apm_log_error_msg(int fatal)
{
  if (fatal)
    debugPrintHalt("Unrecoverable error in mAPM library.");
}
/****************************************************************************/

