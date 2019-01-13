function [ momTerm2 ] = GenerateMoM_Term2( ...
	angFreq, faceCount, ...
	vertexList, swgVertexIndex, faceTetraIndex, ...
	faceArea, tetraVol, kontrast, ...
	tetraMidPoint, ...
	I2, I3TT ...
)
	% m = which row, n = which col
	
	momTerm2 = zeros( faceCount, faceCount );
	
	for idFaceM = 1 : faceCount
		for idFaceN = 1 : faceCount
			
			idTetraM1 = faceTetraIndex( 1, idFaceM );
			idTetraM2 = faceTetraIndex( 2, idFaceM );
			
			idTetraN1 = faceTetraIndex( 1, idFaceN );
			idTetraN2 = faceTetraIndex( 2, idFaceN );
			
			isSingleM = idTetraM2 == -1;
			isSingleN = idTetraN2 == -1;
			
			if( ~isSingleM && ~isSingleN )
				
				SN1 = vertexList( :, swgVertexIndex( 1, idFaceN ) );
				SN2 = vertexList( :, swgVertexIndex( 2, idFaceN ) );
				
				SM1 = vertexList( :, swgVertexIndex( 1, idFaceM ) );
				SM2 = vertexList( :, swgVertexIndex( 2, idFaceM ) );
				
				MC1 = tetraMidPoint( :, idTetraM1 );
				MC2 = tetraMidPoint( :, idTetraM2 );
				
				K = - angFreq * angFreq * M.mu0 * faceArea( idFaceN ) * faceArea( idFaceM ) / ( 36 * pi );
				
				CN1 = kontrast( idTetraN1 ) / tetraVol( idTetraN1 );
				CN2 = - kontrast( idTetraN2 ) / tetraVol( idTetraN2 );
				
				I_11 = I2( :, idTetraM1, idTetraN1 ) + ( MC1 - SN1 ) .* I3TT( idTetraM1, idTetraN1 );
				I_12 = I2( :, idTetraM1, idTetraN2 ) + ( MC1 - SN2 ) .* I3TT( idTetraM1, idTetraN2 );
				I_21 = I2( :, idTetraM2, idTetraN1 ) + ( MC2 - SN1 ) .* I3TT( idTetraM2, idTetraN1 );
				I_22 = I2( :, idTetraM2, idTetraN2 ) + ( MC2 - SN2 ) .* I3TT( idTetraM2, idTetraN2 );
				
				mt2 = K * ( ...
					+ M.VecDot( MC1 - SM1, CN1 .* I_11 + CN2 .* I_12 ) ...
					- M.VecDot( MC2 - SM2, CN1 .* I_21 + CN2 .* I_22 ) ...
				);
				
			elseif( ~isSingleM && isSingleN )
				
				SN1 = vertexList( :, swgVertexIndex( 1, idFaceN ) );
				
				SM1 = vertexList( :, swgVertexIndex( 1, idFaceM ) );
				SM2 = vertexList( :, swgVertexIndex( 2, idFaceM ) );
				
				MC1 = tetraMidPoint( :, idTetraM1 );
				MC2 = tetraMidPoint( :, idTetraM2 );
				
				K = - angFreq * angFreq * M.mu0 * faceArea( idFaceN ) * faceArea( idFaceM ) / ( 36 * pi );
				
				CN1 = kontrast( idTetraN1 ) / tetraVol( idTetraN1 );
				
				I_11 = I2( :, idTetraM1, idTetraN1 ) + ( MC1 - SN1 ) .* I3TT( idTetraM1, idTetraN1 );
				I_21 = I2( :, idTetraM2, idTetraN1 ) + ( MC2 - SN1 ) .* I3TT( idTetraM2, idTetraN1 );
				
				mt2 = K * ( ...
					+ M.VecDot( MC1 - SM1, CN1 .* I_11 ) ...
					- M.VecDot( MC2 - SM2, CN1 .* I_21 ) ...
				);
					
			elseif( isSingleM && ~isSingleN )
				
				SN1 = vertexList( :, swgVertexIndex( 1, idFaceN ) );
				SN2 = vertexList( :, swgVertexIndex( 2, idFaceN ) );
				
				SM1 = vertexList( :, swgVertexIndex( 1, idFaceM ) );
				
				MC1 = tetraMidPoint( :, idTetraM1 );
				
				K = - angFreq * angFreq * M.mu0 * faceArea( idFaceN ) * faceArea( idFaceM ) / ( 36 * pi );
				
				CN1 = kontrast( idTetraN1 ) / tetraVol( idTetraN1 );
				CN2 = - kontrast( idTetraN2 ) / tetraVol( idTetraN2 );
				
				I_11 = I2( :, idTetraM1, idTetraN1 ) + ( MC1 - SN1 ) .* I3TT( idTetraM1, idTetraN1 );
				I_12 = I2( :, idTetraM1, idTetraN2 ) + ( MC1 - SN2 ) .* I3TT( idTetraM1, idTetraN2 );
				
				mt2 = K * ( ...
					+ M.VecDot( MC1 - SM1, CN1 .* I_11 + CN2 .* I_12 ) ...
				);
					
			elseif( isSingleM && isSingleN )
				
				SN1 = vertexList( :, swgVertexIndex( 1, idFaceN ) );
				
				SM1 = vertexList( :, swgVertexIndex( 1, idFaceM ) );
				
				MC1 = tetraMidPoint( :, idTetraM1 );
				
				K = - angFreq * angFreq * M.mu0 * faceArea( idFaceN ) * faceArea( idFaceM ) / ( 36 * pi );
				
				CN1 = kontrast( idTetraN1 ) / tetraVol( idTetraN1 );
				
				I_11 = I2( :, idTetraM1, idTetraN1 ) + ( MC1 - SN1 ) .* I3TT( idTetraM1, idTetraN1 );
				
				mt2 = K * ( ...
					+ M.VecDot( MC1 - SM1, CN1 .* I_11 ) ...
				);
				
			end
			
			momTerm2( idFaceM, idFaceN ) = mt2;
			
		end
	end
	
	
end

