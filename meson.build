project('ldpl', 'cpp', version : '4.5-dev')

add_project_arguments('-DLPMLOCATION="~/ldpl/lpm/"', language : 'cpp')
add_project_arguments(''.join(['-DVERSION="',meson.project_version(),'"']) , language : 'cpp')
add_project_arguments('-DVERSIONNAME="Friendly Falcarius"', language : 'cpp')
add_project_arguments('-DCOMPILEDATE=1', language : 'cpp')
add_project_arguments('-DCOMPILEHOUR=1', language : 'cpp')

prog = find_program('awk')
r = run_command('src/lib-to-string.sh')

assert(r.returncode()==0, 'lib-to-strin.sh OK')

list_sources = files([
	'src/ldpl.cpp',
	])

list_headers = files([
	'src/ldpl.h',
	'src/cpptrim.h'
])
install_headers(list_headers)

run_man = run_command('man/generateMan.sh')
assert(run_man.returncode()==0, 'Manpage generate OK')
install_man('man/ldpl.1')


executable('ldpl', sources: list_sources )
