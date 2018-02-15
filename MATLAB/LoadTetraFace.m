function [ faceCount, tetraCount, faceVertexIndex, faceTetraIndex, tetraFaceIndex ] = LoadTetraFace( fileName )
    
    fileID = fopen( "Data/" + fileName + ".tetraface" );
    
    faceCount = fread( fileID, 1, 'uint64' );
    
    tetraCount = fread( fileID, 1, 'uint64' );
    
    faceVertexIndex = fread( fileID, faceCount * 3, 'uint64' );
    faceVertexIndex = transpose( reshape( faceVertexIndex, [ 3, faceCount ] ) );
    
    faceTetraIndex = fread( fileID, faceCount * 2, 'uint64' );
    faceTetraIndex = transpose( reshape( faceTetraIndex, [ 2, faceCount ] ) );
    
    tetraFaceIndex = fread( fileID, tetraCount * 4, 'uint64' );
    tetraFaceIndex = transpose( reshape( tetraFaceIndex, [ 4, tetraCount ] ) );
    
    fclose( fileID );
end

