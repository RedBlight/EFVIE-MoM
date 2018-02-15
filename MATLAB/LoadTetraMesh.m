function [ vertexCount, tetraCount, vertexData, tetraVertexIndex ] = LoadTetraMesh( fileName )
    
    fileID = fopen( "Data/" + fileName + ".tetramesh" );
    
    vertexCount = fread( fileID, 1, 'uint64' );
    
    tetraCount = fread( fileID, 1, 'uint64' );
    
    vertexData = fread( fileID, vertexCount * 3, 'double' );
    vertexData = transpose( reshape( vertexData, [ 3, vertexCount ] ) );
    
    tetraVertexIndex = fread( fileID, tetraCount * 4, 'uint64' );
    tetraVertexIndex = transpose( reshape( tetraVertexIndex, [ 4, tetraCount ] ) );

    fclose( fileID );
end

