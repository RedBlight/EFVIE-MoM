[ faceCount1, tetraCount1, faceVertexIndex, faceTetraIndex, tetraFaceIndex ] = LoadTetraFace( "cube100" );
[ vertexCount, tetraCount2, vertexData, tetraVertexIndex ] = LoadTetraMesh( "cube100" );
[ tetraCount3, tetraQuadCount, tetraQuadData ] = LoadTetraQuad( "cube100" );
[ faceCount2, faceQuadCount, faceQuadData ] = LoadFaceQuad( "cube100" );

surfSelector = repelem( faceTetraIndex(:,2) == 2^64 - 1, faceQuadCount );


sum( faceQuadData( surfSelector, 1 ) ) % must be 6

sum( faceQuadData(surfSelector,1) ...
    .* faceQuadData(surfSelector,2) .* faceQuadData(surfSelector,2) ...
    .* faceQuadData(surfSelector,3) .* faceQuadData(surfSelector,3) ...
    .* faceQuadData(surfSelector,4) .* faceQuadData(surfSelector,4) ...
) % must be 0.0104167

sum( tetraQuadData(:,1) ) % must be 1

sum( tetraQuadData(:,1) ...
    .* tetraQuadData(:,2) .* tetraQuadData(:,2) ...
    .* tetraQuadData(:,3) .* tetraQuadData(:,3) ...
    .* tetraQuadData(:,4) .* tetraQuadData(:,4) ...
) % must be 0.000578704

[ faceCount3, momMatrix ] = LoadMomMatrix( "cube100" );
figure();
h = pcolor( abs( momMatrix ) );
set(h, 'EdgeColor', 'none');
axis equal tight square;