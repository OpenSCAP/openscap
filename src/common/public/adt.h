/*
 * Copyright 2012 Red Hat Inc., Durham, North Carolina.
 * All Rights Reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#pragma once
#ifndef OSCAP_ADT_H
#define OSCAP_ADT_H

/**
 * @file
 * Supporting structures for OpenSCAP Abstract Data Types.
 *
 * @addtogroup COMMON
 * @{
 * @addtogroup ADT
 * @{
 * Abstract Data Types
 */

/**
 * Function pointer to an object destructor.
 */
typedef void (*oscap_destruct_func) (void *);

/** @} */
/** @} */

#endif
