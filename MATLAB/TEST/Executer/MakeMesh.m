function MakeMesh( geomName )
	
	p = load( "..\Misc\pathList.mat" );
	
	applArg = p.meshApp;
	unvmArg = p.dataFolder + geomName + p.unvmExt;
	meshArg = p.dataFolder + geomName + p.meshExt;
	
	sysArg = applArg + " " + unvmArg + " " + meshArg;
	
	system( sysArg );
	
end

