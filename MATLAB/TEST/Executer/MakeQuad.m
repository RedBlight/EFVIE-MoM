function MakeQuad( geomName )
	
	p = load( "..\Misc\pathList.mat" );
	
	applArg = p.quadApp;
	meshArg = p.dataFolder + geomName + p.meshExt;
	faceArg = p.dataFolder + geomName + p.faceExt;
	tetqArg = p.dataFolder + geomName + p.tetqExt;
	triqArg = p.dataFolder + geomName + p.triqExt;
	
	sysArg = applArg + " " + meshArg + " " + faceArg + " " + tetqArg ...
	 + " " + triqArg;
	
	system( sysArg );
	
end