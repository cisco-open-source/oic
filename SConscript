#******************************************************************
#
# Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
#
#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

##
# Examples build script
##
Import('env')
# Add third party libraries
lib_env = env.Clone()
SConscript(env.get('SRC_DIR') + '/resource/third_party_libs.scons', 'lib_env')

stb_env = lib_env.Clone()

secured = stb_env.get('SECURED')

######################################################################
# Build flags
######################################################################
stb_env.AppendUnique(CPPPATH = [
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

stb_env.PrependUnique(LIBS = ['oc', 'octbstack', 'oc_logger', 'connectivity_abstraction', 'coap'])

if secured == '1':
	stb_env.PrependUnique(LIBS = ['ocpmapi'])

target_os = env.get('TARGET_OS')
if target_os not in ['windows', 'winrt']:
	stb_env.AppendUnique(CXXFLAGS = ['-std=c++0x', '-Wall', '-pthread'])

	# Note: 'pthread' is in libc for android. On other platform, if use
	# new gcc(>4.9?) it isn't required, otherwise, it's required
	if target_os != 'android':
		stb_env.AppendUnique(LIBS = ['-lpthread'])

stb_env.AppendUnique(LIBPATH = [env.get('BUILD_DIR')])
stb_env.AppendUnique(RPATH = [env.get('BUILD_DIR')])
stb_env.PrependUnique(LIBS = ['oc', 'octbstack', 'oc_logger', 'connectivity_abstraction', 'coap'])
if env.get('SECURED') == '1':
    stb_env.AppendUnique(LIBS = ['tinydtls'])

if target_os == 'android':
	stb_env.AppendUnique(CXXFLAGS = ['-frtti', '-fexceptions'])
	stb_env.AppendUnique(LIBS = ['gnustl_shared'])

	if not env.get('RELEASE'):
		stb_env.AppendUnique(LIBS = ['log'])

if target_os in ['darwin', 'ios']:
	stb_env.AppendUnique(CPPDEFINES = ['_DARWIN_C_SOURCE'])

######################################################################
# Source files and Targets
######################################################################
stbserver = stb_env.Program('stbserver', ['stbserver.cpp', 'utils.cpp', 'observer.cpp'])
oicfridgeserver = stb_env.Program('oicfridgeserver', ['oicfridgeserver.cpp', 'utils.cpp', 'observer.cpp'])
oicacserver = stb_env.Program('oicacserver', ['oicacserver.cpp', 'utils.cpp', 'observer.cpp'])
input_key = stb_env.Program('input_key', ['input_key.c'])

Alias("stb", [stbserver, oicfridgeserver, oicacserver, input_key])
env.AppendTarget('stb')

src_dir = stb_env.get('SRC_DIR')
svr_db_src_dir = src_dir + '/resource/oic/'
svr_db_build_dir = env.get('BUILD_DIR') +'/resource/oic/'
stb_env.Alias("install", stb_env.Install( svr_db_build_dir,
                svr_db_src_dir + 'oic_svr_db_client.json'))
stb_env.Alias("install", stb_env.Install( svr_db_build_dir,
                svr_db_src_dir + 'oic_svr_db_server.json'))
stb_env.Alias("install", stb_env.Install( svr_db_build_dir,
                svr_db_src_dir + 'oic_stb_server.json'))
stb_env.Alias("install", stb_env.Install( svr_db_build_dir,
                svr_db_src_dir + 'oic_fridge_server.json'))
stb_env.Alias("install", stb_env.Install( svr_db_build_dir,
                svr_db_src_dir + 'oic_ac_server.json'))