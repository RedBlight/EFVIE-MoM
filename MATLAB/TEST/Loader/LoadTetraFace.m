function [ faceCount, tetraCount, faceVertexIndex, faceTetraIndex, tetraFaceIndex ] = LoadTetraFace( geomName )
    
    p = load( "..\Misc\pathList.mat" );
	facePath = p.dataFolder + geomName + p.faceExt;
    
    fileId = fopen( facePath );
    
    faceCount = fread( fileId, 1, 'uint64' );
    
    tetraCount = fread( fileId, 1, 'uint64' );
    
    faceVertexIndex = fread( fileId, faceCount * 3, 'uint64' );
    faceVertexIndex = transpose( reshape( faceVertexIndex, [ 3, faceCount ] ) );
    
    faceTetraIndex = fread( fileId, faceCount * 2, 'uint64' );
    faceTetraIndex = transpose( reshape( faceTetraIndex, [ 2, faceCount ] ) );
    
    tetraFaceIndex = fread( fileId, tetraCount * 4, 'uint64' );
    tetraFaceIndex = transpose( reshape( tetraFaceIndex, [ 4, tetraCount ] ) );
    
    fclose( fileId );
	
end

