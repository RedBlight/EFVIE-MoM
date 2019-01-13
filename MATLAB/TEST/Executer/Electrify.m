function Electrify( geomName, ruleName )
	
	p = load( "..\Misc\pathList.mat" );
	
	applArg = p.elecApp;
	meshArg = p.dataFolder + geomName + p.meshExt;
	ruleArg = p.dataFolder + ruleName + p.ruleExt;
	propArg = p.dataFolder + geomName + p.propExt;
	
	sysArg = applArg + " " + meshArg + " " + ruleArg + " " + propArg;
	
	system( sysArg );
	
end