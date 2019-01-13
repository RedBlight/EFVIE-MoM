function [ tetraCount, quadCount, quadData ] = LoadTetraQuad( geomName )
	
    p = load( "..\Misc\pathList.mat" );
	tetqPath = p.dataFolder + geomName + p.tetqExt;
    
    fileId = fopen( tetqPath );
    
    tetraCount = fread( fileId, 1, 'uint64' );
    
    quadCount = fread( fileId, 1, 'uint64' );
    
    quadData = fread( fileId, tetraCount * quadCount * 4, 'double' );
    quadData = transpose( reshape( quadData, [ 4, tetraCount * quadCount ] ) );
    
    fclose( fileId );
	
end

