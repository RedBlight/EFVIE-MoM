function [ I3VS ] = GenerateI3VS( waveNum, vertexList, faceCount, tetraCount, tetraVertexIndex, faceQuadCount, faceQuadW, faceQuadR, tetraQuadW, tetraQuadR )
	
	% I3VS( a, n )
	% a is tetra index
	% n is face
		
	I3VS = zeros( tetraCount, faceCount );
	
	for idFaceN = 1 : faceCount
		
		qNW = squeeze( faceQuadW( 1, idFaceN, : ) );
		qNX = squeeze( faceQuadR( 1, idFaceN, : ) );
		qNY = squeeze( faceQuadR( 2, idFaceN, : ) );
		qNZ = squeeze( faceQuadR( 3, idFaceN, : ) );
		
		for idTetraA = 1 : tetraCount
			
			v1A = vertexList( :, tetraVertexIndex( 1, idTetraA ) );
			v2A = vertexList( :, tetraVertexIndex( 2, idTetraA ) );
			v3A = vertexList( :, tetraVertexIndex( 3, idTetraA ) );
			v4A = vertexList( :, tetraVertexIndex( 4, idTetraA ) );
			
			qAW = squeeze( tetraQuadW( 1, idTetraA, : ) );
			qAX = squeeze( tetraQuadR( 1, idTetraA, : ) );
			qAY = squeeze( tetraQuadR( 2, idTetraA, : ) );
			qAZ = squeeze( tetraQuadR( 3, idTetraA, : ) );
			
			I3VS_S = 0;
			I3VS_R = 0;
			
			for idQuadN = 1 : faceQuadCount
				
				qNW_ = qNW( idQuadN );
				qNR_ = [ qNX( idQuadN ); qNY( idQuadN ); qNZ( idQuadN ) ];
				
				I3VS_S = I3VS_S + qNW_ * I.SingIntVol( qNR_, v1A, v2A, v3A, v4A );
				I3VS_R = I3VS_R + qNW_ * M.GreenRegInt( waveNum, qNR_, qAW, qAX, qAY, qAZ );
				
			end
			
			I3VS( idTetraA, idFaceN ) = I3VS_S + I3VS_R;
			
		end
		
	end
	
end