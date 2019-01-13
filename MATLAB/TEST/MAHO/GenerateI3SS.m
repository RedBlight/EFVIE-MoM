function [ I3SS ] = GenerateI3SS( waveNum, vertexList, faceCount, faceVertexIndex, faceQuadCount, faceQuadW, faceQuadR )
	
	% I3SS( m, n )
		
	I3SS = zeros( faceCount, faceCount );
	
	for idFaceN = 1 : faceCount
		
		qNW = squeeze( faceQuadW( 1, idFaceN, : ) );
		qNX = squeeze( faceQuadR( 1, idFaceN, : ) );
		qNY = squeeze( faceQuadR( 2, idFaceN, : ) );
		qNZ = squeeze( faceQuadR( 3, idFaceN, : ) );
		
		for idFaceM = 1 : faceCount
			
			v1M = vertexList( :, faceVertexIndex( 1, idFaceM ) );
			v2M = vertexList( :, faceVertexIndex( 2, idFaceM ) );
			v3M = vertexList( :, faceVertexIndex( 3, idFaceM ) );
			
			qMW = squeeze( faceQuadW( 1, idFaceM, : ) );
			qMX = squeeze( faceQuadR( 1, idFaceM, : ) );
			qMY = squeeze( faceQuadR( 2, idFaceM, : ) );
			qMZ = squeeze( faceQuadR( 3, idFaceM, : ) );
			
			I3SS_S = 0;
			I3SS_R = 0;
			
			for idQuadN = 1 : faceQuadCount
				
				qNW_ = qNW( idQuadN );
				qNR_ = [ qNX( idQuadN ); qNY( idQuadN ); qNZ( idQuadN ) ];
				
				I3SS_S = I3SS_S + qNW_ * I.SingIntSur( qNR_, v1M, v2M, v3M );
				I3SS_R = I3SS_R + qNW_ * M.GreenRegInt( waveNum, qNR_, qMW, qMX, qMY, qMZ );
				
			end
			
			I3SS( idFaceM, idFaceN ) = I3SS_S + I3SS_R;
			
		end
		
	end
	
end

