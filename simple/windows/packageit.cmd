set STAGING_DIR=STAGING\com.palm.app.simplepdk

rmdir /s /y %STAGING_DIR%
del *.ipk
mkdir %STAGING_DIR%
call buildit.cmd
copy ..\src\appinfo.json %STAGING_DIR%
copy simple %STAGING_DIR%
echo filemode.755=simple > %STAGING_DIR%\package.properties
palm-package %STAGING_DIR%
