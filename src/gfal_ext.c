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
 * @file gfal_ext.c
 * @brief some extended functionalities for gfalFS
 * @author Devresse Adrien
 * @date 19/09/2011
 * */

#include "gfal_ext.h"
#include <gfal_api.h>




gfalFS_file_handle gfalFS_file_handle_new(void* fh, const char* path){
	gfalFS_file_handle ret = g_new(struct _gfalFS_file_handle, 1) ;
	g_strlcpy(ret->path, path, GFALFS_URL_MAX_LEN);
	ret->fh = fh;
	ret->offset = 0;
	ret->mut = g_mutex_new();
	return ret;
}


void gfalFS_file_handle_delete(gfalFS_file_handle handle){
	if(handle){
		g_mutex_free (handle->mut);
		free(handle);
	}
}

void* gfalFS_file_handle_get_fd(gfalFS_file_handle handle){
	return handle->fh;
}


int gfalFS_file_handle_write(gfalFS_file_handle handle, const char *buf, size_t size, off_t offset){
	g_assert(handle);
	char err_buff[1024];
	int ret = 0;
	int i=-1;
	
	g_mutex_lock(handle->mut);
	if(handle->offset != offset){
		i = gfal_lseek(GPOINTER_TO_INT(handle->fh), offset, SEEK_SET);
		if(i <0 ){
			g_log(NULL, G_LOG_LEVEL_WARNING , "gfal_lseek err %d for path %s: %s ", (int) gfal_posix_code_error(), (char*) handle->path, (char*) gfal_posix_strerror_r(err_buff, 1024));
			ret = -(gfal_posix_code_error());
			gfal_posix_clear_error();		
		}
	}

	if( ret == 0){
		i = gfal_write(GPOINTER_TO_INT(handle->fh),(void*)buf, size);
		if(i <0 ){
			g_log(NULL, G_LOG_LEVEL_WARNING , "gfalfs_write err %d for path %s: %s ", (int) gfal_posix_code_error(), (char*) handle->path, (char*) gfal_posix_strerror_r(err_buff, 1024));
			ret = -(gfal_posix_code_error());
			gfal_posix_clear_error();	
		}else{
			ret = i;
			handle->offset = offset+ i;
		}		
	}

	g_mutex_unlock(handle->mut);
	return ret;
}



int gfalFS_file_handle_read(gfalFS_file_handle handle, const char *buf, size_t size, off_t offset){
	g_assert(handle);
	char err_buff[1024];
	int ret = 0;
	int i=-1;
	
	g_mutex_lock(handle->mut);
	if(handle->offset != offset){
		i = gfal_lseek(GPOINTER_TO_INT(handle->fh), offset, SEEK_SET);
		g_log(NULL, G_LOG_LEVEL_MESSAGE , "gfal_lseek random read");
		if(i <0 ){
			g_log(NULL, G_LOG_LEVEL_WARNING , "gfal_lseek err %d for path %s: %s ", (int) gfal_posix_code_error(), (char*) handle->path, (char*) gfal_posix_strerror_r(err_buff, 1024));
			ret = -(gfal_posix_code_error());
			gfal_posix_clear_error();		
		}
	}

	if( ret == 0){
		i = gfal_read(GPOINTER_TO_INT(handle->fh),(void*)buf, size);
		if(i <0 ){
			g_log(NULL, G_LOG_LEVEL_WARNING , "gfalfs_read err %d for path %s: %s ", (int) gfal_posix_code_error(), (char*) handle->path, (char*) gfal_posix_strerror_r(err_buff, 1024));
			ret = -(gfal_posix_code_error());
			gfal_posix_clear_error();
		}else{
			ret = i;
			handle->offset = offset+ i;
		}		
	}

	g_mutex_unlock(handle->mut);
	return ret;
}
