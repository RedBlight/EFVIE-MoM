function [ I2 ] = GenerateI2( waveNum, vertexList, tetraCount, tetraMidPoint, tetraVertexIndex, tetraQuadW, tetraQuadR )
	
	% I3VV( a, b )
	% a is tetra index
	% b is tetra index
		
	I2 = zeros( 3, tetraCount, tetraCount );
	
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
			
			I2_S = I.SingIntVec( centerA, v1B, v2B, v3B, v4B );
			I2_R = M.GreenRegIntVec( waveNum, centerA, qBW, qBX, qBY, qBZ );
			
			I2( :, idTetraA, idTetraB ) = I2_S + I2_R;
			
		end
		
	end
	
end