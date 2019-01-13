function [ I3ST ] = GenerateI3ST( waveNum, tetraCount, faceCount, vertexList, tetraVertexIndex, faceTetraIndex, faceMidPoint, tetraMidPoint, tetraVol )
	
	% I3ST( m, b )
	% b is tetra index
	% m is face
		
	I3ST = zeros( faceCount, tetraCount );
	
	lastSingleBaseId = M.FindMin1( faceTetraIndex( 2, : ) );
	
	parfor idFaceM = 1 : lastSingleBaseId
		
		centerM = faceMidPoint( :, idFaceM );

		for idTetraB = 1 : tetraCount
			
			centerB = tetraMidPoint( :, idTetraB );
			tetraVolB = tetraVol( idTetraB );

			v1B = vertexList( :, tetraVertexIndex( 1, idTetraB ) );
			v2B = vertexList( :, tetraVertexIndex( 2, idTetraB ) );
			v3B = vertexList( :, tetraVertexIndex( 3, idTetraB ) );
			v4B = vertexList( :, tetraVertexIndex( 4, idTetraB ) );

			I3ST_S = I.SingIntVol( centerM, v1B, v2B, v3B, v4B );
			I3ST_R = M.GreenRegInt( waveNum, tetraVolB, centerM, centerB );

			I3ST( idFaceM, idTetraB ) = I3ST_S + I3ST_R;

		end
		
	end
	
end