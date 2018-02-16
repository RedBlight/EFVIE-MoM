[ faceCount1, tetraCount1, faceVertexIndex, faceTetraIndex, tetraFaceIndex ] = LoadTetraFace( "spher378r1" );
[ vertexCount, tetraCount2, vertexData, tetraVertexIndex ] = LoadTetraMesh( "spher378r1" );
[ tetraCount3, tetraQuadCount, tetraQuadData ] = LoadTetraQuad( "spher378r1" );
[ faceCount2, faceQuadCount, faceQuadData ] = LoadFaceQuad( "spher378r1" );

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

[ faceCount3, momMatrix ] = LoadMomMatrix( "spher378r1" );
figure();
h = pcolor( abs( momMatrix ) );
set(h, 'EdgeColor', 'none');
axis equal tight square;