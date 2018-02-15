[ faceCount, tetraCount2, faceVertexIndex, faceTetraIndex, tetraFaceIndex ] = LoadTetraFace( "cube100" );
[ vertexCount, tetraCount, vertexData, tetraVertexIndex ] = LoadTetraMesh( "cube100" );

figure();
patch('Faces',faceVertexIndex(faceTetraIndex(:,2) ==2^64 - 1,:)+1,'Vertices',vertexData, "FaceColor", "red", "FaceAlpha", .1 );



figure();
patch('Faces',faceVertexIndex+1,'Vertices',vertexData, "FaceColor", "red", "FaceAlpha", .1 );