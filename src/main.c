/**
    

    This program can be distributed under the terms of the GNU GPL.
    See the file COPYING.
*/



#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <syslog.h>
#include <glib.h>
#include "gfal_opers.h"

static const char* str_version = _GRIDFS_VERSION;
static gboolean verbose_mode = FALSE;
static gboolean debug_mode = FALSE;

static void gridfs_log_handler(const gchar *log_domain, GLogLevelFlags log_level,
                                 const gchar *message, gpointer user_data){
	if( log_level & ( G_LOG_LEVEL_WARNING | G_LOG_LEVEL_ERROR | G_LOG_LEVEL_CRITICAL)  ){
		syslog(LOG_WARNING, message);
	}else if( (log_level & G_LOG_LEVEL_MESSAGE) && verbose_mode){
		syslog(LOG_INFO, message);	
	}
	if(debug_mode)
		g_printerr("%s\n", message);
									 
}
     
static void path_to_abspath(const char* path, char* abs_buff, size_t s_buff){
	char cdir[2048];
	if(path == NULL || *path=='\0'){
		memset(abs_buff,'\0', s_buff);
		return;
	}
	if(*path!='/'){
		getcwd(cdir, 2048);
		g_strlcpy(abs_buff, cdir, s_buff);
		g_strlcat(abs_buff, "/",s_buff);
		g_strlcat(abs_buff, path,s_buff);		
	}else
		g_strlcpy(abs_buff, path, s_buff);
	
}

static void print_help(char* progname){
	g_printerr("Usage %s [-d] [-s] mount_point grid_url    \n", progname);
	g_printerr("\t [-d] : Debug mode 					   \n");	
	g_printerr("\t [-s] : Single thread mode			   \n");	
	g_printerr("\t [-g] : Guid mode, without grid url		\n");
	g_printerr("\t [-v] : Verbose mode, log all events with syslog, can cause major slowdown \n");
	g_printerr("\t [-V] : Print version number \n");
}

static void print_version(){
	printf("gridfs_version : %s \n", str_version);
}

static void parse_args(int argc, char** argv, int* targc, char** targv){
	int c;
	char abs_path[2048];
	while( (c = getopt(argc, argv, "dshgvV"))  != -1){
		switch(c){
			case 'd':
				debug_mode = TRUE;
				targv[*targc] ="-d";
				*targc+=1;
				break;
			case 's':
				targv[*targc] ="-s";
				*targc+=1;
				break;
			case 'h':
				print_help(argv[0]);
				exit(1);
			case 'g':
				guid_mode = TRUE;
				break;
			case 'v':
				verbose_mode = TRUE;
				configure_verbose();
				printf("verbose mode....\n");
				break;
			case 'V':
				print_version();
				exit(1);
			case '?':
				g_printerr("Unknow option -%c \n", optopt);
				print_help(argv[0]);
				exit(1);
		}		
	}
	int index = optind;
	if(guid_mode){
		if(index +1 != argc){
			g_printerr("Bad number of arguments \n");
			print_help(argv[0]);
			exit(1);
		}
		gfalfs_set_remote_mount_point("");	
		path_to_abspath(argv[index++], abs_path, 2048);
		gfalfs_set_local_mount_point(abs_path);
		targv[(*targc)++] = abs_path;
	}else{
		if(index +2 != argc){
			g_printerr("Bad number of arguments \n");
			print_help(argv[0]);
			exit(1);
		}
		path_to_abspath(argv[index++], abs_path, 2048);
		gfalfs_set_local_mount_point(abs_path);
		targv[(*targc)++] = abs_path;
		gfalfs_set_remote_mount_point(argv[index]);
	}
	return;	
}




int main(int argc, char *argv[])
{   
	g_thread_init(NULL);
	g_log_set_handler (NULL,G_LOG_LEVEL_WARNING | G_LOG_LEVEL_ERROR | G_LOG_LEVEL_CRITICAL |  G_LOG_LEVEL_MESSAGE, gridfs_log_handler, NULL);	// set default log handler
	int targc = 1;
	char* targv[20];
	targv[0] = argv[0]; 
	parse_args(argc, argv, &targc, targv);
	return fuse_main(targc, targv, &gfal_oper,NULL);
}
