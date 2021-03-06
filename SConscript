##
# Examples build script
##
Import('env')
# Add third party libraries
lib_env = env.Clone()
SConscript(env.get('SRC_DIR') + '/resource/third_party_libs.scons', 'lib_env')

oic_env = lib_env.Clone()

######################################################################
# Build flags
######################################################################
oic_env.PrependUnique(CPPPATH = [
		'include',
		'../include/',
		'../csdk/stack/include',
		'../csdk/security/include',
		'../csdk/ocrandom/include',
		'../csdk/logger/include',
		'../csdk/connectivity/api',
		'../oc_logger/include',
		'../csdk/security/provisioning/include/internal',
		'../csdk/security/provisioning/include/oxm',
		'../csdk/security/provisioning/include/',
		'../../extlibs/tinydtls',
		'../../extlibs/cjson',
		'../../extlibs/base64',
		'../csdk/connectivity/inc',
		'../csdk/connectivity/common/inc',
		'../csdk/connectivity/lib/libcoap-4.1.1',
		'../csdk/connectivity/api'
		])

oic_env.PrependUnique(LIBS = ['oc', 'octbstack', 'oc_logger', 'connectivity_abstraction', 'coap'])

if env.get('SECURED') == '1':
	oic_env.AppendUnique(LIBS = ['ocpmapi', 'tinydtls'])
	oic_env.AppendUnique(CPPDEFINES = ['SECURE'])

target_os = env.get('TARGET_OS')

if target_os not in ['windows', 'winrt']:
	oic_env.AppendUnique(CXXFLAGS = ['-std=c++0x', '-Wall', '-pthread'])

	# Note: 'pthread' is in libc for android. On other platform, if use
	# new gcc(>4.9?) it isn't required, otherwise, it's required
	if target_os != 'android':
		oic_env.AppendUnique(LIBS = ['-lpthread'])

oic_env.AppendUnique(LIBPATH = [env.get('BUILD_DIR')])
oic_env.AppendUnique(RPATH = [env.get('BUILD_DIR')])

if target_os == 'android':
	oic_env.AppendUnique(CXXFLAGS = ['-frtti', '-fexceptions'])
	oic_env.AppendUnique(LIBS = ['gnustl_shared'])

	if not env.get('RELEASE'):
		oic_env.AppendUnique(LIBS = ['log'])

if target_os in ['darwin', 'ios']:
	oic_env.AppendUnique(CPPDEFINES = ['_DARWIN_C_SOURCE'])

######################################################################
# Source files and Targets
######################################################################
oic_src = [
		'src/observer.cpp', 'src/utils.cpp'
	]

oic = oic_env.StaticLibrary('oic', oic_src)
oic_env.InstallTarget(oic, 'liboic')
SConscript('samples/SConscript')

