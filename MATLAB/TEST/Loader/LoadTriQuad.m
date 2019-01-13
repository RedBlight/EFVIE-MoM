function [ faceCount, quadCount, quadData ] = LoadTriQuad( geomName )
	
    p = load( "..\Misc\pathList.mat" );
	triqPath = p.dataFolder + geomName + p.triqExt;
    
    fileId = fopen( triqPath );
    
    faceCount = fread( fileId, 1, 'uint64' );
    
    quadCount = fread( fileId, 1, 'uint64' );
    
    quadData = fread( fileId, faceCount * quadCount * 4, 'double' );
    quadData = transpose( reshape( quadData, [ 4, faceCount * quadCount ] ) );
    
    fclose( fileId );
	
end

