function SetEmrule( epsRel )

    p = load( "..\Misc\pathList.mat" );
	emrulePath = p.dataFolder + "constant.emrule";
	
    fileId = fopen( emrulePath, 'w' );
    
    fprintf( fileId, 'Constant\n%12.12f\n%12.12f', epsRel, epsRel );
    
    fclose( fileId );
	
end

