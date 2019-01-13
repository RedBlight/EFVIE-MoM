function [ C21 ] = GenerateC21( angFreq, faceCount, faceTetraIndex, kontrast, faceArea, tetraVol )
	
	% C21( m, a, n, b )
	% a := { 1, 2 }
	% b := { 1, 2 }
	
	C21 = zeros( faceCount, 2, faceCount, 2 );
	
	K = angFreq * angFreq * M.mu0 / ( 36 * pi );
	
	for idFaceM = 1 : faceCount
		for idFaceN = 1 : faceCount
			
			idTetM1 = faceTetraIndex( 1, idFaceM );
			idTetM2 = faceTetraIndex( 2, idFaceM );
			
			idTetN1 = faceTetraIndex( 1, idFaceN );
			idTetN2 = faceTetraIndex( 2, idFaceN );
			
			faceArM = faceArea( idFaceM );
			faceArN = faceArea( idFaceN );
			
			% ++
			
			tetVolM1 = tetraVol( idTetM1 );
			tetVolN1 = tetraVol( idTetN1 );
			konN1 = kontrast( idTetN1 );
			C21( idFaceM, 1, idFaceN, 1 ) = K * konN1 * faceArN * faceArM / ( tetVolM1 * tetVolN1 );
			
			% +-
			
			if idTetN2 ~= -1
				tetVolN2 = tetraVol( idTetN2 );
				konN2 = kontrast( idTetN2 );
				C21( idFaceM, 1, idFaceN, 2 ) = K * konN2 * faceArN * faceArM / ( tetVolM1 * tetVolN2 );
			end
			
			% -+
			
			if idTetM2 ~= -1
				tetVolM2 = tetraVol( idTetM2 );
				C21( idFaceM, 2, idFaceN, 1 ) = K * konN1 * faceArN * faceArM / ( tetVolM2 * tetVolN1 );
			end
			
			% --
			
			if idTetM2 ~= -1 && idTetN2 ~= -1
				C21( idFaceM, 2, idFaceN, 2 ) = K * konN2 * faceArN * faceArM / ( tetVolM2 * tetVolN2 );
			end
			
		end
	end
	
end

