function [ vertexCount, tetraCount, vertexData, tetraVertexIndex ] = LoadTetraMesh( geomName )

	p = load( "..\Misc\pathList.mat" );
	meshPath = p.dataFolder + geomName + p.meshExt;
    
    fileId = fopen( meshPath );
    
    vertexCount = fread( fileId, 1, 'uint64' );
    
    tetraCount = fread( fileId, 1, 'uint64' );
    
    vertexData = fread( fileId, vertexCount * 3, 'double' );
    vertexData = transpose( reshape( vertexData, [ 3, vertexCount ] ) );
    
    tetraVertexIndex = fread( fileId, tetraCount * 4, 'uint64' );
    tetraVertexIndex = transpose( reshape( tetraVertexIndex, [ 4, tetraCount ] ) );

    fclose( fileId );
	
end

