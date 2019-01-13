function [ I3SS ] = GenerateI3SS( lastSingleBaseId, waveNum, faceCount, vertexList, faceVertexIndex, faceMidPoint, faceArea, relSS )
	
	% I3SS( m, n )
		
	I3SS = zeros( faceCount, faceCount );
	
	mjk = -1i * waveNum;
	
	idFaceVec = 1 : faceCount;
	
	parfor idFaceM = 1 : lastSingleBaseId
		
		faceAreaM = faceArea( idFaceM );
		centerM = faceMidPoint( :, idFaceM );
		matCenterM = repmat( centerM, 1, faceCount );
		
		vecR = faceMidPoint - matCenterM;
		R = sqrt( vecR(1,:).^2 + vecR(2,:).^2 + vecR(3,:).^2 )
		I3SS_M = faceArea .* exp( mjk .* R ) ./ R;
		
		for idFaceN = idFaceVec( relSS( idFaceM, : ) )

			faceAreaN = faceArea( idFaceN );
			centerN = faceMidPoint( :, idFaceN );
			
			v1N = vertexList( :, faceVertexIndex( 1, idFaceN ) );
			v2N = vertexList( :, faceVertexIndex( 2, idFaceN ) );
			v3N = vertexList( :, faceVertexIndex( 3, idFaceN ) );
			
			I3SS_M( idFaceN ) = I.SingIntSur( centerM, v1N, v2N, v3N ) + M.GreenRegInt( waveNum, faceAreaN, centerM, centerN );
			
		end
		
		I3SS( idFaceM, : ) = I3SS_M;
		
	end
	
end


