::created by xgl,2019/1/24
@set temp_path=%path%
@call :add_path %~dp0\tools
@call :add_path %~dp0\tools\python3
::@set path=%path%;%~dp0\tools
@set param1=%1
@if NOT "%param1%"=="DNLM" if not "%param1%" == "CLEAN" (goto error1)
@if %param1% == DNLM (goto make_ml302) 
@if %param1% == CLEAN (goto make_clean) 

:error1
@echo usage:build.bat param
@echo 	DNLM:compile ML302 DNLM version
@echo 	CLEAN:clean the project
@goto exit1
:make_ml302
@if exist %~dp0\firmware\DNLM (goto cr1)
@md %~dp0\firmware\DNLM
:cr1
@echo Making ML302 DNLM Version
@echo HWVER :=DNLM >%~dp0\tools\config.mk
@echo LINK_FILE:=link_option_lv.tmp >>%~dp0\tools\config.mk
@make -j8
@goto exit1
:make_clean
@make  clean
@set path=%temp_path%
:exit1




:add_path
@(echo ";%PATH%;" | find /C /I ";%1;" > nul) || set "PATH=%1;%PATH%"
@goto :eof


