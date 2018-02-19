
modelName = "sphere1r2p24t";

ruleN = "eps2";
waveN = "PlaneZinYpol";
obspN = "xycircular181";

ObserveRCS( modelName, ruleN, waveN, obspN );

[ faceCount1, tetraCount1, faceVertexIndex, faceTetraIndex, tetraFaceIndex ] = LoadTetraFace( modelName );
[ vertexCount, tetraCount2, vertexData, tetraVertexIndex ] = LoadTetraMesh( modelName );
[ tetraCount3, tetraQuadCount, tetraQuadData ] = LoadTetraQuad( modelName );
[ faceCount2, faceQuadCount, faceQuadData ] = LoadFaceQuad( modelName );

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

[ faceCount3, momMatrix ] = LoadMomMatrix( modelName );
fig = figure();
h = pcolor( abs( momMatrix ) );
set(h, 'EdgeColor', 'none');
axis equal tight square;
ax = fig.CurrentAxes;
ax.YDir = 'reverse';
ax.XAxisLocation = 'top';

% [ faceCount4, rhsVector ] = LoadRhs( modelName );
% figure();
% plot( abs( rhsVector ) );
% 
% [ faceCount5, coefVector ] = LoadCoef( modelName );
% figure();
% plot( abs( coefVector ) );

% figure();
% plot( abs( momMatrix * coefVector ) );