
#ifndef _CORTEX_OUT_H_
#define _CORTEX_OUT_H_

/** \file cortex_out.h
 * \brief cortex out format common definition
 * \author Tristan Lelong <tristan.lelong@blunderer.org>
 * \date 2011 06 27
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

int cortex_output_set_format(char *fmt);

void cortex_output_write_process(struct cortex_proc_info *info, FILE * output,
				 int ctx);

#endif /* _CORTEX_OUT_H_ */
