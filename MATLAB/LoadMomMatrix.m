function [ faceCount, momMatrix ] = LoadMomMatrix( fileName )
    
    fileID = fopen( "Data/" + fileName + ".mommatrix" );
    
    faceCount = fread( fileID, 1, 'uint64' );
    
    momData = fread( fileID, faceCount * faceCount * 2, 'double' );
    momData = reshape( momData, [ 2, faceCount * faceCount ] );
    momData = momData(1,:) + momData(2,:) .* 1i;
    momMatrix = reshape( momData, [ faceCount, faceCount ] );
    
    fclose( fileID );
end

