function MakeFace( geomName )
	
	p = load( "..\Misc\pathList.mat" );
	
	applArg = p.faceApp;
	meshArg = p.dataFolder + geomName + p.meshExt;
	faceArg = p.dataFolder + geomName + p.faceExt;
	
	sysArg = applArg + " " + meshArg + " " + faceArg;
	
	system( sysArg );
	
end