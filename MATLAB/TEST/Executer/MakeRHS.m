function MakeRHS( geomName, waveName )
	
	p = load( "..\Misc\pathList.mat" );
	
	applArg = p.rhsvApp;
	meshArg = p.dataFolder + geomName + p.meshExt;
	faceArg = p.dataFolder + geomName + p.faceExt;
	tetqArg = p.dataFolder + geomName + p.tetqExt;
	waveArg = p.dataFolder + waveName + p.waveExt;
	rhsvArg = p.dataFolder + geomName + p.rhsvExt;
	
	sysArg = applArg + " " + meshArg + " " + faceArg + " " + tetqArg ...
	 + " " + waveArg + " " + rhsvArg;
	
	system( sysArg );
	
end