function [ I3TT ] = GenerateI3TT( waveNum, tetraCount, vertexList, tetraVertexIndex, tetraMidPoint, tetraVol )
	
	% I3TT( a, b )
	% a and b are tetrahedron indices
		
	I3TT = zeros( tetraCount, tetraCount );
	
	parfor idTetraA = 1 : tetraCount
		
		centerA = tetraMidPoint( :, idTetraA );
		
		for idTetraB = 1 : tetraCount
			
			centerB = tetraMidPoint( :, idTetraB );
			tetraVolB = tetraVol( idTetraB );
			
			v1B = vertexList( :, tetraVertexIndex( 1, idTetraB ) );
			v2B = vertexList( :, tetraVertexIndex( 2, idTetraB ) );
			v3B = vertexList( :, tetraVertexIndex( 3, idTetraB ) );
			v4B = vertexList( :, tetraVertexIndex( 4, idTetraB ) );
			
			I3TT_S = I.SingIntVol( centerA, v1B, v2B, v3B, v4B );
			I3TT_R = M.GreenRegInt( waveNum, tetraVolB, centerA, centerB );
			
			I3TT( idTetraA, idTetraB ) = I3TT_S + I3TT_R;
			
		end
		
	end
	
end