##
# @author Alexander Breuer (alex.breuer AT uni-jena.de)
#
# @section DESCRIPTION
# Entry-point for builds.
##
import SCons
import os
import SCons.Script

print( '####################################' )
print( '### Tsunami Lab                  ###' )
print( '###                              ###' )
print( '### https://scalable.uni-jena.de ###' )
print( '####################################' )
print()
print('runnning build script')


# configuration
vars = Variables()

vars.AddVariables(
    EnumVariable('mode',
                 'compile modes, option \'san\' enables address and undefined behavior sanitizers',
                 'release',
                 allowed_values=('release', 'debug', 'release+san', 'debug+san')
                 ),
    BoolVariable('use_report',
                 'Enable compiler optimization report',
                 False  # Set the default value to False; adjust as needed
                 ),
    PathVariable('custom_cxx',
                 'Path to a custom C++ compiler',
                 os.environ.get('CXX', ''),
                 PathVariable.PathAccept
                 )
)

# exit in the case of unknown variables
if vars.UnknownVariables():
  print( "build configuration corrupted, don't know what to do with: " + str(vars.UnknownVariables().keys()) )
  exit(1)

# create environment
env = Environment( variables = vars )

if env['custom_cxx']:
    env.Replace(CXX=env['custom_cxx'])
    print("Selected Compiler: Custom Compiler")

compiler_path = env['CXX']     
compiler_name = os.path.basename(compiler_path)

if env['use_report']:
  if not (compiler_name == 'icpc'):
    print("Warning: Cannot generate report because you are running the code on the GNU Compiler.")
  else:
    env.Append(CXXFLAGS=['-qopt-report=5'])
    print("the report is in the build folder")  




# generate help message
Help( vars.GenerateHelpText( env ) )

# add default flags
if (compiler_name == 'icpc'):
   env.Append( CXXFLAGS = [ '-std=c++17',
                         '-Wall',
                         '-Wextra',
                         '-Werror',
                         '-qopenmp' ] )
else:    
  env.Append( CXXFLAGS = [ '-std=c++17',
                          '-Wall',
                          '-Wextra',
                          '-Wpedantic',
                          '-Werror',
                          '-fopenmp' ] )
env.Append( LINKFLAGS = [ '-fopenmp' ] )

# set optimization mode
if 'debug' in env['mode']:
  env.Append( CXXFLAGS = [ '-g',
                           '-O0' ] )
else:
  env.Append( CXXFLAGS = [ '-O3' ] )

# add sanitizers
if 'san' in  env['mode']:
  env.Append( CXXFLAGS =  [ '-g',
                            '-fsanitize=float-divide-by-zero',
                            '-fsanitize=bounds',
                            '-fsanitize=address',
                            '-fsanitize=undefined',
                            '-fno-omit-frame-pointer' ] )
  env.Append( LINKFLAGS = [ '-g',
                            '-fsanitize=address',
                            '-fsanitize=undefined','-fopenmp'
                            ] )
                            
                             

 



env.Append(LIBS=['netcdf'])

env.Append(LIBS=['z'])

env.Append(LIBS=['hdf5_serial'])

if 'LD_LIBRARY_PATH' not in env['ENV']:
    env['ENV']['LD_LIBRARY_PATH'] = ''

env['ENV']['LD_LIBRARY_PATH'] = '/usr/lib/x86_64-linux-gnu' + env['ENV']['LD_LIBRARY_PATH']
# add Catch2
env.Append(CXXFLAGS = [ '-isystem', 'submodules/Catch2/single_include'])

# add nlohmann json 
env.Append(CXXFLAGS = ['-isystem', 'submodules/json/single_include'])


env.Append(LIBPATH=['/home/imissoldgaren/tools/netcdf/include'])

# get source files
VariantDir( variant_dir = 'build/src',
            src_dir     = 'src' )

env.sources = []
env.tests = []

Export('env')
SConscript( 'build/src/SConscript' )
Import('env')

env.Program( target = 'build/tsunami_lab',
             source = env.sources + env.standalone )

env.Program( target = 'build/tests',
             source = env.sources + env.tests )