function [ faceCount, rhsVector ] = LoadRhs( fileName )

    fileID = fopen( "Data/" + fileName + ".rhs" );
    
    faceCount = fread( fileID, 1, 'uint64' );
    
    rhsVector = fread( fileID, faceCount * 2, 'double' );
    rhsVector = reshape( rhsVector, [ 2, faceCount ] );
    rhsVector = rhsVector(1,:) + rhsVector(2,:) .* 1i;
    rhsVector = reshape( rhsVector, [ faceCount, 1 ] );
    
    fclose( fileID );
end

