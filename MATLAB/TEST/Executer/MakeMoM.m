function MakeMoM( geomName, waveName )
	
	p = load( "..\Misc\pathList.mat" );
	
	applArg = p.mommApp;
	meshArg = p.dataFolder + geomName + p.meshExt;
	faceArg = p.dataFolder + geomName + p.faceExt;
	tetqArg = p.dataFolder + geomName + p.tetqExt;
	triqArg = p.dataFolder + geomName + p.triqExt;
	propArg = p.dataFolder + geomName + p.propExt;
	waveArg = p.dataFolder + waveName + p.waveExt;
	mommArg = p.dataFolder + geomName + p.mommExt;
	
	sysArg = applArg + " " + meshArg + " " + faceArg + " " + tetqArg ...
	 + " " + triqArg + " " + propArg + " " + waveArg + " " + mommArg;
	
	system( sysArg );
	
end