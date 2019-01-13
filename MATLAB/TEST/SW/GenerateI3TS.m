function [ I3TS ] = GenerateI3TS( waveNum, tetraCount, faceCount, vertexList, faceVertexIndex, tetraMidPoint, faceMidPoint, faceArea, relTS )
	
	% I3TS( a, n )
	% a is tetra index
	% n is face
		
	I3TS = zeros( tetraCount, faceCount );
	
	mjk = -1i * waveNum;
	
	idFaceVec = 1 : faceCount;
	
	parfor idTetraA = 1 : tetraCount
		
		centerA = tetraMidPoint( :, idTetraA );
		matCenterA = repmat( centerA, 1, faceCount );
		
		vecR = faceMidPoint - matCenterA;
		R = sqrt( vecR(1,:).^2 + vecR(2,:).^2 + vecR(3,:).^2 )
		I3TS_M = faceArea .* exp( mjk .* R ) ./ R;
		
		for idFaceN = idFaceVec( relTS( idTetraA, : ) )

			faceAreaN = faceArea( idFaceN );
			centerN = faceMidPoint( :, idFaceN );
			
			v1N = vertexList( :, faceVertexIndex( 1, idFaceN ) );
			v2N = vertexList( :, faceVertexIndex( 2, idFaceN ) );
			v3N = vertexList( :, faceVertexIndex( 3, idFaceN ) );
			
			I3TS_M( idFaceN ) = I.SingIntSur( centerA, v1N, v2N, v3N ) + M.GreenRegInt( waveNum, faceAreaN, centerA, centerN );
			
		end
		
		I3TS( idTetraA, : ) = I3TS_M;
		
		
		
% 		centerA = tetraMidPoint( :, idTetraA );
% 		
% 		for idFaceN = 1 : faceCount
% 			
% 			centerN = faceMidPoint( :, idFaceN );
% 			faceAreaN = faceArea( idFaceN );
% 			
% 			v1N = vertexList( :, faceVertexIndex( 1, idFaceN ) );
% 			v2N = vertexList( :, faceVertexIndex( 2, idFaceN ) );
% 			v3N = vertexList( :, faceVertexIndex( 3, idFaceN ) );
% 			
% 			I3TS_S = I.SingIntSur( centerA, v1N, v2N, v3N );
% 			I3TS_R = M.GreenRegInt( waveNum, faceAreaN, centerA, centerN );
% 			
% 			I3TS( idTetraA, idFaceN ) = I3TS_S + I3TS_R;
% 			
% 		end
		
	end
	
end