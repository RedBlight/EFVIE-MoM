function [ I2 ] = GenerateI2( waveNum, vertexList, tetraCount, tetraVol, tetraMidPoint, tetraVertexIndex )
	
	% I3VV( a, b )
	% a is tetra index
	% b is tetra index
		
	I2 = zeros( 3, tetraCount, tetraCount );
	
	mjk = -1i * waveNum;
	
	idTetraVec = 1 : tetraCount;
	
	parfor idTetraA = 1 : tetraCount
		
% 		tetraVolA = tetraVol( idTetraA );
% 		centerA = tetraMidPoint( :, idTetraA );
% 		matCenterA = repmat( centerA, 1, tetraCount );
% 		
% 		vecR = tetraMidPoint - matCenterA;
% 		R = sqrt( vecR(1,:).^2 + vecR(2,:).^2 + vecR(3,:).^2 )
% 		I2_M = tetraVol .* exp( mjk .* R ) ) .*  ./ R;
% 		
% 		for idTetraB = idTetraVec( relTT( idTetraA, : ) )
% 			
% 			centerB = tetraMidPoint( :, idTetraB );
% 			tetraVolB = tetraVol( idTetraB );
% 			
% 			v1B = vertexList( :, tetraVertexIndex( 1, idTetraB ) );
% 			v2B = vertexList( :, tetraVertexIndex( 2, idTetraB ) );
% 			v3B = vertexList( :, tetraVertexIndex( 3, idTetraB ) );
% 			v4B = vertexList( :, tetraVertexIndex( 4, idTetraB ) );
% 			
% 			I3TT_M( idTetraB ) = I.SingIntVol( centerA, v1B, v2B, v3B, v4B ) + M.GreenRegInt( waveNum, tetraVolB, centerA, centerB );
% 			
% 		end
% 		
% 		I3TT( idTetraA, : ) = I3TT_M;
		
		
		
		
		
		
		
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