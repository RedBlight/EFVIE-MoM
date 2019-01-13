clc;
clear all;
close all;

p = load( "..\Misc\pathList.mat" );

holdExt = { p.unvmExt, p.ruleExt, p.waveExt, p.obspExt };

dataList = dir( char( p.dataFolder ) );
nameList = {dataList.name};
isdirList = {dataList.isdir};
for idFile = 1 : numel( nameList )
	fileName = nameList{ idFile };
	[ path_, name_, ext_ ] = fileparts( p.dataFolder + fileName );
	del_ = true;
	for idHold = 1 : numel( holdExt )
		if ext_ == holdExt{ idHold }
			del_ = false;
		end
	end
	if del_ && ~isdirList{ idFile }
		delete( char( p.dataFolder + fileName ) );
	end
end