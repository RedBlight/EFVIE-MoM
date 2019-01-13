function [ I3SV ] = GenerateI3SV( waveNum, vertexList, faceCount, tetraCount, faceVertexIndex, faceQuadW, faceQuadR, tetraQuadCount, tetraQuadW, tetraQuadR )
	
	% I3VV( m, b )
	% a is tetra index
	% b is tetra index
		
	I3SV = zeros( faceCount, tetraCount );
	
	for idTetraB = 1 : tetraCount
		
		qBW = squeeze( tetraQuadW( 1, idTetraB, : ) );
		qBX = squeeze( tetraQuadR( 1, idTetraB, : ) );
		qBY = squeeze( tetraQuadR( 2, idTetraB, : ) );
		qBZ = squeeze( tetraQuadR( 3, idTetraB, : ) );
		
		for idFaceM = 1 : faceCount
			
			v1M = vertexList( :, faceVertexIndex( 1, idFaceM ) );
			v2M = vertexList( :, faceVertexIndex( 2, idFaceM ) );
			v3M = vertexList( :, faceVertexIndex( 3, idFaceM ) );
			
			qMW = squeeze( faceQuadW( 1, idFaceM, : ) );
			qMX = squeeze( faceQuadR( 1, idFaceM, : ) );
			qMY = squeeze( faceQuadR( 2, idFaceM, : ) );
			qMZ = squeeze( faceQuadR( 3, idFaceM, : ) );
			
			I3SV_S = 0;
			I3SV_R = 0;
			
			for idQuadB = 1 : tetraQuadCount
				
				qBW_ = qBW( idQuadB );
				qBR_ = [ qBX( idQuadB ); qBY( idQuadB ); qBZ( idQuadB ) ];
				
				I3SV_S = I3SV_S + qBW_ * I.SingIntSur( qBR_, v1M, v2M, v3M );
				I3SV_R = I3SV_R + qBW_ * M.GreenRegInt( waveNum, qBR_, qMW, qMX, qMY, qMZ );
				
			end
			
			I3SV( idFaceM, idTetraB ) = I3SV_S + I3SV_R;
			
		end
		
	end
	
end