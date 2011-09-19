#pragma once
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
 * @file gfal_opers.h
 * @brief header for the mapping of the gfal operators
 * @author Devresse Adrien
 * @version 0.1
 * @date 2/07/2011
 * */
 
#define FUSE_USE_VERSION 28

#include <glib.h>
#include <fuse.h>

void gfalfs_set_local_mount_point(const char* local_mp);

void gfalfs_set_remote_mount_point(const char* remote_mp);

void configure_verbose();

extern gboolean guid_mode;
extern struct fuse_operations gfal_oper;

