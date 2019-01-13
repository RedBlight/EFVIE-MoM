function ObserveRCS( geomName, waveName, obspName )
	
	p = load( "..\Misc\pathList.mat" );
	
	applArg = p.orcsApp;
	meshArg = p.dataFolder + geomName + p.meshExt;
	faceArg = p.dataFolder + geomName + p.faceExt;
	tetqArg = p.dataFolder + geomName + p.tetqExt;
	propArg = p.dataFolder + geomName + p.propExt;
	waveArg = p.dataFolder + waveName + p.waveExt;
	coefArg = p.dataFolder + geomName + p.coefExt;
	obspArg = p.dataFolder + obspName + p.obspExt;
	rcsrArg = p.dataFolder + geomName + p.rcsrExt;
	
	sysArg = applArg + " " + meshArg + " " + faceArg + " " + tetqArg ...
	 + " " + propArg + " " + waveArg + " " + coefArg + " " + obspArg ...
	 + " " + rcsrArg;
	
	system( sysArg );
	
end