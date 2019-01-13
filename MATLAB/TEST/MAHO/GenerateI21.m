function [ I21 ] = GenerateI21( waveNum, vertexList, tetraCount, tetraVertexIndex, tetraQuadCount, tetraQuadW, tetraQuadR )
	
	% I3VV( a, b )
	% a is tetra index
	% b is tetra index
		
	I21 = zeros( tetraCount, tetraCount );
	
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
			
			I21_S = 0;
			I21_R = 0;
			
			for idQuadB = 1 : tetraQuadCount
				
				qBW_ = qBW( idQuadB );
				qBR_ = [ qBX( idQuadB ); qBY( idQuadB ); qBZ( idQuadB ) ];
				
				SIVol = I.SingIntVol( qBR_, v1A, v2A, v3A, v4A );
				SIVec = I.SingIntVec( qBR_, v1A, v2A, v3A, v4A );
				
				RIVec = M.GreenRegIntVec( waveNum, qBR_, qAW, qAX, qAY, qAZ );
				
				I21_S = I21_S + qBW_ * ( M.VecDot( qBR_, qBR_ ) * SIVol + M.VecDot( qBR_, SIVec ) );
				I21_R = I21_R + qBW_ * M.VecDot( qBR_, RIVec );
				
			end
			
			I21( idTetraA, idTetraB ) = I21_S + I21_R;
			
		end
		
	end
	
end