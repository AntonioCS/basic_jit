@echo off
@setlocal EnableExtensions EnableDelayedExpansion	
REM NOTE: Run this in admin mode if it doesn't work

SET rootDir=%~dp0%
SET rootDir=%rootDir:\=/%
SET builDir=%rootDir%build
SET toolChainPath=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
SET vtt=x64-windows

choice /c vc /m "Create new video or just run cmake?"

if !errorlevel! == 1 (
	set /p video_num="Enter video number: "
	IF NOT DEFINED video_num ( goto exit )
	
	set /p video_playlist_url="Enter video playlist url: "
	IF NOT DEFINED video_playlist_url ( goto :exit )
	
	set /a video_as_num=!video_num! -1
	set video_previous_dir_num=!video_as_num!


	if exist "%rootDir%video!video_num!/*" (
		echo This video folder already exists
		goto :exit
	) else (
		REM This works because of EnableExtensions
		mkdir "%rootDir%video!video_num!/src"
		
		if exist "%rootDir%video!video_previous_dir_num!/src/*" (
			echo Copying files from previous video
			echo xcopy /i "%rootDir%video!video_previous_dir_num!/src" "%rootDir%video!video_num!/src"
			xcopy /i "%rootDir%video!video_previous_dir_num!/src" "%rootDir%video!video_num!/src"
		) else (
			echo Generating main.cpp
			(
				echo #include ^<windows.h^>
				echo #include ^<iostream^>
				echo:
				echo int main^(int argc, char** argv^) {
				echo:
				echo    std::cout ^<^< "Hello World!\n";
				echo:
				echo }
			) > "%rootDir%video!video_num!/src/main.cpp"
		)

		echo Generating CMakeLists.txt for video!video_num!
		(
			echo project^(video!video_num! DESCRIPTION "!video_playlist_url!"^)
			echo.
			echo file^(GLOB_RECURSE VIDEO!video_num!_SOURCES
			echo 	CONFIGURE_DEPENDS
			echo 	src/*.c* src/*.h*
			echo ^)
			echo.
			echo add_executable^(${PROJECT_NAME} ${VIDEO!video_num!_SOURCES}^)
			echo.
			echo target_compile_options^(${PROJECT_NAME}
			echo  PRIVATE
			echo 	$^<$^<OR:$^<CXX_COMPILER_ID:Clang^>,$^<CXX_COMPILER_ID:AppleClang^>,$^<CXX_COMPILER_ID:GNU^>^>:
			echo 		-Werror -Wall -Wextra^>
			echo 	$^<$^<CXX_COMPILER_ID:MSVC^>:
			echo 		/W4^>
			echo ^)
			echo.
		) > "%rootDir%video!video_num!/CMakeLists.txt"

		if exist "%rootDir%/CMakeLists.txt" (
			echo add_subdirectory^(video!video_num!^) >> "%rootDir%/CMakeLists.txt"
			echo Added video!video_num! folder to main CMakeLists.txt file
			goto call_cmake
		)
	)
)

:call_cmake
	if not exist "%builDir%" ( 
		echo Creating build dir %builDir%
		mkdir "%builDir%"
	)
	echo Executing cmake command
	cd "%builDir%"
	cmake .. -DCMAKE_TOOLCHAIN_FILE="%toolChainPath%" -DVCPKG_TARGET_TRIPLET="%vtt%"
	cd ..


:exit
