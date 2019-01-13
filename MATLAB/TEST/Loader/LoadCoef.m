function [ faceCount, coefVector ] = LoadCoef( geomName )

    p = load( "..\Misc\pathList.mat" );
	coefPath = p.dataFolder + geomName + p.coefExt;
	
    fileId = fopen( coefPath );
    
    faceCount = fread( fileId, 1, 'uint64' );
    
    coefVector = fread( fileId, faceCount * 2, 'double' );
    coefVector = reshape( coefVector, [ 2, faceCount ] );
    coefVector = coefVector(1,:) + coefVector(2,:) .* 1i;
    coefVector = reshape( coefVector, [ faceCount, 1 ] );
    
    fclose( fileId );
	
end

