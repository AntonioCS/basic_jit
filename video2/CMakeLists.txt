project(video2 DESCRIPTION "https://www.youtube.com/watch?v=8oIPHHZXsZ0&list=PLvdK1vRmp8wMzH4w_8sQ30NKU3Bt4Cc-M&index=2")

file(GLOB_RECURSE VIDEO2_SOURCES
	CONFIGURE_DEPENDS
	src/*.c* src/*.h*
)

add_executable(${PROJECT_NAME} ${VIDEO2_SOURCES})

target_compile_options(${PROJECT_NAME}
 PRIVATE
	$<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
		-Werror -Wall -Wextra>
	$<$<CXX_COMPILER_ID:MSVC>:
		/W4>
)

