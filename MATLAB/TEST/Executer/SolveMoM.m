function SolveMoM( geomName )
	
	p = load( "..\Misc\pathList.mat" );
	
	applArg = p.solvApp;
	mommArg = p.dataFolder + geomName + p.mommExt;
	rhsvArg = p.dataFolder + geomName + p.rhsvExt;
	coefArg = p.dataFolder + geomName + p.coefExt;
	
	sysArg = applArg + " " + mommArg + " " + rhsvArg + " " + coefArg;
	
	system( sysArg );
	
end