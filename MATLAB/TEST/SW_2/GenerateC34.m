function [ C34 ] = GenerateC34( faceCount, faceTetraIndex, kontrastDiff, faceArea, tetraVol )
	
	% C34( m, a, n )
	% a := { 1, 2 }
	
	C34 = zeros( faceCount, 2, faceCount );
	
	K = 1 / ( 4 * pi * M.eps0 );
	
	for idFaceM = 1 : faceCount
		for idFaceN = 1 : faceCount
			
			idTetM1 = faceTetraIndex( 1, idFaceM );
			idTetM2 = faceTetraIndex( 2, idFaceM );
			
			faceArM = faceArea( idFaceM );
			
			kDiffN = kontrastDiff( idFaceN );
			
			% +
			
			tetVolM1 = tetraVol( idTetM1 );
			C34( idFaceM, 1, idFaceN ) = - K * faceArM * kDiffN / tetVolM1;
			
			% -
			
			if idTetM2 ~= -1
				tetVolM2 = tetraVol( idTetM2 );
				C34( idFaceM, 2, idFaceN ) = - K * faceArM * kDiffN / tetVolM2;
			end
			
			
		end
	end
	
end

