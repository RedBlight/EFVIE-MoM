function SaveTetraMesh( geomName, vertexCount, tetraCount, vertexData, tetraVertexIndex )

	p = load( "..\Misc\pathList.mat" );
	meshPath = p.dataFolder + geomName + p.meshExt;
    
    fileId = fopen( meshPath, 'w' );
    
    fwrite( fileId, vertexCount, 'uint64' );
    
    fwrite( fileId, tetraCount, 'uint64' );
	
    fwrite( fileId, vertexData, 'double' );
	
    fwrite( fileId, tetraVertexIndex-1, 'uint64' );

    fclose( fileId );
	
end

