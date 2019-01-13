function [ D ] = GetD( r, tetraCount, faceArea, tetraVol, vertexList, tetraVertexIndex, tetraFaceIndex, faceTetraIndex, swgVertexIndex, coefVector )
	
	tetraIdxIn = 0;
	
	tetInCount = 0;
	
	for tetraIdx = 1 : tetraCount
		if( M.PointInTet( r, ...
			vertexList( :, tetraVertexIndex( 1, tetraIdx ) ), ...
			vertexList( :, tetraVertexIndex( 2, tetraIdx ) ), ...
			vertexList( :, tetraVertexIndex( 3, tetraIdx ) ), ...
			vertexList( :, tetraVertexIndex( 4, tetraIdx ) ) ...
		) )
			
			tetraIdxIn = tetraIdx;
			
			tetInCount = tetInCount + 1;
			
		end
	end
	
	fprintf( '%i\n', tetInCount );
	
	faceList = tetraFaceIndex( :, tetraIdxIn );
	faceTetraList = faceTetraIndex( :, faceList );
	
	swgIdx = 1 * int64( faceTetraList( 1, : ) == tetraIdxIn ) + 2 * int64( faceTetraList( 2, : ) == tetraIdxIn );
	swgSign = 1 * double( swgIdx == 1 ) - 1 * double( swgIdx == 2 );
	
	swgFactor = transpose( coefVector( faceList ) ) .* swgSign .* faceArea( faceList ) ./ ( 3 * tetraVol( tetraIdxIn ) );
	
	swgVertexIndexList = diag( swgVertexIndex( swgIdx, faceList ) );
	
	swgVertex = vertexList( :, swgVertexIndexList );
	
	rMat = repmat( r, 1, 4 );
	swgFactorMat = repmat( swgFactor, 3, 1 );
	
	D = sum( swgFactorMat .* ( rMat - swgVertex ), 2 );
	
end

