function [ faceCount, coefVector ] = LoadCoef( fileName )

    fileID = fopen( "Data/" + fileName + ".coef" );
    
    faceCount = fread( fileID, 1, 'uint64' );
    
    coefVector = fread( fileID, faceCount * 2, 'double' );
    coefVector = reshape( coefVector, [ 2, faceCount ] );
    coefVector = coefVector(1,:) + coefVector(2,:) .* 1i;
    coefVector = reshape( coefVector, [ faceCount, 1 ] );
    
    fclose( fileID );
end

