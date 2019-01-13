function [ I23 ] = GenerateI23( waveNum, vertexList, tetraCount, tetraVertexIndex, tetraQuadCount, tetraQuadW, tetraQuadR )
	
	% I3VV( a, b )
	% a is tetra index
	% b is tetra index
		
	I23 = zeros( 3, tetraCount, tetraCount );
	
	for idTetraB = 1 : tetraCount
		
		qBW = squeeze( tetraQuadW( 1, idTetraB, : ) );
		qBX = squeeze( tetraQuadR( 1, idTetraB, : ) );
		qBY = squeeze( tetraQuadR( 2, idTetraB, : ) );
		qBZ = squeeze( tetraQuadR( 3, idTetraB, : ) );
		
		for idTetraA = 1 : tetraCount
			
			v1A = vertexList( :, tetraVertexIndex( 1, idTetraA ) );
			v2A = vertexList( :, tetraVertexIndex( 2, idTetraA ) );
			v3A = vertexList( :, tetraVertexIndex( 3, idTetraA ) );
			v4A = vertexList( :, tetraVertexIndex( 4, idTetraA ) );
			
			qAW = squeeze( tetraQuadW( 1, idTetraA, : ) );
			qAX = squeeze( tetraQuadR( 1, idTetraA, : ) );
			qAY = squeeze( tetraQuadR( 2, idTetraA, : ) );
			qAZ = squeeze( tetraQuadR( 3, idTetraA, : ) );
			
			I23_S = [ 0; 0; 0 ];
			I23_R = [ 0; 0; 0 ];
			
			for idQuadB = 1 : tetraQuadCount
				
				qBW_ = qBW( idQuadB );
				qBR_ = [ qBX( idQuadB ); qBY( idQuadB ); qBZ( idQuadB ) ];
				
				I23_S = I23_S + qBW_ .* qBR_ .* I.SingIntVol( qBR_, v1A, v2A, v3A, v4A );
				I23_R = I23_R + qBW_ .* qBR_ .* M.GreenRegInt( waveNum, qBR_, qAW, qAX, qAY, qAZ );
				
			end
			
			I23( :, idTetraA, idTetraB ) = I23_S + I23_R;
			
		end
		
	end
	
end