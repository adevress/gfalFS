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
#include <errno.h>




gfalFS_file_handle gfalFS_file_handle_new(void* fh, const char* path){
	gfalFS_file_handle ret = g_new(struct _gfalFS_file_handle, 1) ;
	g_strlcpy(ret->path, path, GFALFS_URL_MAX_LEN);
	ret->fh = fh;
	ret->offset = 0;
	ret->mut = g_mutex_new();
	return ret;
}

gfalFS_dir_handle gfalFS_dir_handle_new(void* fh, const char* dirpath){
	gfalFS_dir_handle ret = g_new0(struct _gfalFS_dir_handle, 1) ;
	g_strlcpy(ret->path, dirpath, GFALFS_URL_MAX_LEN);
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


void gfalFS_dir_handle_delete(gfalFS_dir_handle handle){
	if(handle){
		g_mutex_free (handle->mut);
		free(handle);
	}
}

int gfalFS_dir_handle_readdir(gfalFS_dir_handle handle, off_t offset, void* buf, fuse_fill_dir_t filler){
	char buff[2048];
	char err_buff[1024];
	int ret;
	
	if(offset != handle->offset){ // corrupted seq
		gfalfs_log(NULL, G_LOG_LEVEL_WARNING , "gfalfs_readdir err : Dir descriptor corruption, not in order %ld %ld", (long) offset, (long) handle->offset);
		return -(EFAULT);
	}
	if(handle->dir != NULL){ // try to recover from  previous saved status
		if( filler(buf, handle->dir->d_name, &handle->st, handle->offset+1) ==1){ 
			return 0; // filler buffer full 
		} else{
			handle->offset += 1;
			handle->dir = NULL;
			return 0;
		}
	}
	
	while( (handle->dir = gfal_readdir(handle->fh)) != NULL){
		if(fuse_interrupted())
			return -(ECANCELED);	
			
		g_strlcpy(buff, handle->path, 2048);
		g_strlcat(buff, "/", 2048);
		g_strlcat(buff, handle->dir->d_name, 2048);
		
		if( gfal_lstat(buff, &handle->st) != 0){ // get stats
			ret = -(gfal_posix_code_error());
			gfalfs_log(NULL, G_LOG_LEVEL_WARNING , "gfalfs_readdir err %d for path %s: %s ", (int) -ret, (char*)buff, (char*)gfal_posix_strerror_r(err_buff, 1024));
			gfal_posix_clear_error();
			return ret;
		}		
	
		ret = filler(buf, handle->dir->d_name, &handle->st, handle->offset+1);	
		if(ret == 1) // buffer full
			return 0;
		handle->offset += 1;
		
	}
    if( (ret = -(gfal_posix_code_error()))){
		gfalfs_log(NULL, G_LOG_LEVEL_WARNING , "gfalfs_readdir err %d for path %s: %s ", (int) gfal_posix_code_error(), (char*)buff, (char*)gfal_posix_strerror_r(err_buff, 1024));
		gfal_posix_clear_error();
		return ret;
	}
	return 0;		
		
}

void* gfalFS_dir_handle_get_fd(gfalFS_dir_handle handle){
	return handle->fh;
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
			gfalfs_log(NULL, G_LOG_LEVEL_WARNING , "gfal_lseek err %d for path %s: %s ", (int) gfal_posix_code_error(), (char*) handle->path, (char*) gfal_posix_strerror_r(err_buff, 1024));
			ret = -(gfal_posix_code_error());
			gfal_posix_clear_error();		
		}
	}

	if( ret == 0){
		i = gfal_write(GPOINTER_TO_INT(handle->fh),(void*)buf, size);
		if(i <0 ){
			gfalfs_log(NULL, G_LOG_LEVEL_WARNING , "gfalfs_write err %d for path %s: %s ", (int) gfal_posix_code_error(), (char*) handle->path, (char*) gfal_posix_strerror_r(err_buff, 1024));
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
		gfalfs_log(NULL, G_LOG_LEVEL_MESSAGE , "gfal_lseek random read");
		if(i <0 ){
			gfalfs_log(NULL, G_LOG_LEVEL_WARNING , "gfal_lseek err %d for path %s: %s ", (int) gfal_posix_code_error(), (char*) handle->path, (char*) gfal_posix_strerror_r(err_buff, 1024));
			ret = -(gfal_posix_code_error());
			gfal_posix_clear_error();		
		}
	}

	if( ret == 0){
		i = gfal_read(GPOINTER_TO_INT(handle->fh),(void*)buf, size);
		if(i <0 ){
			gfalfs_log(NULL, G_LOG_LEVEL_WARNING , "gfalfs_read err %d for path %s: %s ", (int) gfal_posix_code_error(), (char*) handle->path, (char*) gfal_posix_strerror_r(err_buff, 1024));
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
