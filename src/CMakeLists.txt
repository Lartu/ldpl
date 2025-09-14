cmake_minimum_required(VERSION 3.13)  # CMake version check
project(ldpl)               # Create project "simple_example"
set(CMAKE_CXX_STANDARD 11)            # Enable c++11 standard

# Add main.cpp file of project root directory as source file
set(SOURCE_FILES ldpl.cpp)

set(VERSION "5.2.1")
set(VERSIONNAME "Groovy Gualicho")

# Alternative version name (cmake -Dversion="<version>")
if (version)
	set(VERSION ${version})
endif ()

if (CMAKE_SIZEOF_VOID_P EQUAL 4)
	string(CONCAT VERSION ${VERSION} " (i386)")
	add_definitions(-m32)
endif ()

# add VERSION
set(VERSION " -DVERSION='\"${VERSION}\"' ")
add_definitions(${VERSION})

# add VERSIONNAME
set(VERSIONNAME " -DVERSIONNAME='\"${VERSIONNAME}\"' ")
add_definitions(${VERSIONNAME})

add_definitions(-Wall -fpermissive)

# Add DATE
string(TIMESTAMP DATE "%Y-%m-%d")
set(DATE " -DCOMPILEDATE='\"${DATE}\"' ")
add_definitions(${DATE})

# Add HOUR
string(TIMESTAMP HOUR "+%H:%M:%S")
set(HOUR " -DCOMPILEHOUR='\"${HOUR}\"' ")
add_definitions(${HOUR})

# add LPMLOCATION
set(LPMLOCATION " -DLPMLOCATION='\"~/ldpl/lpm/\"' ")
add_definitions(${LPMLOCATION})

# add LDPLLIBLOCATION
string(CONCAT LDPLLIBLOCATION ${CMAKE_INSTALL_PREFIX} "/lib/ldpl")
set(LDPLLIBLOCATION " -DLDPLLIBLOCATION='\"${LDPLLIBLOCATION}\"' ")
add_definitions(${LDPLLIBLOCATION})

# Add executable target with source files listed in SOURCE_FILES variable
add_executable(ldpl ${SOURCE_FILES})

if (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin|Android")
	target_link_libraries(ldpl -static-libgcc -static-libstdc++)
endif ()

install(TARGETS ldpl)
install(FILES ldpl_lib/ldpl_lib.cpp TYPE LIB)

