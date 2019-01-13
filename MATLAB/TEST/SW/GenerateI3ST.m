function [ I3ST ] = GenerateI3ST( lastSingleBaseId, waveNum, tetraCount, faceCount, vertexList, tetraVertexIndex, faceMidPoint, tetraMidPoint, tetraVol, relTS )
	
	% I3ST( m, b )
	% b is tetra index
	% m is face
		
	I3ST = zeros( faceCount, tetraCount );
	
	mjk = -1i * waveNum;
	
	idTetraVec = 1 : tetraCount;
	
	parfor idFaceM = 1 : lastSingleBaseId
		
		%faceAreaM = faceArea( idFaceM );
		centerM = faceMidPoint( :, idFaceM );
		matCenterM = repmat( centerM, 1, tetraCount );
		
		vecR = tetraMidPoint - matCenterM;
		R = sqrt( vecR(1,:).^2 + vecR(2,:).^2 + vecR(3,:).^2 )
		I3ST_M = tetraVol .* exp( mjk .* R ) ./ R;
		
		
		
		for idTetraB = idTetraVec( relTS( :, idFaceM ) )
			
			centerB = tetraMidPoint( :, idTetraB );
			tetraVolB = tetraVol( idTetraB );

			v1B = vertexList( :, tetraVertexIndex( 1, idTetraB ) );
			v2B = vertexList( :, tetraVertexIndex( 2, idTetraB ) );
			v3B = vertexList( :, tetraVertexIndex( 3, idTetraB ) );
			v4B = vertexList( :, tetraVertexIndex( 4, idTetraB ) );

			I3ST_M( idTetraB ) = I.SingIntVol( centerM, v1B, v2B, v3B, v4B ) + M.GreenRegInt( waveNum, tetraVolB, centerM, centerB );
			
		end
		
		
		
		
		
		

% 		v1M = vertexList( :, faceVertexIndex( 1, idFaceM ) );
% 		v2M = vertexList( :, faceVertexIndex( 2, idFaceM ) );
% 		v3M = vertexList( :, faceVertexIndex( 3, idFaceM ) );
% 		I3SS_M( idFaceM ) = I.SingIntSur( centerM, v1M, v2M, v3M ) + mjk * faceAreaM;
% 		
% 		I3SS( idFaceM, : ) = I3SS_M;


		I3ST( idFaceM, : ) = I3ST_M;
		
		
		
% 		centerM = faceMidPoint( :, idFaceM );
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
% 			I3ST_S = I.SingIntVol( centerM, v1B, v2B, v3B, v4B );
% 			I3ST_R = M.GreenRegInt( waveNum, tetraVolB, centerM, centerB );
% 
% 			I3ST( idFaceM, idTetraB ) = I3ST_S + I3ST_R;
% 
% 		end
		
	end
	
end