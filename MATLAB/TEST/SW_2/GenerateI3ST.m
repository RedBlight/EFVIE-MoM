function [ I3ST ] = GenerateI3ST( waveNum, tetraCount, faceCount, vertexList, tetraVertexIndex, faceTetraIndex, faceMidPoint, tetraQuadW, tetraQuadR )
	
	% I3ST( m, b )
	% b is tetra index
	% m is face
		
	I3ST = zeros( faceCount, tetraCount );
	
	parfor idFaceM = 1 : faceCount
		
		if faceTetraIndex( 2, idFaceM ) == -1
		
			centerM = faceMidPoint( :, idFaceM );
		
			for idTetraB = 1 : tetraCount

				v1B = vertexList( :, tetraVertexIndex( 1, idTetraB ) );
				v2B = vertexList( :, tetraVertexIndex( 2, idTetraB ) );
				v3B = vertexList( :, tetraVertexIndex( 3, idTetraB ) );
				v4B = vertexList( :, tetraVertexIndex( 4, idTetraB ) );

				qBW = squeeze( tetraQuadW( 1, idTetraB, : ) );
				qBX = squeeze( tetraQuadR( 1, idTetraB, : ) );
				qBY = squeeze( tetraQuadR( 2, idTetraB, : ) );
				qBZ = squeeze( tetraQuadR( 3, idTetraB, : ) );

				I3ST_S = I.SingIntVol( centerM, v1B, v2B, v3B, v4B );
				I3ST_R = M.GreenRegInt( waveNum, centerM, qBW, qBX, qBY, qBZ );

				I3ST( idFaceM, idTetraB ) = I3ST_S + I3ST_R;

			end
			
		end
		
	end
	
end