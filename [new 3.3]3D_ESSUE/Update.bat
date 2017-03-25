

xcopy /y .\Engine\Export\*.*				.\Reference\Headers\
xcopy /y .\Engine\Headers\*.*				.\Reference\Headers\

xcopy /y .\Engine\System\Code\*.h			.\Reference\Headers\
xcopy /y .\Engine\System\Bin\*.lib			.\Reference\Librarys\
xcopy /y .\Engine\System\Bin\*.dll			.\Client\Bin\
xcopy /y .\Engine\System\Bin\*.dll			.\Tool\Bin\

xcopy /y .\Engine\Utility\Code\*.h			.\Reference\Headers\
xcopy /y .\Engine\Utility\Code\*.hpp			.\Reference\Headers\
xcopy /y .\Engine\Utility\Bin\*.lib			.\Reference\Librarys\
xcopy /y .\Engine\Utility\Bin\*.dll			.\Client\Bin\
xcopy /y .\Engine\Utility\Bin\*.dll			.\Tool\Bin\
