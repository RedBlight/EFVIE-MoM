function [ I2, I3TT ] = GenerateI3TT( waveNum, tetraCount, vertexList, tetraVertexIndex, tetraMidPoint, tetraVol, relTT )
	
	% I3TT( a, b )
	% a and b are tetrahedron indices
		
	I3TT = zeros( tetraCount, tetraCount );
	I2 = zeros( 3, tetraCount, tetraCount );
	
	mjk = -1i * waveNum;
	
	idTetraVec = 1 : tetraCount;
	
	parfor idTetraA = 1 : tetraCount
		
		centerA = tetraMidPoint( :, idTetraA );
		matCenterA = repmat( centerA, 1, tetraCount );
		
		vecR = tetraMidPoint - matCenterA;
		R = sqrt( vecR(1,:).^2 + vecR(2,:).^2 + vecR(3,:).^2 )
		I3TT_M = tetraVol .* exp( mjk .* R ) ./ R;
		I2_M = vecR .* repmat( I3TT_M, 3, 1 );
		
		for idTetraB = idTetraVec( relTT( idTetraA, : ) )
			
			centerB = tetraMidPoint( :, idTetraB );
			tetraVolB = tetraVol( idTetraB );
			
			v1B = vertexList( :, tetraVertexIndex( 1, idTetraB ) );
			v2B = vertexList( :, tetraVertexIndex( 2, idTetraB ) );
			v3B = vertexList( :, tetraVertexIndex( 3, idTetraB ) );
			v4B = vertexList( :, tetraVertexIndex( 4, idTetraB ) );
			
			I3TT_M( idTetraB ) = I.SingIntVol( centerA, v1B, v2B, v3B, v4B ) + M.GreenRegInt( waveNum, tetraVolB, centerA, centerB );
			I2_M( :, idTetraB ) = I.SingIntVec( centerA, v1B, v2B, v3B, v4B ) + M.GreenRegIntVec( waveNum, tetraVolB, centerA, centerB );
			
		end
		
		I3TT( idTetraA, : ) = I3TT_M;
		I2( :, idTetraA, : ) = permute( I2_M, [ 1, 3, 2 ] );
		
% 		centerA = tetraMidPoint( :, idTetraA );
% 		
% 		for idTetraB = 1 : tetraCount
% 			
% 			centerB = tetraMidPoint( :, idTetraB );
% 			tetraVolB = tetraVol( idTetraB );
% 			
% 			v1B = vertexList( :, tetraVertexIndex( 1, idTetraB ) );
% 			v2B = vertexList( :, tetraVertexIndex( 2, idTetraB ) );
% 			v3B = vertexList( :, tetraVertexIndex( 3, idTetraB ) );
% 			v4B = vertexList( :, tetraVertexIndex( 4, idTetraB ) );
% 			
% 			I3TT_S = I.SingIntVol( centerA, v1B, v2B, v3B, v4B );
% 			I3TT_R = M.GreenRegInt( waveNum, tetraVolB, centerA, centerB );
% 			
% 			I3TT( idTetraA, idTetraB ) = I3TT_S + I3TT_R;
% 			
% 		end
		
	end
	
end