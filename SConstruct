##
#  Build file for gridfs
# @author : Devresse Adrien
# @version 1.0_beta
# @date 29/06/2011

import os

## pre-defined vars for local build only
etics_build_dir= "/home/adevress/workspace"
glib_location = etics_build_dir+ "/repository/externals/glib2-devel/2.12.3/sl5_x86_64_gcc412"

version = "1.0"
package_version= "1.0_preview"

	
## generic function to get conf value
def get_depconf(key_value, include_path='/include/', lib_path='/lib/', lib64_path='/lib64/', etics_suffix="/stage/"):
	if ARGUMENTS.get(key_value,'0') !='0':
		tmp_path = ARGUMENTS.get(key_value,'0')
	else:
		tmp_path= etics_build_dir+ etics_suffix 
	return ([ tmp_path+ include_path],[ tmp_path + lib64_path, tmp_path + lib_path ] )


gfal_headers_dir, gfal_lib_dir = get_depconf('gfal_path', include_path="/include/gfal2", etics_suffix="stage/")

libs = ['fuse', 'glib-2.0','gfal2', 'uuid'];
libs_path= gfal_lib_dir + ["../gfal/build/libs/"]
headers = gfal_headers_dir  + ["../gfal/src/"]
src_all = Glob("src/*.c");
resu = "gridfs";

env = Environment(tools=['default', 'packaging']);

# debug mode
if ARGUMENTS.get('debug','0') =='yes':
	print "DEBUG MODE"
	env.Append(CFLAGS='-g')
	
# prod mode
if ARGUMENTS.get('production','0') =='yes':
	print "prod MODE"
	env.Append(CFLAGS='-O3')


r = os.getenv('LD_LIBRARY_PATH')	# get ld path
env['ENV']['LD_LIBRARY_PATH'] = (r is not None) and r or "" # set ld path or empty one if not exist
env.Append(CFLAGS=['-D_FILE_OFFSET_BITS=64', '-Wall',  "-D_GRIDFS_VERSION=\\\"1.0\\\""], LIBS=libs, CPPPATH=headers, LIBPATH=libs_path)
env.ParseConfig('pkg-config --cflags --libs glib-2.0')
env.ParseConfig('pkg-config --cflags --libs gthread-2.0')


prog = env.Program(resu, src_all);
env.Depends(prog, Glob("src/*.h"))


def define_rpm_install(opt):
	return 'scons -j 8 '+ opt+ ' --install-sandbox="$RPM_BUILD_ROOT" "$RPM_BUILD_ROOT" '

def define_rpm_install_with_file(str1, dest_file, content, mode):
	str1 += " && mkdir -p `dirname " + dest_file + "` "
	str1 +=	' && echo -e   "'+content+'" > '+ dest_file + " "
	str1 +=	" && chmod "+str(mode)+'  '+ dest_file + " "
	return str1

def arguments_to_str():
	ret = ' ';
	for arg, value in ARGUMENTS.iteritems():
		ret += arg+ '=' +value+ ' '
	return ret

def package_main():
	env_gridfs= env.Clone()
	i = env_gridfs.Install("/usr/bin/", prog)
	x_rpm_install = define_rpm_install(arguments_to_str());
	p_main = env_gridfs.Package( 
			 NAME     = 'gridfs',
			 VERSION        = version,
			 PACKAGEVERSION = package_version,
			 PACKAGETYPE    = 'rpm',
			 LICENSE        = 'Apache2.0',
			 SUMMARY        = 'filesystem for the grid',
			 DESCRIPTION    = 'Fuse filesystem for the grid, based on gfal, support the same url type than gfal',
			 X_RPM_GROUP    = 'CERN/grid',
			 X_RPM_INSTALL= x_rpm_install,
			 X_RPM_REQUIRES = 'glib2, fuse, gfal2-core',
			 source= [i] 
			 )
	return p_main
	
if ARGUMENTS.get('package','0') !='0':
	str_func = "package_" + ARGUMENTS.get('package','0')
	print "package : "+ str_func
	p_res = globals()[str_func]()
	Default(p_res)	

