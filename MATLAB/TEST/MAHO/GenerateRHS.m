function [ RHS ] = GenerateRHS( ...
	incWave, ...
	faceCount, faceArea, tetraVol, ...
	vertexList, faceTetraIndex, swgVertexIndex, ...
	tetraQuadCount, tetraQuadW, tetraQuadR ...
)
	
	% RHS( m )
		
	RHS = zeros( faceCount, 1 );
	
	for idFaceM = 1 : faceCount
		
		idTetraM1 = faceTetraIndex( 1, idFaceM );
		idTetraM2 = faceTetraIndex( 2, idFaceM );
		
		faceAreaM = faceArea( idFaceM );
		
		rhsM = 0;
		
		% +
		tetraVolM1 = tetraVol( idTetraM1 );
		SM1 = vertexList( :, swgVertexIndex( 1, idFaceM ) );
		Swg1 = @( r_ ) ( faceAreaM / ( 3 * tetraVolM1 ) ) .* ( r_ - SM1 );
		
		for idQuadM = 1 : tetraQuadCount
			
			qW = tetraQuadW( 1, idTetraM1, idQuadM );
			qR = tetraQuadR( :, idTetraM1, idQuadM );
			
			rhsM = rhsM + qW * M.VecDot( incWave.FieldE( qR ), Swg1( qR ) );
			
		end
		
		% -
		if idTetraM2 ~= -1
			
			tetraVolM2 = tetraVol( idTetraM2 );
			SM2 = vertexList( :, swgVertexIndex( 2, idFaceM ) );
			Swg2 = @( r_ ) ( faceAreaM / ( 3 * tetraVolM2 ) ) .* ( SM2 - r_ );

			for idQuadM = 1 : tetraQuadCount

				qW = tetraQuadW( 1, idTetraM2, idQuadM );
				qR = tetraQuadR( :, idTetraM2, idQuadM );

				rhsM = rhsM + qW * M.VecDot( incWave.FieldE( qR ), Swg2( qR ) );

			end
			
		end
		
		RHS( idFaceM ) = rhsM;
		
	end
	
end