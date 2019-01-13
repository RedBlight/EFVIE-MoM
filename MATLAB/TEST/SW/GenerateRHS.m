function [ RHS ] = GenerateRHS( ...
	incWave, ...
	faceCount, faceArea, ...
	vertexList, faceTetraIndex, swgVertexIndex, ...
	tetraMidPoint ...
)
	
	% RHS( m )
		
	RHS = zeros( faceCount, 1 );
	
	for idFaceM = 1 : faceCount
		
		idTetraM1 = faceTetraIndex( 1, idFaceM );
		idTetraM2 = faceTetraIndex( 2, idFaceM );
		
		faceAreaM = faceArea( idFaceM );
		
		% +
		tetraMidM1 = tetraMidPoint( :, idTetraM1 );
		SM1 = vertexList( :, swgVertexIndex( 1, idFaceM ) );
		rhsM = M.VecDot( incWave.FieldE( tetraMidM1 ), tetraMidM1 - SM1 );
		
		% -
		if idTetraM2 ~= -1
			tetraMidM2 = tetraMidPoint( :, idTetraM2 );
			SM2 = vertexList( :, swgVertexIndex( 2, idFaceM ) );
			rhsM = rhsM + M.VecDot( incWave.FieldE( tetraMidM2 ), SM2 - tetraMidM2 );
		end
		
		RHS( idFaceM ) = ( faceAreaM / 3 ) .* rhsM;
		
	end
	
end