function [ tetraQuadCount, tetraQuadW, tetraQuadR ] = GenerateTetraQuad( tetraCount, vertexList, tetraVertexIndex, qLevel )
	
	% tetraQuadW( tetraId, quadId )
	% tetraQuadR( xyz, tetraId, quadId )
	
	tetraQuadCount = qLevel^3;
	tetraQuadW = zeros( 1, tetraCount, tetraQuadCount );
	tetraQuadR = zeros( 3, tetraCount, tetraQuadCount );
	
	for idTet = 1 : tetraCount
		vIndex = tetraVertexIndex( :, idTet );
		v1 = vertexList( :, vIndex( 1 ) );
		v2 = vertexList( :, vIndex( 2 ) );
		v3 = vertexList( :, vIndex( 3 ) );
		v4 = vertexList( :, vIndex( 4 ) );
		[ ...
			tetraQuadR( 1, idTet, : ), ...
			tetraQuadR( 2, idTet, : ), ...
			tetraQuadR( 3, idTet, : ), ...
			tetraQuadW( 1, idTet, : ) ...
		] = tetraquad( qLevel, [ v1'; v2'; v3'; v4' ] );
	end
	
end