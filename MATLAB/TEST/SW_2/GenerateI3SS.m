function [ I3SS ] = GenerateI3SS( waveNum, faceCount, vertexList, faceVertexIndex, faceTetraIndex, faceMidPoint, faceQuadW, faceQuadR )
	
	% I3SS( m, n )
		
	I3SS = zeros( faceCount, faceCount );
	
	parfor idFaceM = 1 : faceCount
		
		if faceTetraIndex( 2, idFaceM ) == -1
		
			centerM = faceMidPoint( :, idFaceM );

			for idFaceN = 1 : faceCount

				v1N = vertexList( :, faceVertexIndex( 1, idFaceN ) );
				v2N = vertexList( :, faceVertexIndex( 2, idFaceN ) );
				v3N = vertexList( :, faceVertexIndex( 3, idFaceN ) );

				qNW = squeeze( faceQuadW( 1, idFaceN, : ) );
				qNX = squeeze( faceQuadR( 1, idFaceN, : ) );
				qNY = squeeze( faceQuadR( 2, idFaceN, : ) );
				qNZ = squeeze( faceQuadR( 3, idFaceN, : ) );

				I3SS_S = I.SingIntSur( centerM, v1N, v2N, v3N );
				I3SS_R = M.GreenRegInt( waveNum, centerM,  qNW, qNX, qNY, qNZ );

				I3SS( idFaceM, idFaceN ) = I3SS_S + I3SS_R;

			end
			
		end
		
	end
	
end


