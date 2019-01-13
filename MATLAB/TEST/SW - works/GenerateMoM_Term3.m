function [ momTerm3 ] = GenerateMoM_Term3( ...
	faceCount, faceTetraIndex, ...
	faceArea, tetraVol, kontrast, kontrastDiff, ...
	I3TT, I3TS, I3ST, I3SS ...
)
	% m = which row, n = which col
	
	momTerm3 = zeros( faceCount, faceCount );
	
	for idFaceM = 1 : faceCount
		for idFaceN = 1 : faceCount
			
			idTetraM1 = faceTetraIndex( 1, idFaceM );
			idTetraM2 = faceTetraIndex( 2, idFaceM );
			
			idTetraN1 = faceTetraIndex( 1, idFaceN );
			idTetraN2 = faceTetraIndex( 2, idFaceN );
			
			isSingleM = idTetraM2 == -1;
			isSingleN = idTetraN2 == -1;
			
			if( ~isSingleM && ~isSingleN )
				
				C1 = - faceArea( idFaceN ) / ( 4 * pi * M.eps0 );
				C2 = kontrast( idTetraN1 ) / tetraVol( idTetraN1 );
				C3 = - kontrast( idTetraN2 ) / tetraVol( idTetraN2 );
				C4 = - kontrastDiff( idFaceN ) / faceArea( idFaceN );
				
				phi1 = C1 * ( ...
					C2 * I3TT( idTetraM1, idTetraN1 ) ...
					+ C3 * I3TT( idTetraM1, idTetraN2 ) ...
					+ C4 * I3TS( idTetraM1, idFaceN ) ...
				);
				
				phi2 = C1 * ( ...
					C2 * I3TT( idTetraM2, idTetraN1 ) ...
					+ C3 * I3TT( idTetraM2, idTetraN2 ) ...
					+ C4 * I3TS( idTetraM2, idFaceN ) ...
				);
			
				mt3 = faceArea( idFaceM ) * ( phi2 - phi1 );
				
			elseif( ~isSingleM && isSingleN )
				
				C1 = - faceArea( idFaceN ) / ( 4 * pi * M.eps0 );
				C2 = kontrast( idTetraN1 ) / tetraVol( idTetraN1 );
				C4 = - kontrastDiff( idFaceN ) / faceArea( idFaceN );
				
				phi1 = C1 * ( ...
					C2 * I3TT( idTetraM1, idTetraN1 ) ...
					+ C4 * I3TS( idTetraM1, idFaceN ) ...
				);
				
				phi2 = C1 * ( ...
					C2 * I3TT( idTetraM2, idTetraN1 ) ...
					+ C4 * I3TS( idTetraM2, idFaceN ) ...
				);
			
				mt3 = faceArea( idFaceM ) * ( phi2 - phi1 );
					
			elseif( isSingleM && ~isSingleN )
				
				C1 = - faceArea( idFaceN ) / ( 4 * pi * M.eps0 );
				C2 = kontrast( idTetraN1 ) / tetraVol( idTetraN1 );
				C3 = - kontrast( idTetraN2 ) / tetraVol( idTetraN2 );
				C4 = - kontrastDiff( idFaceN ) / faceArea( idFaceN );
				
				phi1 = C1 * ( ...
					C2 * I3TT( idTetraM1, idTetraN1 ) ...
					+ C3 * I3TT( idTetraM1, idTetraN2 ) ...
					+ C4 * I3TS( idTetraM1, idFaceN ) ...
				);
				
				phi2 = C1 * ( ...
					C2 * I3ST( idFaceM, idTetraN1 ) ...
					+ C3 * I3ST( idFaceM, idTetraN2 ) ...
					+ C4 * I3SS( idFaceM, idFaceN ) ...
				);
			
				mt3 = faceArea( idFaceM ) * ( phi2 - phi1 );
					
			elseif( isSingleM && isSingleN )
				
				C1 = - faceArea( idFaceN ) / ( 4 * pi * M.eps0 );
				C2 = kontrast( idTetraN1 ) / tetraVol( idTetraN1 );
				C4 = - kontrastDiff( idFaceN ) / faceArea( idFaceN );
				
				phi1 = C1 * ( ...
					C2 * I3TT( idTetraM1, idTetraN1 ) ...
					+ C4 * I3TS( idTetraM1, idFaceN ) ...
				);
				
				phi2 = C1 * ( ...
					C2 * I3ST( idFaceM, idTetraN1 ) ...
					+ C4 * I3SS( idFaceM, idFaceN ) ...
				);
			
				mt3 = faceArea( idFaceM ) * ( phi2 - phi1 );
				
			end
			
			momTerm3( idFaceM, idFaceN ) = mt3;
			
		end
	end
	
	
end

