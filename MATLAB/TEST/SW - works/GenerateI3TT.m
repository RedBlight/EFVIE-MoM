function [ I3TT ] = GenerateI3TT( waveNum, tetraCount, vertexList, tetraVertexIndex, tetraMidPoint, tetraQuadW, tetraQuadR )
	
	% I3TT( a, b )
	% a and b are tetrahedron indices
		
	I3TT = zeros( tetraCount, tetraCount );
	
	for idTetraA = 1 : tetraCount
		
		centerA = tetraMidPoint( :, idTetraA );
		
		for idTetraB = 1 : tetraCount
			
			v1B = vertexList( :, tetraVertexIndex( 1, idTetraB ) );
			v2B = vertexList( :, tetraVertexIndex( 2, idTetraB ) );
			v3B = vertexList( :, tetraVertexIndex( 3, idTetraB ) );
			v4B = vertexList( :, tetraVertexIndex( 4, idTetraB ) );
			
			qBW = squeeze( tetraQuadW( 1, idTetraB, : ) );
			qBX = squeeze( tetraQuadR( 1, idTetraB, : ) );
			qBY = squeeze( tetraQuadR( 2, idTetraB, : ) );
			qBZ = squeeze( tetraQuadR( 3, idTetraB, : ) );
			
			I3TT_S = I.SingIntVol( centerA, v1B, v2B, v3B, v4B );
			I3TT_R = M.GreenRegInt( waveNum, centerA, qBW, qBX, qBY, qBZ );
			
			I3TT( idTetraA, idTetraB ) = I3TT_S + I3TT_R;
			
		end
		
	end
	
end