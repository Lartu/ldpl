project('ldpl', 'cpp', version : '4.5-dev')

date_prog = find_program('date')

date = run_command(date_prog,'+%Y-%m-%d')
assert(date.returncode()==0, 'Date OK')

hour = run_command(date_prog,'+%H:%M:%S')
assert(hour.returncode()==0, 'Hour OK')

add_project_arguments('-DLPMLOCATION="~/ldpl/lpm/"', language : 'cpp')
add_project_arguments(''.join(['-DVERSION="',meson.project_version(),'"']) , language : 'cpp')
add_project_arguments('-DVERSIONNAME="Friendly Falcarius"', language : 'cpp')
add_project_arguments('-DCOMPILEDATE="' + date.stdout().strip('\n') + '"', language : 'cpp')
add_project_arguments('-DCOMPILEHOUR="' + hour.stdout().strip('\n') + '"', language : 'cpp')

prog = find_program('awk')
r = run_command('src/lib-to-string.sh')

assert(r.returncode()==0, 'lib-to-strin.sh OK')

run_man = run_command('man/generateMan.sh')
assert(run_man.returncode()==0, 'Manpage generate OK')
install_man('man/ldpl.1')


executable('ldpl', sources: 'src/ldpl.cpp' )
