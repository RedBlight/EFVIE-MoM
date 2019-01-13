function [ I3TS ] = GenerateI3TS( waveNum, tetraCount, faceCount, vertexList, faceVertexIndex, tetraMidPoint, faceQuadW, faceQuadR )
	
	% I3TS( a, n )
	% a is tetra index
	% n is face
		
	I3TS = zeros( tetraCount, faceCount );
	
	parfor idTetraA = 1 : tetraCount
		
		centerA = tetraMidPoint( :, idTetraA );
		
		for idFaceN = 1 : faceCount
			
			v1N = vertexList( :, faceVertexIndex( 1, idFaceN ) );
			v2N = vertexList( :, faceVertexIndex( 2, idFaceN ) );
			v3N = vertexList( :, faceVertexIndex( 3, idFaceN ) );
			
			qNW = squeeze( faceQuadW( 1, idFaceN, : ) );
			qNX = squeeze( faceQuadR( 1, idFaceN, : ) );
			qNY = squeeze( faceQuadR( 2, idFaceN, : ) );
			qNZ = squeeze( faceQuadR( 3, idFaceN, : ) );
			
			I3TS_S = I.SingIntSur( centerA, v1N, v2N, v3N );
			I3TS_R = M.GreenRegInt( waveNum, centerA, qNW, qNX, qNY, qNZ );
			
			I3TS( idTetraA, idFaceN ) = I3TS_S + I3TS_R;
			
		end
		
	end
	
end