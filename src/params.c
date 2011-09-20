/*
 * Copyright (c) Members of the EGEE Collaboration. 2004.
 * See http://www.eu-egee.org/partners/ for details on the copyright holders.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file params.c
 * @brief Parameters management 
 * @author Devresse Adrien
 * @version 0.1
 * @date 13/09/2011
 * */

#include <gfal_api.h>
#include <errno.h>

#include "params.h"

static gboolean verbose_mode = FALSE;
static gboolean debug_mode = FALSE;

/**
 * define verbose mode for gfalFS and GFAL 2.0
 * */
void gfalfs_set_verbose_mode(gboolean status){
	verbose_mode= status;
	if(status)
		gfal_set_verbose(GFAL_VERBOSE_VERBOSE | GFAL_VERBOSE_TRACE);
}

gboolean gfalfs_get_verbose_mode(){
	return verbose_mode;
}


/**
 * define verbose mode for gfalFS and GFAL 2.0
 * */
void gfalfs_set_debug_mode(gboolean status){
	debug_mode= status;
}

gboolean gfalfs_get_debug_mode(){
	return debug_mode;
}
