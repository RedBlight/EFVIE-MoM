function [ I3SS ] = GenerateI3SS( waveNum, faceCount, vertexList, faceVertexIndex, faceTetraIndex, faceMidPoint, faceArea )
	
	% I3SS( m, n )
		
	I3SS = zeros( faceCount, faceCount );
	
	lastSingleBaseId = M.FindMin1( faceTetraIndex( 2, : ) );
	
	disp(lastSingleBaseId);
	
	parfor idFaceM = 1 : lastSingleBaseId

		centerM = faceMidPoint( :, idFaceM );

		for idFaceN = 1 : faceCount
			
			centerN = faceMidPoint( :, idFaceN );
			faceAreaN = faceArea( idFaceN );

			v1N = vertexList( :, faceVertexIndex( 1, idFaceN ) );
			v2N = vertexList( :, faceVertexIndex( 2, idFaceN ) );
			v3N = vertexList( :, faceVertexIndex( 3, idFaceN ) );

			I3SS_S = I.SingIntSur( centerM, v1N, v2N, v3N );
			I3SS_R = M.GreenRegInt( waveNum, faceAreaN, centerM, centerN );

			I3SS( idFaceM, idFaceN ) = I3SS_S + I3SS_R;

		end
		
	end
	
end


