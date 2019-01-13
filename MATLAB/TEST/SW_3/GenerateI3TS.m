function [ I3TS ] = GenerateI3TS( waveNum, tetraCount, faceCount, vertexList, faceVertexIndex, tetraMidPoint, faceMidPoint, faceArea )
	
	% I3TS( a, n )
	% a is tetra index
	% n is face
		
	I3TS = zeros( tetraCount, faceCount );
	
	parfor idTetraA = 1 : tetraCount
		
		centerA = tetraMidPoint( :, idTetraA );
		
		for idFaceN = 1 : faceCount
			
			centerN = faceMidPoint( :, idFaceN );
			faceAreaN = faceArea( idFaceN );
			
			v1N = vertexList( :, faceVertexIndex( 1, idFaceN ) );
			v2N = vertexList( :, faceVertexIndex( 2, idFaceN ) );
			v3N = vertexList( :, faceVertexIndex( 3, idFaceN ) );
			
			I3TS_S = I.SingIntSur( centerA, v1N, v2N, v3N );
			I3TS_R = M.GreenRegInt( waveNum, faceAreaN, centerA, centerN );
			
			I3TS( idTetraA, idFaceN ) = I3TS_S + I3TS_R;
			
		end
		
	end
	
end