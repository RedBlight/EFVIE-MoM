function [ I2 ] = GenerateI2( waveNum, vertexList, tetraCount, tetraVol, tetraMidPoint, tetraVertexIndex )
	
	% I3VV( a, b )
	% a is tetra index
	% b is tetra index
		
	I2 = zeros( 3, tetraCount, tetraCount );
	
	parfor idTetraA = 1 : tetraCount
		
		centerA = tetraMidPoint( :, idTetraA );
		
		for idTetraB = 1 : tetraCount
			
			centerB = tetraMidPoint( :, idTetraB );
			tetraVolB = tetraVol( idTetraB );
			
			v1B = vertexList( :, tetraVertexIndex( 1, idTetraB ) );
			v2B = vertexList( :, tetraVertexIndex( 2, idTetraB ) );
			v3B = vertexList( :, tetraVertexIndex( 3, idTetraB ) );
			v4B = vertexList( :, tetraVertexIndex( 4, idTetraB ) );
			
			I2_S = I.SingIntVec( centerA, v1B, v2B, v3B, v4B );
			I2_R = M.GreenRegIntVec( waveNum, tetraVolB, centerA, centerB );
			
			I2( :, idTetraA, idTetraB ) = I2_S + I2_R;
			
		end
		
	end
	
end