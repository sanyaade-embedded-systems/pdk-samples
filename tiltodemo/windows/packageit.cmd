set STAGING_DIR=STAGING\com.palm.app.tiltodemo
set STRIP=y

rmdir /s /y %STAGING_DIR%
del *.ipk
mkdir %STAGING_DIR%
call buildit.cmd
copy ..\appinfo.json %STAGING_DIR%
copy tiltodemo %STAGING_DIR%
if %STRIP%==y arm-none-linux-gnueabi-strip %STAGING_DIR%\tiltodemo
copy ..\res\*.* %STAGING_DIR%
echo filemode.755=tiltodemo > %STAGING_DIR%\package.properties
palm-package %STAGING_DIR%
