function [ faceCount, momMatrix ] = LoadMomMatrix( geomName )
	
    p = load( "..\Misc\pathList.mat" );
	mommPath = p.dataFolder + geomName + p.mommExt;
    
    fileId = fopen( mommPath );
    
    faceCount = fread( fileId, 1, 'uint64' );
    
    momData = fread( fileId, faceCount * faceCount * 2, 'double' );
    momData = reshape( momData, [ 2, faceCount * faceCount ] );
    momData = momData(1,:) + momData(2,:) .* 1i;
    momMatrix = reshape( momData, [ faceCount, faceCount ] );
    
    fclose( fileId );
end

