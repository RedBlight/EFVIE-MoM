function [ momTerm2 ] = GenerateMoM_Term2( ...
	faceCount, faceTetraIndex, ...
	vertexList, swgVertexIndex, ...
	I21, I22, I23, I3VV, ...
	C21 ...
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
			
			% C31( n, b )
			% C32( n )
			% C33( m, a, n, b )
			% C34( m, a, n )
			
% 			C31( idFaceN, 1 )
% 			C32( idFaceN )
% 			C33( idFaceM, 1, idFaceN, 1 )
% 			C34( idFaceM, a, idFaceN )
			
			if( ~isSingleM && ~isSingleN )
				
				C21_11 = C21( idFaceM, 1, idFaceN, 1 );
				C21_21 = C21( idFaceM, 2, idFaceN, 1 );
				C21_12 = C21( idFaceM, 1, idFaceN, 2 );
				C21_22 = C21( idFaceM, 2, idFaceN, 2 );
				
				I21_11 = I21( idTetraM1, idTetraN1 );
				I21_21 = I21( idTetraM2, idTetraN1 );
				I21_12 = I21( idTetraM1, idTetraN2 );
				I21_22 = I21( idTetraM2, idTetraN2 );
				
				I22_11 = I22( :, idTetraM1, idTetraN1 );
				I22_21 = I22( :, idTetraM2, idTetraN1 );
				I22_12 = I22( :, idTetraM1, idTetraN2 );
				I22_22 = I22( :, idTetraM2, idTetraN2 );
				
				I23_11 = I23( :, idTetraM1, idTetraN1 );
				I23_21 = I23( :, idTetraM2, idTetraN1 );
				I23_12 = I23( :, idTetraM1, idTetraN2 );
				I23_22 = I23( :, idTetraM2, idTetraN2 );
				
				I3VV_11 = I3VV( idTetraM1, idTetraN1 );
				I3VV_21 = I3VV( idTetraM2, idTetraN1 );
				I3VV_12 = I3VV( idTetraM1, idTetraN2 );
				I3VV_22 = I3VV( idTetraM2, idTetraN2 );
				
				SM1 = vertexList( :, swgVertexIndex( 1, idFaceM ) );
				SM2 = vertexList( :, swgVertexIndex( 2, idFaceM ) );
				
				SN1 = vertexList( :, swgVertexIndex( 1, idFaceN ) );
				SN2 = vertexList( :, swgVertexIndex( 2, idFaceN ) );
				
				momTerm2( idFaceM, idFaceN ) = ...
					- C21_11 * ( I21_11 - M.VecDot( SN1, I22_11 ) - M.VecDot( SM1, I23_11 ) + M.VecDot( SM1, SN1 ) * I3VV_11 ) ...
					- C21_21 * ( I21_21 - M.VecDot( SN1, I22_21 ) - M.VecDot( SM2, I23_21 ) + M.VecDot( SM2, SN1 ) * I3VV_21 ) ...
					+ C21_12 * ( I21_12 - M.VecDot( SN2, I22_12 ) - M.VecDot( SM1, I23_12 ) + M.VecDot( SM1, SN2 ) * I3VV_12 ) ...
					+ C21_22 * ( I21_22 - M.VecDot( SN2, I22_22 ) - M.VecDot( SM2, I23_22 ) + M.VecDot( SM2, SN2 ) * I3VV_22 ) ...
				;
				
			elseif( ~isSingleM && isSingleN )
				
				C21_11 = C21( idFaceM, 1, idFaceN, 1 );
				C21_21 = C21( idFaceM, 2, idFaceN, 1 );
				
				I21_11 = I21( idTetraM1, idTetraN1 );
				I21_21 = I21( idTetraM2, idTetraN1 );
				
				I22_11 = I22( :, idTetraM1, idTetraN1 );
				I22_21 = I22( :, idTetraM2, idTetraN1 );
				
				I23_11 = I23( :, idTetraM1, idTetraN1 );
				I23_21 = I23( :, idTetraM2, idTetraN1 );
				
				I3VV_11 = I3VV( idTetraM1, idTetraN1 );
				I3VV_21 = I3VV( idTetraM2, idTetraN1 );
				
				SM1 = vertexList( :, swgVertexIndex( 1, idFaceM ) );
				SM2 = vertexList( :, swgVertexIndex( 2, idFaceM ) );
				
				SN1 = vertexList( :, swgVertexIndex( 1, idFaceN ) );
				
				momTerm2( idFaceM, idFaceN ) = ...
					- C21_11 * ( I21_11 - M.VecDot( SN1, I22_11 ) - M.VecDot( SM1, I23_11 ) + M.VecDot( SM1, SN1 ) * I3VV_11 ) ...
					- C21_21 * ( I21_21 - M.VecDot( SN1, I22_21 ) - M.VecDot( SM2, I23_21 ) + M.VecDot( SM2, SN1 ) * I3VV_21 ) ...
				;
					
			elseif( isSingleM && ~isSingleN )
				
				C21_11 = C21( idFaceM, 1, idFaceN, 1 );
				C21_12 = C21( idFaceM, 1, idFaceN, 2 );
				
				I21_11 = I21( idTetraM1, idTetraN1 );
				I21_12 = I21( idTetraM1, idTetraN2 );
				
				I22_11 = I22( :, idTetraM1, idTetraN1 );
				I22_12 = I22( :, idTetraM1, idTetraN2 );
				
				I23_11 = I23( :, idTetraM1, idTetraN1 );
				I23_12 = I23( :, idTetraM1, idTetraN2 );
				
				I3VV_11 = I3VV( idTetraM1, idTetraN1 );
				I3VV_12 = I3VV( idTetraM1, idTetraN2 );
				
				SM1 = vertexList( :, swgVertexIndex( 1, idFaceM ) );
				
				SN1 = vertexList( :, swgVertexIndex( 1, idFaceN ) );
				SN2 = vertexList( :, swgVertexIndex( 2, idFaceN ) );
				
				momTerm2( idFaceM, idFaceN ) = ...
					- C21_11 * ( I21_11 - M.VecDot( SN1, I22_11 ) - M.VecDot( SM1, I23_11 ) + M.VecDot( SM1, SN1 ) * I3VV_11 ) ...
					+ C21_12 * ( I21_12 - M.VecDot( SN2, I22_12 ) - M.VecDot( SM1, I23_12 ) + M.VecDot( SM1, SN2 ) * I3VV_12 ) ...
				;
					
			elseif( isSingleM && isSingleN )
				
				C21_11 = C21( idFaceM, 1, idFaceN, 1 );
				
				I21_11 = I21( idTetraM1, idTetraN1 );
				
				I22_11 = I22( :, idTetraM1, idTetraN1 );
				
				I23_11 = I23( :, idTetraM1, idTetraN1 );
				
				I3VV_11 = I3VV( idTetraM1, idTetraN1 );
				
				SM1 = vertexList( :, swgVertexIndex( 1, idFaceM ) );
				
				SN1 = vertexList( :, swgVertexIndex( 1, idFaceN ) );
				
				momTerm2( idFaceM, idFaceN ) = ...
					- C21_11 * ( I21_11 - M.VecDot( SN1, I22_11 ) - M.VecDot( SM1, I23_11 ) + M.VecDot( SM1, SN1 ) * I3VV_11 ) ...
				;
				
			end
			
		end
	end
	
	
end

